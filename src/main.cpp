#include "main.h"

int main(int argc, char *argv[]) {
    if (argc < 8) {
        std::cerr << "Need at least 5 parameters: testCaseName, measurementDelim, runs, input_data, dimensions, delimiter, epsilon, minPts" << std::endl;
        return 1;
    }
    std::string testCaseName = std::string(argv[1]);
    char measurementDelim = argv[2][0];
    int runs = std::stoi(argv[3], nullptr, 10);

    std::string dataFile(argv[4]);
    int dimensions = std::stoi(argv[5], nullptr, 10);
    char delim = argv[6][0];
    float epsilon = std::stof(argv[7], nullptr);
    int minPts = std::stoi(argv[8], nullptr, 10);

    for (int i = 0; i < 2; i++) {
        DBSCAN::dbscan(dataFile, dimensions, delim, epsilon, minPts, false);
    }

    std::chrono::high_resolution_clock::time_point startTime, endTime;
    std::cerr << testCaseName;
    for (int i = 0; i < runs; i++) {
        startTime = std::chrono::high_resolution_clock::now();
        DBSCAN::dbscan(dataFile, dimensions, delim, epsilon, minPts);
        endTime = std::chrono::high_resolution_clock::now();

        long long int timeMS = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        std::cerr << measurementDelim << timeMS;
    }
    std::cerr << std::endl;
    return 0;
}