#include "main.h"

int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::cerr << "Need at least 5 parameters: input_data, dimensions, delimiter, epsilon, minPts" << std::endl;
        return 1;
    }
    std::string dataFile(argv[1]);
    int dimensions = std::stoi(argv[2], nullptr, 10);
    char delim = argv[3][0];
    float epsilon = std::stof(argv[4], nullptr);
    int minPts = std::stoi(argv[5], nullptr, 10);
    DBSCAN::dbscan(dataFile, dimensions, delim, epsilon, minPts);
    return 0;
}