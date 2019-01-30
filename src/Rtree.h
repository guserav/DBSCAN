#ifndef STUDIENARBEITCODE_RTREE_H
#define STUDIENARBEITCODE_RTREE_H

#include "RtreeNode.h"

class Rtree {
public:
    Rtree(unsigned int dimensions, DataPointFloat* firstDataPoint);
    Rtree(const Rtree& obj) = delete; // copy constructor
    Rtree(Rtree&& obj) noexcept; // move constructor
    Rtree& operator=(const Rtree& obj) = delete; //copy assignment
    Rtree& operator=(Rtree&& obj) noexcept; // move assignment constructor
    ~Rtree();

    void addDataPoint(DataPointFloat * point);
private:
    unsigned int dimensions;
    RtreeNode * root; // This object is not longer valid if root is nullptr
};


#endif //STUDIENARBEITCODE_RTREE_H