//
// Created by zeiske on 26.01.2019.
//

#include "RtreeNode.h"

RtreeNode::RtreeNode(unsigned int dimensions): dimensions(dimensions) {
    for(int i=0; i < R_TREE_NUMBER_CHIELDS; i++){
        childNodes[i] = nullptr;
        childLeaves[i] = nullptr;
    }
    minBoundaries = new float[dimensions];
    maxBoundaries = new float[dimensions];
    for(int i=0; i < dimensions; i++){
        minBoundaries[i] = NAN;
        maxBoundaries[i] = NAN;
    }
}

RtreeNode::~RtreeNode() {
    delete [] minBoundaries;
    delete [] maxBoundaries;
}

RtreeNode *RtreeNode::insertNewPoint(DataPointFloat *dataPoint) {
    if(hasLeaves()) {
        return this->addLeaveChild(dataPoint);
    }

    // Find child node to insert into
    // Insert node
    // On split of child add Child to this node
    // Split if necessary
    NOT_YET_IMPLEMENTED("Insert New Point");
    return nullptr;
}

bool RtreeNode::hasLeaves() {
    if(this->childNodes[0] != nullptr) return false;
    if(this->childLeaves[0] != nullptr) return true;
    throw std::runtime_error("R_Tree_Node not fully initialised at least one child should be included before using this function");
}

void RtreeNode::addChild(RtreeNode *child) {
    this->childNodes[this->childCount++] = child;
    NOT_YET_IMPLEMENTED("Expand Rectangle")
}

RtreeNode* RtreeNode::addLeaveChild(DataPointFloat *child) {
    if(childCount < R_TREE_NUMBER_CHIELDS) {
        this->childLeaves[this->childCount++] = child;
        NOT_YET_IMPLEMENTED("Expand Rectangle")
        return nullptr;
    } else {
        NOT_YET_IMPLEMENTED("Split Node with leaves");
    }
}

