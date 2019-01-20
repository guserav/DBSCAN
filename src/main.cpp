#include <fstream>
#include "main.h"

void dbscan(const char* filename, int dimensions, char delim){
    std::string line;
    std::ifstream file(filename);
    if(file.peek() == std::ifstream::traits_type::eof()){
        throw std::runtime_error("Couldn't read from file");
    }
    std::vector<DataPointFloat> datapoints;
    while (std::getline(file, line)){
        if(!line.empty()){
            datapoints.emplace_back(DataPointFloat(line, dimensions, delim));
        }
    }
    std::cout << "Done reading file" << std::endl;
}

int main() {
    dbscan("../irisShuffeled.data", 4, ';');
    return 0;
}