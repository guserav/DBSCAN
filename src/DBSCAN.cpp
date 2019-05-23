#include "DBSCAN.h"

void DBSCAN::dbscan(const std::string& filename, unsigned int dimensions, char delim, float epsilon, int minPts, bool writeToConsole){
    epsilon = epsilon * epsilon; // As euclidian distance is used this saves a lot of sqrt calls TODO discuss in paper
    std::string line;
    std::ifstream file(filename);
    if(file.peek() == std::ifstream::traits_type::eof()){
        throw std::runtime_error("Couldn't read from file");
    }
    std::vector<DataPointFloat> datapoints;
    std::getline(file, line);
    datapoints.emplace_back(line, dimensions, delim);
    Rtree tree(dimensions, datapoints.data());

    while (std::getline(file, line)){
        if(!line.empty()){
            datapoints.emplace_back(line, dimensions, delim);
            tree.addDataPoint(datapoints.data() + datapoints.size() - 1);
        }
    }

    int maxCluster = 0;
    for(DataPointFloat& seed : datapoints){
        if(seed.isUnClassified()) {
            std::list<DataPointFloat*> toDiscover = seed.getNeighbours(epsilon);
            if(toDiscover.size() < minPts) {
                seed.setCluster(NOISE);
                continue;
            }
            int currentCluster = ++maxCluster;
            seed.setCluster(currentCluster);
            DataPointFloat* currentNode;
            while(!toDiscover.empty()) {
                currentNode = toDiscover.front(); // Do breath first as this should avoid more collisions in parallel case
                toDiscover.pop_front();
                if(currentNode->isNoise()){
                    currentNode->setCluster(currentCluster);
                    continue;
                }
                if(currentNode->isUnClassified()){
                    currentNode->setCluster(currentCluster);
                    std::list<DataPointFloat*> neighbours = currentNode->getNeighbours(epsilon);
                    if(neighbours.size() < minPts) continue;
                    for(DataPointFloat* point : neighbours) {
                        toDiscover.push_back(point);
                    }
                }
#ifdef _DEBUG
                if(currentNode->getCluster() != currentCluster) {
                    throw std::runtime_error("This should be in the current cluster right now");
                }
#endif
            }
        }
    }
    if(writeToConsole) {
        for(DataPointFloat& point : datapoints){
            point.printToConsoleWithCluster();
        }
    }
}
