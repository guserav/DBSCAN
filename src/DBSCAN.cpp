#include "DBSCAN.h"

void DBSCAN::dbscan(const std::string& filename, unsigned int dimensions, char delim, float epsilon, int minPts, bool writeToConsole){
    epsilon = epsilon * epsilon; // As euclidian distance is used this saves a lot of sqrt calls TODO discuss in paper
    std::string line;
    std::ifstream file(filename);
    if(file.peek() == std::ifstream::traits_type::eof()){
        throw std::runtime_error("Couldn't read from file");
    }
    std::vector<DataPointFloat> datapoints;

    while (std::getline(file, line)){
        if(!line.empty()){
            datapoints.emplace_back(line, dimensions, delim);
        }
    }

    Rtree tree(dimensions, datapoints.data());
    for(int i=1; i < datapoints.size(); i++){
        tree.addDataPoint(datapoints.data() + i);
    }

#ifdef _DEBUG
    tree.checkIntegrity();
#endif

    std::mutex toDiscoverMutex;
    std::condition_variable toDiscoverMoreEntries;
    int maxCluster = 0;
    for(DataPointFloat& seed : datapoints){
        if(seed.isUnClassified()) {
            std::list<DataPointFloat*> toDiscover = tree.getNeighbours(&seed, epsilon);
            if(toDiscover.size() < minPts + 1) { //getNeighbours includes the point itself
                seed.setCluster(NOISE);
                continue;
            }
            int const currentCluster = ++maxCluster;
            seed.setCluster(currentCluster);
            for(DataPointFloat*& point : toDiscover){
                point->seen();
            }
            std::atomic<int> threadsRunning = 0;
            bool toDiscoverFinalEmpty = false;
#pragma omp parallel shared(toDiscover, threadsRunning, toDiscoverMutex, toDiscoverFinalEmpty, toDiscoverMoreEntries) firstprivate(currentCluster)
            {
                threadsRunning.fetch_add(1, std::memory_order_release);

                DataPointFloat* currentNode;
                while (true) {
                    {
                        std::unique_lock<std::mutex> lock(toDiscoverMutex);
                        if (toDiscover.empty()) {
#ifdef _DEBUG
                            if(threadsRunning <= 0) {
                                throw std::runtime_error("Wrong atomic order somewhere");
                            }
#endif
                            // Wait for new items arriving in list this. If this thread is the last to go to sleep it has to wake all others to terminate
                            if(threadsRunning.load(std::memory_order_acquire) == 1) {
                                //This is the last running thread and toDiscover is empty
                                toDiscoverFinalEmpty = true;
                                toDiscoverMoreEntries.notify_all();
                                break;
                            } else {
                                threadsRunning.fetch_sub(1, std::memory_order_release);
                                while(toDiscover.empty() && !toDiscoverFinalEmpty) {
                                    toDiscoverMoreEntries.wait(lock);
                                }
                                if(toDiscoverFinalEmpty) break;
                                threadsRunning.fetch_add(1, std::memory_order_release);
                            }
                        }
                        currentNode = toDiscover.front(); // Do breath first as this should avoid more collisions in parallel case
                        toDiscover.pop_front();
                    }
                    if (currentNode->isNoise()) {
                        currentNode->setCluster(currentCluster);
                        continue;
                    }
                    if (currentNode->isUnClassified()) {
                        currentNode->setCluster(currentCluster);
                        std::list<DataPointFloat *> neighbours = tree.getNeighbours(currentNode, epsilon);
                        if (neighbours.size() < minPts + 1) continue;
                        std::lock_guard<std::mutex> lock(toDiscoverMutex);
                        for (DataPointFloat *point : neighbours) {
                            if (!point->seen()) {
                                toDiscover.push_back(point);
                            }
                        }
                        toDiscoverMoreEntries.notify_all();
                    }
#ifdef _DEBUG
                    if (currentNode->getCluster() != currentCluster) {
                        throw std::runtime_error("This should be in the current cluster right now");
                    }
#endif
                }
            }
        }
    }
    if(writeToConsole) {
        for(DataPointFloat& point : datapoints){
            point.printToConsoleWithCluster();
        }
    }
}
