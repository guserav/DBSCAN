#ifndef STUDIENARBEITCODE_DBSCAN_H
#define STUDIENARBEITCODE_DBSCAN_H

#include <iostream>
#include <vector>
#include <fstream>
#include "DataPointFloat.h"
#include "Rtree.h"

namespace DBSCAN {
    void dbscan(const char* filename, unsigned int dimensions, char delim);
}


#endif //STUDIENARBEITCODE_DBSCAN_H
