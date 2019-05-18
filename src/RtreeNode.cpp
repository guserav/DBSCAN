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
    //TODO replace with std::copy
    for(int i=0; i < dimensions; i++){
        this->minBoundaries[i] = firstChild->minBoundaries[i];
        this->maxBoundaries[i] = firstChild->maxBoundaries[i];
    }
    this->childNodes[0] = firstChild;
    this->childCount = 1;
    firstChild->parent = this;
}

RtreeNode::RtreeNode(RtreeNode *firstChild, RtreeNode *secondChild): RtreeNode(firstChild) {
    this->addChild(secondChild);
}

RtreeNode::RtreeNode(DataPointFloat *firstChild): dimensions(firstChild->getDimensions()) {
    this->init();
    //TODO replace with std::copy
    for(int i=0; i < dimensions; i++){
        this->minBoundaries[i] = (*firstChild)[i];
        this->maxBoundaries[i] = (*firstChild)[i];
    }
    this->childLeaves[0] = firstChild;
    this->childCount = 1;
    firstChild->setParent(this);
}

RtreeNode::~RtreeNode() {
    delete [] minBoundaries;
    delete [] maxBoundaries;
    for(int i=0; i < R_TREE_NUMBER_CHILDS; i++){
        if(childNodes[i]) childNodes[i]->parent = nullptr;
        delete childNodes[i];
        if(childLeaves[i]) childLeaves[i]->setParent(nullptr);
        // Don't clear child leaves as they are held by something else
        //delete childLeaves[i];
    }
    //TODO remove from parentNode
}

RtreeNode *RtreeNode::insertNewPoint(DataPointFloat *dataPoint) {
#ifdef _DEBUG
    this->checkIntegrity();
#endif
    if(hasLeaves()) {
        return this->addLeaveChild(dataPoint);
    }
    int chosenIndex;
    float areaEnlargement[childCount];
    float minEnlargement;
    float minArea;
    float minAreaBoundary[dimensions];
    float maxAreaBoundary[dimensions];
    RtreeNode * newNode;
    bool multipleBest = false;
    // Find child node to insert into
    // Choose of subtree depends of depth in the tree
    if(childNodes[0]->hasLeaves()) {
        float areaOverlap[childCount];
        float minOverlap;
        //Choose least overlap
        for(int i=0; i < childCount; i++) {
            for(int d=0; d < dimensions; d++) {
                if((*dataPoint)[d] < childNodes[i]->minBoundaries[d]) {
                    minAreaBoundary[d] = (*dataPoint)[d];
                    maxAreaBoundary[d] = childNodes[i]->maxBoundaries[d];
                } else if((*dataPoint)[d] > childNodes[i]->maxBoundaries[d]) {
                    maxAreaBoundary[d] = (*dataPoint)[d];
                    minAreaBoundary[d] = childNodes[i]->minBoundaries[d];
                } else {
                    minAreaBoundary[d] = childNodes[i]->minBoundaries[d];
                    maxAreaBoundary[d] = childNodes[i]->maxBoundaries[d];
                }
            }
            areaOverlap[i] = 0.0f;
            //Skip index i without having to check it every time
            for(int j=0; j < i; j++){
                areaOverlap[i] += calculateOverlap(minAreaBoundary, maxAreaBoundary, childNodes[j]->minBoundaries, childNodes[j]->maxBoundaries, dimensions);
            }
            for(int j=i + 1; j < childCount; j++) {
                areaOverlap[i] += calculateOverlap(minAreaBoundary, maxAreaBoundary, childNodes[j]->minBoundaries, childNodes[j]->maxBoundaries, dimensions);
            }
        }
        //Determine least overlap
        chosenIndex = 0;
        minOverlap = areaOverlap[0];
        for(int i=1; i < childCount; i++){
            if(minOverlap > areaOverlap[i]) {
                minOverlap = areaOverlap[i];
                chosenIndex = i;
                multipleBest = false;
            } else if(minOverlap == areaOverlap[i]) {
                multipleBest = true;
            }
        }
        // Determine minimum required Enlargement
        if(multipleBest) {
            multipleBest = false;
            minEnlargement = childNodes[chosenIndex]->calculateEnlargement(dataPoint);
            areaEnlargement[chosenIndex] = minEnlargement;
            for(int i=chosenIndex + 1; i<childCount; i++){
                areaEnlargement[i] = -1.0f; //Set Enlargement to impossible value for determining minimum area to ignore it if not calculated
                if(minOverlap == areaOverlap[i]) { //Not for nodes that don't have the minimum overlap cost
                    areaEnlargement[i] = childNodes[i]->calculateEnlargement(dataPoint);
                    if(areaEnlargement[i] < minEnlargement) {
                        multipleBest = false;
                        chosenIndex = i;
                        minEnlargement = areaEnlargement[i];
                    } else if(areaEnlargement[i] == minEnlargement) {
                        multipleBest = true;
                    }
                }
            }
            // Determine minimumArea
            if(multipleBest) {
                if(minEnlargement == 0.0f) {
                    minArea = childNodes[chosenIndex]->volume;
                    for(int i=chosenIndex+1; i<childCount; i++) {
                        if(minEnlargement == areaEnlargement[i]) { // Not for nodes that don't have the minimum enlargement from beforehand
                            if(minArea > childNodes[i]->volume) {
                                minArea = childNodes[i]->volume;
                                chosenIndex = i;
                            }
                        }
                    }
                } else {
                    NOT_YET_IMPLEMENTED("Add data point based on area"); //TODO calculate volume with point inserted
                }
            }
        }
    } else { // Node has no leave nodes as childs
        //Choose least area enlargement then least area
        multipleBest = false;
        chosenIndex = 0;
        minEnlargement = childNodes[0]->calculateEnlargement(dataPoint);
        areaEnlargement[0] = minEnlargement;
        for(int i = 1; i < childCount; i++) {
            areaEnlargement[i] = childNodes[i]->calculateEnlargement(dataPoint);
            if(areaEnlargement[i] < minEnlargement) {
                multipleBest = false;
                chosenIndex = i;
                minEnlargement = areaEnlargement[i];
            } else if(minEnlargement == areaEnlargement[i]) {
                multipleBest = true;
            }
        }
        if(multipleBest) { //Determine minimum area
            if(minEnlargement == 0.0f) {
                minArea = childNodes[chosenIndex]->volume;
                for(int i = chosenIndex + 1; i < childCount; i++) {
                    if(minEnlargement == areaEnlargement[i]) { // Only check minimum area if node belongs to minimum area enlargement
                        if(minArea < childNodes[i]->volume) {
                            chosenIndex = i;
                            minArea = childNodes[i]->volume;
                        }
                    }
                }
            } else {
                NOT_YET_IMPLEMENTED("Add data point based on area"); //TODO calculate volume with point inserted
            }
        }
    }
    // Insert node
    // On split of child add Child to this node
    // Split if necessary
    if((newNode = childNodes[chosenIndex]->insertNewPoint(dataPoint)) != nullptr) {
        return this->addChild(newNode);
    }
    this->expandForNewChild(dataPoint); // Do this here as addChild will expand the Boundaries by itself
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
 * @param newChild  the child node to add.
 * @return  A new node if a split occured
 */
RtreeNode* RtreeNode::addChild(RtreeNode * newChild) {
    newChild->parent = this; // Set this regardless of whether the child will be a part of this node as it is to annoying to make a case later
    if(childCount < R_TREE_NUMBER_CHILDS) {
        this->childNodes[this->childCount++] = newChild;
        for(int i=0; i < this->dimensions; i++){
            if(this->maxBoundaries[i] < newChild->maxBoundaries[i]) {
                this->maxBoundaries[i] = newChild->maxBoundaries[i];
            } else if(this->minBoundaries[i] > newChild->minBoundaries[i]) {
                this->minBoundaries[i] = newChild->minBoundaries[i];
            }
        }
        //TODO think about calculating volume in the code above on extension? This may result in better performance
        this->calculateVolume();
        return nullptr;
    } else {
        float sumMargins[this->dimensions];
        int splitDim = 0;

        //Make container for all Childs to better sort them
        RtreeNode * allCurrentChilds[R_TREE_NUMBER_CHILDS + 1];
        for(int i=0; i < R_TREE_NUMBER_CHILDS; i++) {
            allCurrentChilds[i] = this->childNodes[i];
        }
        allCurrentChilds[R_TREE_NUMBER_CHILDS] = newChild;

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
        for(int k=0; k < R_TREE_NUMBER_SORTS; k++){
            overlapValue[1][k] = calculateOverlap(allCurrentChilds, k + R_TREE_MINIMUM_CHILDS - 1, dimensions);
        }
        //Second for minBoundaries
        sortByMinBoundary(allCurrentChilds, splitDim);
        for(int k=0; k < R_TREE_NUMBER_SORTS; k++){
            overlapValue[0][k] = calculateOverlap(allCurrentChilds, k + R_TREE_MINIMUM_CHILDS - 1, dimensions);
        }

        //Find minimum overlap distribution
        float minOverlap = overlapValue[0][0];
        bool isInMins = true;
        int splitIndex = 0; // + R_TREE_NUMBER_CHILDS - 1 to get split index
        bool multipleFits = false;
        for(int i = 0; i < 2; i++) {
            for(int j = 0; j < R_TREE_NUMBER_SORTS; j++){
                if(minOverlap > overlapValue[i][j]) {
                    minOverlap = overlapValue[i][j];
                    isInMins = i == 0;
                    splitIndex = j;
                    multipleFits = false;
                } else if (minOverlap == overlapValue[i][j]) {
                    multipleFits = true;
                }
            }
        }

        if(multipleFits) { //If there are nodes with the same minimum overlap choose among then with minimum volume
            float minVolume;
            int startIndex;
            float tempVolume;
            //Skip calculating mins if the first minOverlap was in the maximums
            if(isInMins) {
                //Start calculating minimum Volume from first match
                minVolume = calculateVolume(allCurrentChilds, splitIndex  + R_TREE_MINIMUM_CHILDS - 1, this->dimensions);
                //Determine split index by minimum area for the rectangles in question
                //allCurrentChilds is still sorted for minBoundaries therefor start for them
                for(int i = splitIndex + 1; i < R_TREE_NUMBER_SORTS; i++){
                    if(minOverlap == overlapValue[0][i]){
                        tempVolume = calculateVolume(allCurrentChilds, i + R_TREE_MINIMUM_CHILDS - 1, this->dimensions);
                        if(tempVolume < minVolume) {
                            splitIndex = i;
                            minVolume = tempVolume;
                        }
                    }
                }
                startIndex = 0;
            } else { //Ignore all minimum splits
                startIndex = splitIndex + 1;
                minVolume = calculateVolume(allCurrentChilds, splitIndex  + R_TREE_MINIMUM_CHILDS - 1, this->dimensions);
            }
            sortByMaxBoundary(allCurrentChilds, splitDim);
            for(int i = startIndex; i < R_TREE_NUMBER_SORTS; i++){
                if(minOverlap == overlapValue[0][i]){
                    tempVolume = calculateVolume(allCurrentChilds, i + R_TREE_MINIMUM_CHILDS - 1, this->dimensions);
                    if(tempVolume < minVolume) {
                        splitIndex = i;
                        minVolume = tempVolume;
                        isInMins = false;
                    }
                }
            }
            if(isInMins) {
                sortByMinBoundary(allCurrentChilds, splitDim);
            }
        } else { // Ensure that the data is sorted in the correct orientation
            if(!isInMins) {
                sortByMaxBoundary(allCurrentChilds, splitDim);
            }
        }
        //Assuming that the data is sorted in respect to the best split condition
        //Do split after index splitIndex + R_TREE_MINIMUM_CHILDS - 1

        // Put all nodes higher then the split in new node
        RtreeNode * newNode = new RtreeNode(allCurrentChilds[splitIndex + R_TREE_MINIMUM_CHILDS]);
        allCurrentChilds[splitIndex + R_TREE_MINIMUM_CHILDS] = nullptr;
        for(int i=splitIndex + R_TREE_MINIMUM_CHILDS + 1; i < R_TREE_NUMBER_CHILDS + 1; i++) {
#ifdef _DEBUG
            if(newNode->addChild(allCurrentChilds[i]) != nullptr) throw std::runtime_error("Doesn't expect new Node to split on adding childs");
#else
            newNode->addChild(allCurrentChilds[i]);
#endif
            allCurrentChilds[i] = nullptr; // Ensure that non used child Node array slots are set to null
        }
        this->childCount = splitIndex + R_TREE_MINIMUM_CHILDS;
        std::copy(allCurrentChilds, allCurrentChilds + R_TREE_NUMBER_CHILDS, childNodes);
        std::copy(childNodes[0]->minBoundaries, childNodes[0]->minBoundaries + dimensions, minBoundaries);
        std::copy(childNodes[0]->maxBoundaries, childNodes[0]->maxBoundaries + dimensions, maxBoundaries);
        for(int i=1; i < childCount; i++) {
            for(int d=0; d < dimensions; d++) {
                if(childNodes[i]->minBoundaries[d] < minBoundaries[d]) {
                    minBoundaries[d] = childNodes[i]->minBoundaries[d];
                }
                if(childNodes[i]->maxBoundaries[d] > maxBoundaries[d]) {
                    maxBoundaries[d] = childNodes[i]->maxBoundaries[d];
                }
            }
        }
        calculateVolume();
        return newNode;
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
    for(int i = R_TREE_NUMBER_CHILDS; i > 0 && bubbleSortChangedMarker; i--){
        bubbleSortChangedMarker = false;
        for(int j=0; j < i; j++) {
            if(allCurrentChilds[j]->minBoundaries[d] > allCurrentChilds[j + 1]->minBoundaries[d]) {
                std::swap(allCurrentChilds[j], allCurrentChilds[j + 1]);
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
    for(int i = R_TREE_NUMBER_CHILDS; i > 0 && bubbleSortChangedMarker; i--){
        bubbleSortChangedMarker = false;
        for(int j=0; j < i; j++) {
            if(allCurrentChilds[j]->maxBoundaries[d] > allCurrentChilds[j + 1]->maxBoundaries[d]) {
                std::swap(allCurrentChilds[j], allCurrentChilds[j + 1]);
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
 */
void RtreeNode::calculateMargin(RtreeNode *allCurrentChilds[R_TREE_NUMBER_CHILDS + 1], int k, unsigned int dimensions, float& margin) {
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
                    volume *= e2 - s1;
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
 * Returns the area of the overlap described by the handed boundaries
 * @param s1 the minimum boundaries of the first rectangle
 * @param e1 the maximum boundaries of the first rectangle
 * @param s2 the minimum boundaries of the second rectangle
 * @param e2 the maximum boundaries of the second rectangle
 * @param dimensions the dimensions the handed areas have
 * @return
 */
float RtreeNode::calculateOverlap(const float *s1, const float *e1, const float *s2, const float *e2, unsigned int dimensions) {
    float volume = 1.0f;
    for(int d=0; d < dimensions; d++) {
#ifdef _DEBUG
        if(s1[d] > e1[d] || s2[d] > e2[d]) {
            throw std::invalid_argument("Wrong use of minimum and maximum boundaries in calculating the overlap: Please check order of parameter");
        }
#endif
        if(s1[d] < s2[d]) {
            if(s2[d] < e1[d]) {
                if(e1[d] < e2[d]) {
                    // s1 --- s2 ooo e1 --- e2
                    volume *= e1[d] - s2[d];
                } else { // e1 > e2
                    // s1 --- s2 ooo e2 --- e1
                    volume *= e2[d] - s2[d];
                }
            } else { // e1 < s2
                // s1 --- e1 no overlap s2 --- e2
                return 0.0f;
            }
        } else { // s2 < s1
            if(s1[d] < e2[d]) {
                if (e2[d] < e1[d]) {
                    // s2 --- s1 ooo e2 --- e1
                    volume *= e2[d] - s1[d];
                } else { //e1 < e2
                    // s2 --- s1 ooo e1 --- e2
                    volume *= e1[d] - s1[d];
                }
            } else { //e2 < s1
                // s2 --- e2 no overlap s1 --- e1
                return 0.0f;
            }
        }
#ifdef _DEBUG
        if(volume < 0.0f) {
            throw std::runtime_error("volume of overlap cannot be less then 0");
        }
#endif
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
 * Calculated the Area Enlargement needed to fit in the datapoint into this node
 * @param pFloat pointer to the datapoint to add
 * @return the area enlargement needed
 */
float RtreeNode::calculateEnlargement(DataPointFloat *dataPoint) {
    float enlargement = 0.0f;
    for(int i=0; i < dimensions; i++){
        if((*dataPoint)[i] < minBoundaries[i]) {
            enlargement = (volume + enlargement) * (minBoundaries[i] - (*dataPoint)[i]) / (maxBoundaries[i] - minBoundaries[i]);
        } else if((*dataPoint)[i] > maxBoundaries[i]) {
            enlargement = (volume + enlargement) * ((*dataPoint)[i] - maxBoundaries[i]) / (maxBoundaries[i] - minBoundaries[i]);
        }
    }
    return enlargement;
}

/**
 * Adds a Leave child to this node.
 *
 * This functions assumes that there are allready other children and the min/max boundaries are proberly set
 * @param child  the child data Point to add.
 * @return  A new node if a split occured
 */
RtreeNode* RtreeNode::addLeaveChild(DataPointFloat *child) {
#ifdef _DEBUG
    if(this->childCount > 0 && !this->hasLeaves()) {
        throw std::runtime_error("This function should only be called on Leave nodes");
    }
#endif
    child->setParent(this); // Needed anyway and doing it later requires a lot of logic and pain
    if(childCount < R_TREE_NUMBER_CHILDS) {
        this->childLeaves[this->childCount++] = child;
        this->expandForNewChild(child);
        return nullptr;
    } else {
        DataPointFloat * allCurrentChilds[R_TREE_NUMBER_CHILDS + 1];
        float margins[this->dimensions];
        float minBoundary, maxBoundary;
        int splitDim = 0;
        float minArea = std::numeric_limits<float>::max();
        int splitIndex = 0; //should always be overwritten
        float area1, area2;
        RtreeNode * newNode;

        for(int i=0; i < R_TREE_NUMBER_CHILDS; i++) {
            allCurrentChilds[i] = this->childLeaves[i];
        }
        allCurrentChilds[R_TREE_NUMBER_CHILDS] = child;

        for(int d=0; d < this->dimensions; d++){
            sortAllChildsLeaves(allCurrentChilds, d);
            margins[d] = 0;
            for(int k=R_TREE_MINIMUM_CHILDS - 1; k < R_TREE_NUMBER_CHILDS - R_TREE_MINIMUM_CHILDS; k++){
                for(int i = 0; i < this->dimensions; i++) {
                    minBoundary = (*allCurrentChilds[0])[i];
                    maxBoundary = (*allCurrentChilds[0])[i];
                    for(int j = 0; j <= k; j++){
                        if(minBoundary > (*allCurrentChilds[j])[i]){
                            minBoundary = (*allCurrentChilds[j])[i];
                        } else if(maxBoundary < (*allCurrentChilds[j])[i]){
                            maxBoundary = (*allCurrentChilds[j])[i];
                        }
                    }
                    margins[d] += maxBoundary - minBoundary;
                    minBoundary = (*allCurrentChilds[k + 1])[i];
                    maxBoundary = (*allCurrentChilds[k + 1])[i];
                    for(int j = k + 2; j < R_TREE_NUMBER_CHILDS + 1; j++){
                        if(minBoundary > (*allCurrentChilds[j])[i]){
                            minBoundary = (*allCurrentChilds[j])[i];
                        } else if(maxBoundary < (*allCurrentChilds[j])[i]){
                            maxBoundary = (*allCurrentChilds[j])[i];
                        }
                    }
                    margins[d] += maxBoundary - minBoundary;
                }
            }
        }

        // Split dimensions is the dimension with the smallest margins
        for(int i=1; i < this->dimensions; i++){
            if(margins[i] < margins[splitDim]){
                splitDim = i;
            }
        }

        //Skip calculating overlap as it is not possible when splitting the points on an axis (sorted by this axis)
        sortAllChildsLeaves(allCurrentChilds, splitDim);
        for(int k=R_TREE_MINIMUM_CHILDS - 1; k < R_TREE_NUMBER_CHILDS - R_TREE_MINIMUM_CHILDS; k++){
            area1 = 1.0f;
            area2 = 1.0f;

            for(int i = 0; i < this->dimensions; i++) {
                minBoundary = (*allCurrentChilds[0])[i];
                maxBoundary = (*allCurrentChilds[0])[i];
                for(int j = 0; j <= k; j++){
                    if(minBoundary > (*allCurrentChilds[j])[i]){
                        minBoundary = (*allCurrentChilds[j])[i];
                    } else if(maxBoundary < (*allCurrentChilds[j])[i]){
                        maxBoundary = (*allCurrentChilds[j])[i];
                    }
                }
                area1 *= maxBoundary - minBoundary;
                minBoundary = (*allCurrentChilds[k + 1])[i];
                maxBoundary = (*allCurrentChilds[k + 1])[i];
                for(int j = k + 2; j < R_TREE_NUMBER_CHILDS + 1; j++){
                    if(minBoundary > (*allCurrentChilds[j])[i]){
                        minBoundary = (*allCurrentChilds[j])[i];
                    } else if(maxBoundary < (*allCurrentChilds[j])[i]){
                        maxBoundary = (*allCurrentChilds[j])[i];
                    }
                }
                area2 *= maxBoundary - minBoundary;
            }

            if(minArea > area1 + area2) {
                minArea = area1 + area2;
                splitIndex = k;
            }
        }

        //TODO make drop of point less complex
        newNode = new RtreeNode(allCurrentChilds[splitIndex + 1]);
        allCurrentChilds[splitIndex + 1] = nullptr;
        for(int i = splitIndex + 2; i < R_TREE_NUMBER_CHILDS + 1; i++) {
#ifndef _DEBUG
            newNode->addLeaveChild(allCurrentChilds[i]);
#else
            if(newNode->addLeaveChild(allCurrentChilds[i])){
                throw std::runtime_error("Splitout node should never split on adding the children");
            }
#endif
            allCurrentChilds[i] = nullptr;
        }
        this->childCount = splitIndex + 1;
        std::copy(allCurrentChilds, allCurrentChilds + R_TREE_NUMBER_CHILDS, this->childLeaves);
        recalculateBoundaries();
        return newNode;
    }
}

/**
 * Recalculate the boundaries based of the childs currently present.
 * Hereby the values currently in min and maxBoundaries are ignored as there are assumed to be not valid
 */
void RtreeNode::recalculateBoundaries() {
    float * toCopy = childLeaves[0]->getData();
    std::copy(toCopy, toCopy + dimensions, minBoundaries);
    std::copy(minBoundaries, minBoundaries + dimensions, maxBoundaries);
    for (int i = 1; i < childCount; i++) {
        for (int d = 0; d < dimensions; d++) {
            float value = (*(childLeaves[i]))[d];
            if (value < minBoundaries[d]) {
                minBoundaries[d] = value;
            } else if (value > maxBoundaries[d]) {
                maxBoundaries[d] = value;
            }
        }
#ifdef _DEBUG
        checkIntegrity();
#endif
    }
    calculateVolume();
}

void RtreeNode::sortAllChildsLeaves(DataPointFloat *allCurrentChilds[R_TREE_NUMBER_CHILDS + 1], int d) {
    bool bubbleSortChangedMarker = true;
    for(int i = R_TREE_NUMBER_CHILDS; i > 0 && bubbleSortChangedMarker; i--){
        bubbleSortChangedMarker = false;
        for(int j=0; j < i; j++) {
            if((*allCurrentChilds[j])[d] > (*allCurrentChilds[j + 1])[d]) {
                std::swap(allCurrentChilds[j], allCurrentChilds[j + 1]);
                bubbleSortChangedMarker = true;
            }
        }
    }
}

/**
 * Calculate the n-dimensional volume of this Node.
 * In the case that there are not at least two childs the function will fail as it is not expected that this will fail.
 * It should never happen that this node has one node as child and needs its volume
 * This function is currently not threadsafe (TODO)
 */
void RtreeNode::calculateVolume() {
#ifdef _DEBUG
    this->checkIntegrity();
    if(this->childCount < 2) {
        throw std::runtime_error("Can't calculate volume for only one child.");
    }
#endif
    this->volume = 1.0f;
    for(int i=0; i < this->dimensions; i++) {
        this->volume *= this->maxBoundaries[i] - this->minBoundaries[i];
    }
}

/**
 * Removes the data point from this leave node. If the node to remove doesn't exist nothing happens
 * This functions assumes that the point is only once in this node
 * @param point A pointer to the leave node to remove
 * @throws std::runtime_error if this is not a leave node
 */
void RtreeNode::removePoint(DataPointFloat *point) {
    dropPoint(point);
    calculateVolume();
}

/**
 * Removes the data point from this leave node. If the node to remove doesn't exist nothing happens
 * This functions assumes that the point is only once in this node
 * The difference to removePoint is that calculate Point is not called this is to allow for masive drops e.g. on splits
 * @param point A pointer to the leave node to remove
 * @throws std::runtime_error if this is not a leave node
 */
void RtreeNode::dropPoint(DataPointFloat *point) {
    for(int i=0; i < childCount; i++){
        if(childLeaves[i] == point){
            for(int j=i; j < childCount - 1; j++) {
                childLeaves[j] = childLeaves[j + 1];
            }
            childLeaves[childCount--] = nullptr;
#ifdef _DEBUG
            if(this->childCount < R_TREE_MINIMUM_CHILDS) {
                NOT_YET_IMPLEMENTED("Node contains less than minimum child after removing some, should be merged...");
            }
#endif
            calculateVolume();
            return;
        }
    }
}

/**
 * Replaces the old Point pointer with the new one to don't loose old objects
 * This function should only be called by the copy constructors of the DataPoint
 * @param oldPoint the old Pointer to replace
 * @param newPoint the new Pointer to emplace
 * @throws std::invalid_argument when the old Pointer is not contained in here
 * TODO: Look if replacing this to using real objects that are copied impacts performance as no pointers are used and prefetching could do its job
 */
void RtreeNode::replaceNode(DataPointFloat *oldPoint, DataPointFloat *newPoint) {
#ifdef _DEBUG
    if(!hasLeaves()) {
        throw std::runtime_error("Replace node cannot be called on a non Leave node");
    }
#endif
    if(childLeaves[0] == oldPoint) {
        childLeaves[0] = newPoint;
        if(childCount > 1) { // Don't calculate volume when only having one child
            calculateVolume();
        }
        return;
    }
    for(int i=1; i < childCount; i++){
        if(childLeaves[i] == oldPoint){
            childLeaves[i] = newPoint;
            calculateVolume();
            return;
        }
    }
    throw std::invalid_argument("Couldn't find old Point within this node");
}

#ifdef _DEBUG
/**
 * Debug function to check the integrity of this object
 */
void RtreeNode::checkIntegrity() {
    for(int i = 0; i < this->dimensions; i++) {
        if(this->minBoundaries[i] > this->maxBoundaries[i]) {
            throw std::runtime_error("Boundaries of this object don't match (negative expansion)");
        }
    }
    if(this->volume < 0) {
        throw std::runtime_error("Negative volume seems odd.");
    }
    for(int i = 0; i < R_TREE_NUMBER_CHILDS; i++) {
        bool foundChild;
        if(this->hasLeaves()) {
            foundChild = this->childLeaves[i] != nullptr;
        } else {
            foundChild = this->childNodes[i] != nullptr;
        }
        if(foundChild && i >= this->childCount) {
            throw std::runtime_error("Not Expecting child here");
        }
        if(!foundChild && i < this->childCount) {
            throw std::runtime_error("Expecting child here");
        }
    }
}
#endif

/**
 * Expand the boundaries of this node to compensate for the new child.
 * @param child
 */
void RtreeNode::expandForNewChild(DataPointFloat *child) {
    for(int i=0; i < this->dimensions; i++){
        if(this->maxBoundaries[i] < (*child)[i]) {
            this->maxBoundaries[i] = (*child)[i];
        } else if(this->minBoundaries[i] > (*child)[i]) {
            this->minBoundaries[i] = (*child)[i];
        }
#ifdef _DEBUG
        if(this->minBoundaries[i] > this->maxBoundaries[i]) {
            throw std::runtime_error("The minimum Boundary should never be bigger then the maximum why?");
        }
#endif
        //TODO think about calculating volume within the loop above (performance, but maybe not thread safe)
        //TODO also consider not calculating volume here as this function maybe used in fast paste for adding multiple children at the same time
        this->calculateVolume();
    }
}

void RtreeNode::printToConsole(int level) {
    for(int i = 0; i < level; i++) {
        std::cout << "    ";
    }
    std::cout << "[" << std::to_string(reinterpret_cast<long long>(this)) << "] parent: ["
            << std::to_string(reinterpret_cast<long long>(this->parent)) << "] childs: [";
    for(int i = 0; i < R_TREE_NUMBER_CHILDS; i++) {
        if(i < this->childCount) {
            std::cout << "\033[1;30m";
        } else {
            std::cout << "\033[0m";
        }
        if(this->hasLeaves()) {
            std::cout << std::to_string(reinterpret_cast<long long>(this->childLeaves[i])) << ", ";
        } else {
            std::cout << std::to_string(reinterpret_cast<long long>(this->childNodes[i])) << ", ";
        }
    }
    std::cout << "\033[0m]\n";
    for(int i = 0; i < childCount; i++) {
        if (this->hasLeaves()) {
            this->childLeaves[i]->printToConsole(level + 1);
        } else {
            this->childNodes[i]->printToConsole(level + 1);
        }
    }
}

void RtreeNode::printForVisualisation(int level) {
    std::cout << std::to_string(level) << ":[" << std::to_string(minBoundaries[0]);
    for(int i = 1; i < this->dimensions; i++){
        std::cout << ";" << std::to_string(minBoundaries[i]);
    }
    std::cout << "],[" << std::to_string(maxBoundaries[0]);
    for(int i = 1; i < this->dimensions; i++){
        std::cout << ";" << std::to_string(maxBoundaries[i]);
    }
    std::cout << "]\n";
    for(int i = 0; i < childCount; i++) {
        if (this->hasLeaves()) {
            this->childLeaves[i]->printForVisualisation(level + 1);
        } else {
            this->childNodes[i]->printForVisualisation(level + 1);
        }
    }
}
