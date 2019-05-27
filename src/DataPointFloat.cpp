#include "DataPointFloat.h"

DataPointFloat::DataPointFloat(std::string input, unsigned int dimensions, char delim) {
    size_t endPos;
    std::string item;

    this->dimensions = dimensions;
    data = new float[dimensions];

    for(int i = 0; i < dimensions - 1; i++){
        endPos = input.find(delim);
        if(endPos == std::string::npos) throw std::invalid_argument("Can't parse input couldn't find delim in input");
        item = input.substr(0, endPos);
        input = input.substr(endPos + 1, std::string::npos);
        data[i] = std::stof(item);
    }
    data[dimensions - 1] = std::stof(input);
}

DataPointFloat::~DataPointFloat() {
    destruct();
}

/**
 * Free all used resources and clear references to this object.
 *
 * Should only be used by move constructors and destructor.
 */
void DataPointFloat::destruct(){
    delete [] data;
}

DataPointFloat &DataPointFloat::operator=(const DataPointFloat& obj) {
    if(this != &obj){
        destruct();

        dimensions = obj.dimensions;
        cluster = obj.cluster;
        data = new float[dimensions];
        std::copy(obj.data, obj.data + dimensions, data);
        isSeen = obj.isSeen;
    }
    return *this;
}

DataPointFloat &DataPointFloat::operator=(DataPointFloat&& obj) noexcept{
    if(this != &obj){
        destruct();

        dimensions = obj.dimensions;
        cluster = obj.cluster;
        data = obj.data;
        isSeen = obj.isSeen;

        obj.dimensions = 0;
        obj.cluster = 0;
        obj.data = nullptr;
    } else {
#ifdef _DEBUG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wterminate"
        throw std::runtime_error("Can't move copy from this");
#pragma GCC diagnostic pop
#endif
    }
    return *this;
}

DataPointFloat::DataPointFloat(const DataPointFloat &obj) : dimensions(obj.dimensions), cluster(obj.cluster), data(new float[obj.dimensions]), isSeen(obj.isSeen) {
    std::copy(obj.data, obj.data + dimensions, data);
}

DataPointFloat::DataPointFloat(DataPointFloat &&obj) noexcept: dimensions(obj.dimensions), cluster(obj.cluster), data(obj.data), isSeen(obj.isSeen) {
    if(this != &obj) {
        obj.dimensions = 0;
        obj.cluster = 0;
        obj.data = nullptr;
    } else {
#ifdef _DEBUG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wterminate"
        throw std::runtime_error("Can't move copy from this");
#pragma GCC diagnostic pop
#endif
    }
}

/**
 * Non safe access of index. value of the data point.
 * If BEBUG ist set this is safe.
 */
float DataPointFloat::operator[](unsigned int index) const{
#ifdef _DEBUG
    if( index < 0 || index >= this->dimensions) {
        throw std::invalid_argument("index: " + std::to_string(index) + " is out of range of dimensions " + std::to_string(this->dimensions));
    }
#endif
    return this->data[index];
}

unsigned int DataPointFloat::getDimensions() const{
    return this->dimensions;
}

float *DataPointFloat::getData() {
    return data;
}

void DataPointFloat::printToConsole(int level) {
    for(int i = 0; i < level; i++) {
        std::cout << "    ";
    }
    if(this->dimensions <= 0) {
        std::cout << "[]\n";
        return;
    }

    std::cout << "[";
    for(int i = 0; i < this->dimensions - 1; i++) {
        std::cout << std::to_string(data[i]) << ", ";
    }
    std::cout << std::to_string(data[this->dimensions - 1]) << "]\n";
}

void DataPointFloat::printForVisualisation(int level) {
    std::cout << std::to_string(level) << ":[" << std::to_string(data[0]);
    for(int i = 1; i < this->dimensions; i++){
        std::cout << ";" << std::to_string(data[i]);
    }
    std::cout << "]\n";
}

void DataPointFloat::printToConsoleWithCluster(){
    for(int i = 0; i < this->dimensions; i++){
        std::cout << std::to_string(data[i]) << ";" ;
    }
    std::cout << std::to_string(this->cluster) << std::endl;
}

bool DataPointFloat::isUnClassified() const{
    return this->cluster == UNCLASSIFIED;
}

/**
 * Returns the distance squared to that point
 * @param pFloat the point to get the distance to
 * @return squared distance as float
 */
float DataPointFloat::getDistance(DataPointFloat *pFloat) const{
    float distance = 0.0f;
    for(int i = 0; i < this->dimensions; i++){
        distance += std::pow((*pFloat)[i] - (*this)[i], 2);
    }
    return distance;
}

void DataPointFloat::setCluster(int cluster) {
    this->cluster = cluster;
}

int DataPointFloat::getCluster() const{
    return this->cluster;
}

bool DataPointFloat::isNoise() const{
    return this->cluster == NOISE;
}

/**
 * Indicates whether the object hast been seen before and marks it as seen.
 *
 * This means after calling this function it will be asumend that this object is in toDiscover or has an asigned cluster to it.
 * @return whether it has been seen before.
 */
bool DataPointFloat::seen() {
    if(this->isSeen){
        return true;
    }
    this->isSeen = true;
    return false;
}
