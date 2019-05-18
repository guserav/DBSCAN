#include "DBSCAN.h"

void DBSCAN::dbscan(const char* filename, unsigned int dimensions, char delim){
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
}