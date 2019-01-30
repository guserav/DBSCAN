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
}

