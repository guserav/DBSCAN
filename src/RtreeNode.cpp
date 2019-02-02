#include <tkDecls.h>
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
        int splitDim = 0;

        //Make container for all Childs to better sort them
        RtreeNode * allCurrentChilds[R_TREE_NUMBER_CHILDS + 1];
        for(int i=0; i < R_TREE_NUMBER_CHILDS; i++) {
            allCurrentChilds[i] = this->childNodes[i];
        }
        allCurrentChilds[R_TREE_NUMBER_CHILDS] = child;

        for(int d=0; d < this->dimensions; d++){
            //Calculate the sum of all margins for all possible splits allong the axis d for the minimum Boundaries
            //Sort by the minimum boundary allong this axis
            sumMargins[d] = 0;

            sortByMinBoundary(allCurrentChilds, d);
            for(int k=R_TREE_MINIMUM_CHILDS - 1; k < R_TREE_NUMBER_CHILDS - R_TREE_MINIMUM_CHILDS; k++){
                calculateMargin(allCurrentChilds, k, this->dimensions, sumMargins[d]);
            }

            sortByMaxBoundary(allCurrentChilds, d);
            for(int k=R_TREE_MINIMUM_CHILDS - 1; k < R_TREE_NUMBER_CHILDS - R_TREE_MINIMUM_CHILDS; k++){
                calculateMargin(allCurrentChilds, k, this->dimensions, sumMargins[d]);
            }
        }

        //Find the axis with the minimum sumMargins
        for(int d=1; d < this->dimensions; d++){
            if(sumMargins[d] < sumMargins[splitDim]) {
                splitDim = d;
            }
        }

        //Find split index by finding the distribution with minimum overlap
        float overlapValue[2][R_TREE_NUMBER_SORTS];

        //First for maxBoundaries because it could be sorted from the last dimension if splitDim == dimensions - 1
        sortByMaxBoundary(allCurrentChilds, splitDim);
        for(int k=R_TREE_MINIMUM_CHILDS - 1; k < R_TREE_NUMBER_CHILDS - R_TREE_MINIMUM_CHILDS; k++){
            overlapValue[1][k - R_TREE_NUMBER_CHILDS + 1] = calculateOverlap(allCurrentChilds, k, dimensions);
        }
        //Second for minBoundaries
        sortByMinBoundary(allCurrentChilds, splitDim);
        for(int k=R_TREE_MINIMUM_CHILDS - 1; k < R_TREE_NUMBER_CHILDS - R_TREE_MINIMUM_CHILDS; k++){
            overlapValue[0][k - R_TREE_NUMBER_CHILDS + 1] = calculateOverlap(allCurrentChilds, k, dimensions);
        }

        //Find minimum overlap distribution
        float minOverlap = overlapValue[0][0];
        bool isInMins = true;
        int splitIndex = 0; // + R_TREE_NUMBER_CHILDS - 1 to get split index
        int amountOfFits = 1;
        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < R_TREE_NUMBER_SORTS; j++){
                if(minOverlap > overlapValue[i][j]) {
                    minOverlap = overlapValue[i][j];
                    isInMins = i == 0;
                    splitIndex = j;
                    amountOfFits = 1;
                } else if (minOverlap == overlapValue[i][j]) {
                    amountOfFits++;
                }
            }
        }
        if(amountOfFits > 1) {
            float minVolume = -1.0f; //Maker for later on
            //Skip calculating mins if the first minOverlap was in the maximums
            if(isInMins) {
                //Start calculating minimum Volume from first match
                minVolume = calculateVolume(allCurrentChilds, splitIndex  + R_TREE_MINIMUM_CHILDS - 1, this->dimensions);
                //Determine split index by minimum area for the rectangles in question
                //allCurrentChilds is still sorted for minBoundaries therefor start for them
                for(int i = splitIndex + 1; i < R_TREE_NUMBER_SORTS; i++){
                    if(minOverlap == overlapValue[0][i]){
                        NOT_YET_IMPLEMENTED("Split Node without leaves");
                        //TODO set minimum index
                        calculateVolume(allCurrentChilds, i + R_TREE_MINIMUM_CHILDS - 1, this->dimensions);
                    }
                }
            }
            sortByMaxBoundary(allCurrentChilds, splitDim);
            if(minVolume == -1.0f){
                minVolume = calculateVolume(allCurrentChilds, splitIndex  + R_TREE_MINIMUM_CHILDS - 1, this->dimensions);
            }
            NOT_YET_IMPLEMENTED("Split Node without leaves");
            //TODO do the same as above for the maximum
        }

        //If childs are no longer sorted for the respective min/max Boundary do that

        //TODO Split at index k
        //TODO Create new Node with all children starting from k + 1 and remove the added children from this node
        //TODO implement further from here
        NOT_YET_IMPLEMENTED("Split Node without leaves");
    }
}

/**
 * Sorts the respective child array by there minimum Boundary
 * @param allCurrentChilds
 * @param d the dimension to sort by
 */
void RtreeNode::sortByMinBoundary(RtreeNode *allCurrentChilds[R_TREE_NUMBER_CHILDS + 1], int d){
    //TODO For now a bubble sort... Maybe change this later on
    bool bubbleSortChangedMarker = true;
    for(int i = 0; i < R_TREE_NUMBER_CHILDS + 1 && bubbleSortChangedMarker; i++){
        bubbleSortChangedMarker = false;
        for(int j=0; j < R_TREE_NUMBER_CHILDS + 1 - i; j++) {
            if(allCurrentChilds[i]->minBoundaries[d] > allCurrentChilds[i + 1]->minBoundaries[d]) {
                std::swap(allCurrentChilds[i], allCurrentChilds[i + 1]);
                bubbleSortChangedMarker = true;
            }
        }
    }
}

/**
 * Sorts the respective child array by there maximum Boundary
 * @param allCurrentChilds
 * @param d the dimension to sort by
 */
void RtreeNode::sortByMaxBoundary(RtreeNode *allCurrentChilds[R_TREE_NUMBER_CHILDS + 1], int d){
    //TODO For now a bubble sort... Maybe change this later on
    bool bubbleSortChangedMarker = true;
    for(int i = 0; i < R_TREE_NUMBER_CHILDS + 1 && bubbleSortChangedMarker; i++){
        bubbleSortChangedMarker = false;
        for(int j=0; j < R_TREE_NUMBER_CHILDS + 1 - i; j++) {
            if(allCurrentChilds[i]->maxBoundaries[d] > allCurrentChilds[i + 1]->maxBoundaries[d]) {
                std::swap(allCurrentChilds[i], allCurrentChilds[i + 1]);
                bubbleSortChangedMarker = true;
            }
        }
    }
}

/**
 * Adds the margin of the respective split to the variable referenced by margin
 * @param allCurrentChilds
 * @param k
 * @param dimensions
 * @param margin
 * @return
 */
float RtreeNode::calculateMargin(RtreeNode *allCurrentChilds[R_TREE_NUMBER_CHILDS + 1], int k, unsigned int dimensions, float& margin) {
    float minDimValue, maxDimValue;
    for(int i=0; i <= dimensions; i++){ //TODO in case of i == d the finding of whatever it is sorted by
        //TODO put the finding of the margins into a function...
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
        margin += maxDimValue - minDimValue;
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
        margin += maxDimValue - minDimValue;
    }
}

/**
 * Calculates the overlap of the two areas described by the bounding boxes of allChilds[0, k] and allChilds[k+1, R_TREE_NUMBER_CHILDS]
 * @param allChilds an array containing a pointer to all childs to consider
 * @param splitIndex the index k to split the childs at
 * @return the area of the overlapping region if it exists else 0
 */
float RtreeNode::calculateOverlap(RtreeNode *allChilds[R_TREE_NUMBER_CHILDS + 1], int k, unsigned int dimensions) {
    float volume = 1.0f;
    //boundaries of the current dimensions (1: [0, k], 2: [k+1, R_TREE_NUMBER_CHILDS])
    float s1, s2, e1, e2;
    for(int i=0; i <= dimensions; i++){ //TODO in case of i == d the finding of minDim can be optimised...
        //First determine the boundaries of the two rectangles
        s1 = allChilds[0]->minBoundaries[i];
        e1 = allChilds[0]->maxBoundaries[i];
        for(int j=1; j <= k; j++) {
            if(e1 < allChilds[j]->maxBoundaries[i]){
                e1 = allChilds[j]->maxBoundaries[i];
            }
            if(s1 > allChilds[j]->minBoundaries[i]){
                s1 = allChilds[j]->minBoundaries[i];
            }
        }
        s2 = allChilds[k+1]->minBoundaries[i];
        e2 = allChilds[k+1]->maxBoundaries[i];
        for(int j=k+2; j < R_TREE_NUMBER_CHILDS + 1; j++) {
            if(e2 < allChilds[j]->maxBoundaries[i]){
                e2 = allChilds[j]->maxBoundaries[i];
            }
            if(s2 > allChilds[j]->minBoundaries[i]){
                s2 = allChilds[j]->minBoundaries[i];
            }
        }

        //Find what kind of intersect it is and calculate amount it is overlapping
        //If there is no overlap at all the function can return with 0 as it is no longer needed to calculate it for the other dimensions
        if(s1 < s2) {
            if(s2 < e1) {
                if(e1 < e2) {
                    // s1 --- s2 ooo e1 --- e2
                    volume *= e1 - s2;
                } else { // e1 > e2
                    // s1 --- s2 ooo e2 --- e1
                    volume *= e2 - s2;
                }
            } else { // e1 < s2
                // s1 --- e1 no overlap s2 --- e2
                return 0.0f;
            }
        } else { // s2 < s1
            if(s1 < e2) {
                if (e2 < e1) {
                    // s2 --- s1 ooo e2 --- e1
                    volume *= e2 - e1;
                } else { //e1 < e2
                    // s2 --- s1 ooo e1 --- e2
                    volume *= e1 - s1;
                }
            } else { //e2 < s1
                // s2 --- e2 no overlap s1 --- e1
                return 0.0f;
            }
        }
    }
#ifdef _DEBUG
    if(volume < 0.0f) {
        throw std::runtime_error("volume of overlap cannot be less then 0");
    }
#endif
    return volume;
}

/**
 * Calculates the volume of the two areas described by the bounding boxes of allChilds[0, k] and allChilds[k+1, R_TREE_NUMBER_CHILDS]
 * @param allChilds an array containing a pointer to all childs to consider
 * @param splitIndex the index k to split the childs at
 * @return the volume of the cubes described
 */
float RtreeNode::calculateVolume(RtreeNode *allChilds[R_TREE_NUMBER_CHILDS + 1], int k, unsigned int dimensions) {
    float volume1 = 1.0f;
    float volume2 = 1.0f;
    //boundaries of the current dimensions (1: [0, k], 2: [k+1, R_TREE_NUMBER_CHILDS])
    float s1, s2, e1, e2;
    for(int i=0; i <= dimensions; i++) { //TODO in case of i == d the finding of minDim can be optimised...
        //First determine the boundaries of the two rectangles
        s1 = allChilds[0]->minBoundaries[i];
        e1 = allChilds[0]->maxBoundaries[i];
        for (int j = 1; j <= k; j++) {
            if (e1 < allChilds[j]->maxBoundaries[i]) {
                e1 = allChilds[j]->maxBoundaries[i];
            }
            if (s1 > allChilds[j]->minBoundaries[i]) {
                s1 = allChilds[j]->minBoundaries[i];
            }
        }
        s2 = allChilds[k + 1]->minBoundaries[i];
        e2 = allChilds[k + 1]->maxBoundaries[i];
        for (int j = k + 2; j < R_TREE_NUMBER_CHILDS + 1; j++) {
            if (e2 < allChilds[j]->maxBoundaries[i]) {
                e2 = allChilds[j]->maxBoundaries[i];
            }
            if (s2 > allChilds[j]->minBoundaries[i]) {
                s2 = allChilds[j]->minBoundaries[i];
            }
        }
        volume1 *= e1 - s1;
        volume2 *= e2 - s2;
    }
#ifdef _DEBUG
    if(volume1 < 0.0f || volume2 < 0.0f) {
        throw std::runtime_error("volume of both rectangels cannot be less then 0");
    }
#endif
    return volume1 + volume2;
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
