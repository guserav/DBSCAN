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

    unsigned int getDimensions();
    float* getData();
    bool isUnClassified();
    bool isNoise();
    float getDistance(DataPointFloat *pFloat);
    void setCluster(int cluster);
    int getCluster();

    void printToConsole(int level);
    void printForVisualisation(int level);
    void printToConsoleWithCluster();
    bool seen();

private:
    float* data;
    int cluster = UNCLASSIFIED;
    bool isSeen = false;
    unsigned int dimensions;
    void destruct();
};


#endif //STUDIENARBEITCODE_DATAPOINTFLOAT_H
