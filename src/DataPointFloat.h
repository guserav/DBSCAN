#ifndef STUDIENARBEITCODE_DATAPOINTFLOAT_H
#define STUDIENARBEITCODE_DATAPOINTFLOAT_H


#include <stdlib.h>
#include <stdexcept>
#include <string>

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
};


#endif //STUDIENARBEITCODE_DATAPOINTFLOAT_H
