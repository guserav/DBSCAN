#include "DataPointFloat.h"

DataPointFloat::DataPointFloat(const std::string& input, unsigned int dimensions, char delim) {
    size_t endPos;
    std::string cut = input;
    std::string item;

    cluster = 0;
    parent = nullptr;
    this->dimensions = dimensions;
    data = new float[dimensions];

    for(int i = 0; i < dimensions - 1; i++){
        endPos = input.find(delim);
        if(endPos == std::string::npos) throw std::invalid_argument("Can't parse input couldn't find delim in input");
        item = cut.substr(0, endPos);
        cut = cut.substr(endPos + 1, std::string::npos);
        data[i] = std::stof(item);
    }
    data[dimensions - 1] = std::stof(cut);
}

DataPointFloat::~DataPointFloat() {
    delete [] data;
    if(this->parent) {
        this->parent->removePoint(this);
    }
}

DataPointFloat &DataPointFloat::operator=(const DataPointFloat& obj) {
    if(this != &obj){
        delete [] data;

        dimensions = obj.dimensions;
        cluster = obj.cluster;
        data = new float[dimensions];
        std::copy(obj.data, obj.data + dimensions, data);
        parent = nullptr;
    }
    return *this;
}

DataPointFloat &DataPointFloat::operator=(DataPointFloat&& obj) noexcept{
    if(this != &obj){
        dimensions = obj.dimensions;
        cluster = obj.cluster;
        data = obj.data;
        parent = obj.parent;

        if(parent){
            parent->replaceNode(&obj, this);
        }

        obj.dimensions = 0;
        obj.cluster = 0;
        obj.data = nullptr;
        obj.parent = nullptr;
    } else {
#ifdef _DEBUG
        throw std::runtime_error("Can't move copy from this");
#endif
    }
    return *this;
}

DataPointFloat::DataPointFloat(const DataPointFloat &obj) : dimensions(obj.dimensions), cluster(obj.cluster), data(new float[obj.dimensions]), parent(nullptr) {
    std::copy(obj.data, obj.data + dimensions, data);
}

DataPointFloat::DataPointFloat(DataPointFloat &&obj) noexcept: dimensions(obj.dimensions), cluster(obj.cluster), data(obj.data), parent(obj.parent) {
    if(this != &obj) {
        obj.dimensions = 0;
        obj.cluster = 0;
        obj.data = nullptr;
        obj.parent = nullptr;

        if(parent){
            parent->replaceNode(&obj, this);
        }
    } else {
#ifdef _DEBUG
        throw std::runtime_error("Can't move copy from this");
#endif
    }
}

/**
 * Non safe access of index. value of the data point.
 * If BEBUG ist set this is safe.
 */
float DataPointFloat::operator[](unsigned int index) {
#ifdef _DEBUG
    if( index < 0 || index > this->dimensions) {
        throw std::invalid_argument("index: " + std::to_string(index) + " is out of range of dimensions " + std::to_string(this->dimensions));
    }
#endif
    return this->data[index];
}

unsigned int DataPointFloat::getDimensions() {
    return this->dimensions;
}

