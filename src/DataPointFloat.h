#ifndef STUDIENARBEITCODE_DATAPOINTFLOAT_H
#define STUDIENARBEITCODE_DATAPOINTFLOAT_H

class DataPointFloat;

#include <stdlib.h>
#include <stdexcept>
#include <string>

#include "RtreeNode.h"

class DataPointFloat {
public:
    DataPointFloat(const std::string& input, unsigned int dimensions, char delim);
    DataPointFloat(const DataPointFloat& obj); // copy constructor
    DataPointFloat(DataPointFloat&& obj) noexcept; // move constructor
    DataPointFloat& operator=(const DataPointFloat& obj); //copy assignment
    DataPointFloat& operator=(DataPointFloat&& obj) noexcept; // move assignment constructor
    float operator[](unsigned int index);
    ~DataPointFloat();

    unsigned int getDimensions();

private:
    float* data;
    int cluster;
    unsigned int dimensions;
    RtreeNode * parent;
};


#endif //STUDIENARBEITCODE_DATAPOINTFLOAT_H
