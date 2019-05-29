#include "Rtree.h"

Rtree::~Rtree() {
    delete this->root;
}

Rtree::Rtree(unsigned int dimensions, DataPointFloat *firstDataPoint): dimensions(dimensions) {
    this->root = new RtreeNode(firstDataPoint);
}

Rtree::Rtree(Rtree &&obj) noexcept {
    this->root = obj.root;
    obj.root = nullptr;
}

Rtree &Rtree::operator=(Rtree &&obj) noexcept {
    if(this != &obj) {
        delete this->root;
        this->root = obj.root;
        obj.root = nullptr;
        return *this;
    }
    return *this;
}

void Rtree::addDataPoint(DataPointFloat *point) {
    RtreeNode * newNode = nullptr;
    if((newNode = this->root->insertNewPoint(point)) != nullptr){
        this->root = new RtreeNode(this->root, newNode);
    }
#ifdef _DEBUG
    this->checkIntegrity();
#endif
}

/**
 * Generates a list of all eps-neighbours of that point
 * @param epsilon the size of the area to look at
 * @return A list of eps-neighbours
 */
std::list<DataPointFloat *> Rtree::getNeighbours(DataPointFloat* point, float epsilon){
    std::list<DataPointFloat *> list;
    this->root->addNeighbours(list, point, epsilon);
    return list;
}

#ifdef _DEBUG
void Rtree::checkIntegrity(){
    this->root->checkIntegrity(true);
}
#endif

void Rtree::printToConsole() {
    this->root->printToConsole(0);
}

void Rtree::printForVisualisation() {
    this->root->printForVisualisation(0);
}

