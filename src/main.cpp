#include "main.h"

int main(int argc, char *argv[]) {
    if (argc < 7) {
        std::cerr << "Need at least 5 parameters: testCaseName, runs, input_data, dimensions, delimiter, epsilon, minPts" << std::endl;
        return 1;
    }
    std::string testCaseName = std::string(argv[1]);
    int runs = std::stoi(argv[2], nullptr, 10);

    std::string dataFile(argv[3]);
    int dimensions = std::stoi(argv[4], nullptr, 10);
    char delim = argv[5][0];
    float epsilon = std::stof(argv[6], nullptr);
    int minPts = std::stoi(argv[7], nullptr, 10);

    for (int i = 0; i < 2; i++) {
        DBSCAN::dbscan(dataFile, dimensions, delim, epsilon, minPts);
    }

    std::chrono::high_resolution_clock::time_point startTime, endTime;
    std::cerr << testCaseName;
    for (int i = 0; i < runs; i++) {
        startTime = std::chrono::high_resolution_clock::now();
        DBSCAN::dbscan(dataFile, dimensions, delim, epsilon, minPts);
        endTime = std::chrono::high_resolution_clock::now();

        long long int timeMS = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        std::cerr << delim << timeMS;
    }
    std::cerr << std::endl;
    return 0;
}