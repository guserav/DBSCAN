#ifndef STUDIENARBEITCODE_DBSCAN_H
#define STUDIENARBEITCODE_DBSCAN_H

#define UNCLASSIFIED -1
#define NOISE 0

#include <iostream>
#include <vector>
#include <fstream>
#include <list>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <omp.h>
#include "DataPointFloat.h"
#include "Rtree.h"

namespace DBSCAN {
    void dbscan(const std::string& filename, unsigned int dimensions, char delim, float epsilon, int minPts, bool writeToConsole=true);
}


#endif //STUDIENARBEITCODE_DBSCAN_H
