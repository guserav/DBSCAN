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

    const unsigned int size = datapoints.size();
    std::mutex clusterMappingLock;
    std::vector<int> clusterMapping;
    clusterMapping.push_back(0);
#pragma omp parallel for shared(clusterMapping, clusterMappingLock, datapoints) firstprivate(size)
    for(int i=0; i < size; i++){
        DataPointFloat * seed = (datapoints.data() + i);
        if(seed->isUnClassified()) {
            std::list<DataPointFloat*> toDiscover = tree.getNeighbours(seed, epsilon);
            if(toDiscover.size() < minPts + 1) { //getNeighbours includes the point itself
                int cluster = seed->setCluster(NOISE);
                if(cluster != UNCLASSIFIED) {
                    seed->setCluster(cluster);
                }
                continue;
            }
            int currentCluster = -1;
            {
                std::lock_guard<std::mutex> lock(clusterMappingLock);
                clusterMapping.push_back(clusterMapping.size());
                currentCluster = clusterMapping.size() - 1;
                const int prevCluster = seed->setCluster(currentCluster);
                if(prevCluster != UNCLASSIFIED) {
                    clusterMapping[currentCluster] = clusterMapping[prevCluster];
                    continue;
                }
            }
            for(DataPointFloat*& point : toDiscover){
                point->seen();
            }
            DataPointFloat* currentNode;
            while(!toDiscover.empty()) {
                currentNode = toDiscover.front(); // Do breath first as this should avoid more collisions in parallel case
                toDiscover.pop_front();
                const int previousCluster = currentNode->setCluster(currentCluster);
                if (previousCluster == NOISE) { continue; }
                if (previousCluster == UNCLASSIFIED) {
                    std::list<DataPointFloat *> neighbours = tree.getNeighbours(currentNode, epsilon);
                    if (neighbours.size() < minPts + 1) continue;
                    for (DataPointFloat *point : neighbours) {
                        if (point->seen()) {
                            const int otherCluster = point->getCluster();
                            if (currentCluster != otherCluster && otherCluster != NOISE &&
                                otherCluster != UNCLASSIFIED) {
                                std::lock_guard<std::mutex> lock(clusterMappingLock);
                                if (currentCluster < otherCluster) {
                                    if (currentCluster <= clusterMapping[otherCluster]) {
                                        clusterMapping[otherCluster] = currentCluster;
                                    } else {
                                        currentCluster = (clusterMapping[currentCluster] = clusterMapping[otherCluster]);
                                    }
                                } else {
                                    currentCluster = (clusterMapping[currentCluster] = clusterMapping[otherCluster]);
                                }
                            }
                        } else {
                            toDiscover.push_back(point);
                        }
                    }
                }
            }
        }
    }
    for(int i=0; i < clusterMapping.size(); i++) {
        clusterMapping[i] = clusterMapping[clusterMapping[i]];
    }
    if(writeToConsole) {
        for(DataPointFloat& point : datapoints){
            point.printToConsoleWithCluster(clusterMapping);
        }
    }
}
