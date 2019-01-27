#ifndef STUDIENARBEITCODE_RTREENODE_H
#define STUDIENARBEITCODE_RTREENODE_H

#include "DataPointFloat.h"
#include <exception>
#include <cmath>

// Use odd number to avoid edge case for insertion
#define R_TREE_NUMBER_CHIELDS 11
#define NOT_YET_IMPLEMENTED(desc) throw std::runtime_error("Not yet implemented: " desc);

class RtreeNode {
public:
    RtreeNode(const RtreeNode&) = delete;
    RtreeNode(RtreeNode&&) = delete;
    RtreeNode& operator=(const RtreeNode&) = delete;
    RtreeNode& operator=(RtreeNode&&) = delete;

    explicit RtreeNode(unsigned int dimensions);
    explicit RtreeNode(RtreeNode * firstChild);
    explicit RtreeNode(DataPointFloat* firstChild);
    ~RtreeNode();

    RtreeNode * insertNewPoint(DataPointFloat* dataPoint);
    bool hasLeaves();
private:
    void init();
    void addChild(RtreeNode* child);
    RtreeNode* addLeaveChild(DataPointFloat* child);
    void calculateVolume();

    char childCount = 0;
    float volume = 0;
    RtreeNode * parent = nullptr;
    unsigned int dimensions;
    float * minBoundaries;
    float * maxBoundaries;
    RtreeNode* childNodes[R_TREE_NUMBER_CHIELDS];
    DataPointFloat* childLeaves[R_TREE_NUMBER_CHIELDS];
};


#endif //STUDIENARBEITCODE_RTREENODE_H