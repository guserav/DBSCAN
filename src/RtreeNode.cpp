#include "RtreeNode.h"
void RtreeNode::init() {
    for(int i=0; i < R_TREE_NUMBER_CHILDS; i++){
        childNodes[i] = nullptr;
        childLeaves[i] = nullptr;
    }
    minBoundaries = new float[this->dimensions];
    maxBoundaries = new float[this->dimensions];
}

/**
 * This should only be used for the first node of the Tree and then probably RtreeNode(DataPointFloat *firstChild) can be used.
 */
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

RtreeNode::RtreeNode(RtreeNode *firstChild, RtreeNode *secondChild): RtreeNode(firstChild) {
    this->addChild(secondChild);
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
    for(int i=0; i < R_TREE_NUMBER_CHILDS; i++){
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
 * This should only be called when a split happened and is therefor private.
 * @param child  the child node to add.
 * @return  A new node if a split occured
 */
void RtreeNode::addChild(RtreeNode *child) {
    if(childCount < R_TREE_NUMBER_CHILDS) {
        this->childNodes[this->childCount++] = child;
        for(int i=0; i < this->dimensions; i++){
            if(this->maxBoundaries[i] < child->maxBoundaries[i]) {
                this->maxBoundaries[i] = child->maxBoundaries[i];
            } else if(this->minBoundaries[i] > child->minBoundaries[i]) {
                this->minBoundaries[i] = child->minBoundaries[i];
            }
        }
        //TODO think about calculating volume in the code above on extension? This may result in better performance
        this->calculateVolume();
    } else {
        float sumMargins[this->dimensions];
        float minDimValue, maxDimValue;
        bool bubbleSortChangedMarker;
        int splitDim = 0;
        // Container for all Childs to better sort them
        RtreeNode * allCurrentChilds[R_TREE_NUMBER_CHILDS + 1];
        for(int i=0; i < R_TREE_NUMBER_CHILDS; i++) {
            allCurrentChilds[i] = this->childNodes[i];
        }
        allCurrentChilds[R_TREE_NUMBER_CHILDS] = child;

        for(int d=0; d < this->dimensions; d++){
            sumMargins[d] = 0;
            //Sort by the minimum boundary allong this axis
            //TODO For now a bubble sort... Maybe change this later on
            bubbleSortChangedMarker = true;
            for(int i = 0; i < R_TREE_NUMBER_CHILDS + 1 && changed; i++){
                bubbleSortChangedMarker = false;
                for(int j=0; j < R_TREE_NUMBER_CHILDS + 1 - i; j++) {
                    if(allCurrentChilds[i]->minBoundaries[d] > allCurrentChilds[j]->minBoundaries[d]) {
                        std::swap(allCurrentChilds[i], allCurrentChilds[j]);
                        bubbleSortChangedMarker = true;
                    }
                }
            }
            //Calculate the sum of all margins for all possible splits allong the axis d for the minimum Boundaries
            for(int k=R_TREE_MINIMUM_CHILDS - 1; k < R_TREE_NUMBER_CHILDS - R_TREE_MINIMUM_CHILDS; k++){
                for(int i=0; i <= this->dimensions; i++){ //TODO in case of i == d the finding of minDim can be optimised...
                    minDimValue = allCurrentChilds[0]->minBoundaries[i];
                    maxDimValue = allCurrentChilds[0]->maxBoundaries[i];
                    for(int j=1; j <= k; j++) {
                        if(maxDimValue < allCurrentChilds[j]->maxBoundaries[i]){
                            maxDimValue = allCurrentChilds[j]->maxBoundaries[i];
                        }
                        if(minDimValue > allCurrentChilds[j]->minBoundaries[i]){
                            minDimValue = allCurrentChilds[j]->minBoundaries[i];
                        }
                    }
                    sumMargins[d] += maxDimValue - minDimValue;
                    minDimValue = allCurrentChilds[k+1]->minBoundaries[i];
                    maxDimValue = allCurrentChilds[k+1]->maxBoundaries[i];
                    for(int j=k+2; j < R_TREE_NUMBER_CHILDS + 1; j++) {
                        if(maxDimValue < allCurrentChilds[j]->maxBoundaries[i]){
                            maxDimValue = allCurrentChilds[j]->maxBoundaries[i];
                        }
                        if(minDimValue > allCurrentChilds[j]->minBoundaries[i]){
                            minDimValue = allCurrentChilds[j]->minBoundaries[i];
                        }
                    }
                    sumMargins[d] += maxDimValue - minDimValue;
                }
            }

            //Sort by the maximum boundary allong this axis
            //TODO For now a bubble sort... Maybe change this later on
            bubbleSortChangedMarker = true;
            for(int i = 0; i < R_TREE_NUMBER_CHILDS + 1 && changed; i++){
                bubbleSortChangedMarker = false;
                for(int j=0; j < R_TREE_NUMBER_CHILDS + 1 - i; j++) {
                    if(allCurrentChilds[i]->maxBoundaries[d] > allCurrentChilds[j]->maxBoundaries[d]) {
                        std::swap(allCurrentChilds[i], allCurrentChilds[j]);
                        bubbleSortChangedMarker = true;
                    }
                }
            }
            //Calculate the sum of all margins for all possible splits allong the axis d for the maximum Boundaries
            for(int k=R_TREE_MINIMUM_CHILDS - 1; k < R_TREE_NUMBER_CHILDS - R_TREE_MINIMUM_CHILDS; k++){
                for(int i=0; i <= this->dimensions; i++){ //TODO in case of i == d the finding of maxDim can be optimised...
                    minDimValue = allCurrentChilds[0]->minBoundaries[i];
                    maxDimValue = allCurrentChilds[0]->maxBoundaries[i];
                    for(int j=1; j <= k; j++) {
                        if(maxDimValue < allCurrentChilds[j]->maxBoundaries[i]){
                            maxDimValue = allCurrentChilds[j]->maxBoundaries[i];
                        }
                        if(minDimValue > allCurrentChilds[j]->minBoundaries[i]){
                            minDimValue = allCurrentChilds[j]->minBoundaries[i];
                        }
                    }
                    sumMargins[d] += maxDimValue - minDimValue;
                    minDimValue = allCurrentChilds[k+1]->minBoundaries[i];
                    maxDimValue = allCurrentChilds[k+1]->maxBoundaries[i];
                    for(int j=k+2; j < R_TREE_NUMBER_CHILDS + 1; j++) {
                        if(maxDimValue < allCurrentChilds[j]->maxBoundaries[i]){
                            maxDimValue = allCurrentChilds[j]->maxBoundaries[i];
                        }
                        if(minDimValue > allCurrentChilds[j]->minBoundaries[i]){
                            minDimValue = allCurrentChilds[j]->minBoundaries[i];
                        }
                    }
                    sumMargins[d] += maxDimValue - minDimValue;
                }
            }
        }
        //Find the axis with the minimum sumMargins
        for(int d=1; d < this->dimensions; d++){
            if(sumMargins[d] < sumMargins[splitDim]) {
                splitDim = d;
            }
        }
        //Split along the axis
        //Find split index by finding the distribution with minimum overlap
        //TODO implement further from here
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
    if(childCount < R_TREE_NUMBER_CHILDS) {
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
