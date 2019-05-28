#ifndef STUDIENARBEITCODE_DATAPOINTFLOAT_H
#define STUDIENARBEITCODE_DATAPOINTFLOAT_H

class DataPointFloat;

#include "DBSCAN.h"
#include <stdlib.h>
#include <stdexcept>
#include <string>

#include "RtreeNode.h"

class DataPointFloat {
public:
    DataPointFloat(std::string input, unsigned int dimensions, char delim);
    DataPointFloat(const DataPointFloat& obj); // copy constructor
    DataPointFloat(DataPointFloat&& obj) noexcept; // move constructor
    DataPointFloat& operator=(const DataPointFloat& obj); //copy assignment
    DataPointFloat& operator=(DataPointFloat&& obj) noexcept; // move assignment constructor
    float operator[](unsigned int index);
    ~DataPointFloat();

    inline unsigned int getDimensions() __attribute__((always_inline));
    inline float* getData() __attribute__((always_inline));
    inline bool isUnClassified() __attribute__((always_inline));
    inline bool isNoise() __attribute__((always_inline));
    inline float getDistance(DataPointFloat *pFloat) __attribute__((always_inline));
    inline void setCluster(int cluster) __attribute__((always_inline));
    inline int getCluster() __attribute__((always_inline));
    inline bool seen() __attribute__((always_inline));

    void printToConsole(int level);
    void printForVisualisation(int level);
    void printToConsoleWithCluster();

private:
    float* data;
    int cluster = UNCLASSIFIED;
    bool isSeen = false;
    unsigned int dimensions;
    void destruct();
};


#endif //STUDIENARBEITCODE_DATAPOINTFLOAT_H
