//
// Created by zeiske on 26.01.2019.
//

#include "RtreeNode.h"
void RtreeNode::init() {
    for(int i=0; i < R_TREE_NUMBER_CHIELDS; i++){
        childNodes[i] = nullptr;
        childLeaves[i] = nullptr;
    }
    minBoundaries = new float[this->dimensions];
    maxBoundaries = new float[this->dimensions];
}

RtreeNode::RtreeNode(unsigned int dimensions): dimensions(dimensions) {
    this->init();
    for(int i=0; i < dimensions; i++){
        this->minBoundaries[i] = NAN;
        this->maxBoundaries[i] = NAN;
    }
}

RtreeNode::RtreeNode(RtreeNode *firstChild): dimensions(firstChild->dimensions) {
    this->init();
    for(int i=0; i < dimensions; i++){
        this->minBoundaries[i] = firstChild->minBoundaries[i];
        this->maxBoundaries[i] = firstChild->maxBoundaries[i];
    }
}

RtreeNode::RtreeNode(DataPointFloat *firstChild): dimensions(firstChild->getDimensions()) {
    this->init();
    for(int i=0; i < dimensions; i++){
        this->minBoundaries[i] = (*firstChild)[i];
        this->maxBoundaries[i] = (*firstChild)[i];
    }
}

RtreeNode::~RtreeNode() {
    delete [] minBoundaries;
    delete [] maxBoundaries;
    for(int i=0; i < R_TREE_NUMBER_CHIELDS; i++){
        delete childNodes[i];
        delete childLeaves[i];
    }
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

/**
 * Adds a normal child to this node.
 *
 * This functions assumes that there are allready other children and the min/max boundaries are proberly set
 * @param child  the child node to add.
 * @return  A new node if a split occured
 */
void RtreeNode::addChild(RtreeNode *child) {
    if(childCount < R_TREE_NUMBER_CHIELDS) {
        this->childNodes[this->childCount++] = child;
        for(int i=0; i < this->dimensions; i++){
            if(this->maxBoundaries[i] < child->maxBoundaries[i]) {
                this->maxBoundaries[i] = child->maxBoundaries[i];
            } else if(this->minBoundaries[i] > child->minBoundaries[i]) {
                this->minBoundaries[i] = child->minBoundaries[i];
            }
        }
    } else {
        NOT_YET_IMPLEMENTED("Split Node without leaves");
    }
}

/**
 * Adds a Leave child to this node.
 *
 * This functions assumes that there are allready other children and the min/max boundaries are proberly set
 * @param child  the child data Point to add.
 * @return  A new node if a split occured
 */
RtreeNode* RtreeNode::addLeaveChild(DataPointFloat *child) {
    if(childCount < R_TREE_NUMBER_CHIELDS) {
        this->childLeaves[this->childCount++] = child;
        for(int i=0; i < this->dimensions; i++){
            if(this->maxBoundaries[i] < (*child)[i]) {
                this->maxBoundaries[i] = (*child)[i];
            } else if(this->minBoundaries[i] > (*child)[i]) {
                this->minBoundaries[i] = (*child)[i];
            }
        }
        //TODO think about calculating volume within the loop above (performance, but maybe not thread safe)
        //TODO also consider not calculating volume here as this function maybe used in fast paste for adding multiple children at the same time
        this->calculateVolume();
        return nullptr;
    } else {
        NOT_YET_IMPLEMENTED("Split Node with leaves");
    }
}

/**
 * Calculate the n-dimensional volume of this Node.
 * In the case that there are not at least two childs the function will fail as it is not expected that this will fail.
 * It should never happen that this node has one node as child and needs its volume
 * This function is currently not threadsafe (TODO)
 */
void RtreeNode::calculateVolume() {
    if(this->childCount > 1){
        this->volume = 1.0f;
        for(int i=0; i < this->dimensions; i++){
            this->volume *= this->maxBoundaries[i] - this->maxBoundaries[i];
        }
    } else {
        throw std::runtime_error("Can't calculate volume for only one child.");
    }
}
