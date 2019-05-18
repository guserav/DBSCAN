#include "main.h"

int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::cerr << "Need at least 5 parameters: input_data, dimensions, delimiter, epsilon, minPts" << std::endl;
        return 1;
    }
    DBSCAN::dbscan(argv[1], std::stoi(argv[2], nullptr, 10), argv[3][0], std::stof(argv[4], nullptr), std::stoi(argv[5], nullptr, 10));
    return 0;
}