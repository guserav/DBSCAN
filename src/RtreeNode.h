#ifndef STUDIENARBEITCODE_RTREENODE_H
#define STUDIENARBEITCODE_RTREENODE_H

class RtreeNode;

#include "DataPointFloat.h"
#include <exception>
#include <cmath>
#include <iostream>

// Use odd number to avoid edge case for insertion
#define R_TREE_NUMBER_CHILDS 10
#define R_TREE_MINIMUM_CHILDS 4
#define R_TREE_NUMBER_SORTS (R_TREE_NUMBER_CHILDS - 2 * R_TREE_MINIMUM_CHILDS + 2)
#define NOT_YET_IMPLEMENTED(desc) throw std::runtime_error("Not yet implemented: " desc);

class RtreeNode {
public:
    RtreeNode(const RtreeNode&) = delete;
    RtreeNode(RtreeNode&&) = delete;
    RtreeNode& operator=(const RtreeNode&) = delete;
    RtreeNode& operator=(RtreeNode&&) = delete;

    explicit RtreeNode(RtreeNode * firstChild);
    RtreeNode(RtreeNode * firstChild, RtreeNode * secondChild);
    explicit RtreeNode(DataPointFloat* firstChild);
    ~RtreeNode();

    RtreeNode * insertNewPoint(DataPointFloat* dataPoint);
    inline bool hasLeaves() __attribute__((always_inline));

    void addNeighbours(std::list<DataPointFloat *>& list, DataPointFloat *pFloat, float epsilon);

    void printToConsole(int level);
    void printForVisualisation(int level);


private:
    inline void init() __attribute__((always_inline));
    inline static float calculateOverlap(RtreeNode * allChilds[R_TREE_NUMBER_CHILDS + 1], int k, unsigned int dimension) __attribute__((always_inline));
    inline static float calculateOverlap(const float * s1, const float * e1, const float * s2, const float * e2, unsigned int dimension) __attribute__((always_inline));
    inline static float calculateVolume(RtreeNode * allChilds[R_TREE_NUMBER_CHILDS + 1], int k, unsigned int dimension) __attribute__((always_inline));
    inline static void calculateMargin(RtreeNode * allChilds[R_TREE_NUMBER_CHILDS + 1], int k, unsigned int dimension, float& margin) __attribute__((always_inline));
    inline static void sortByMinBoundary(RtreeNode * allChilds[R_TREE_NUMBER_CHILDS + 1], int d) __attribute__((always_inline));
    inline static void sortByMaxBoundary(RtreeNode * allChilds[R_TREE_NUMBER_CHILDS + 1], int d) __attribute__((always_inline));
    inline float calculateEnlargement(DataPointFloat *pFloat) __attribute__((always_inline));
    inline void sortAllChildsLeaves(DataPointFloat *allCurrentChilds[R_TREE_NUMBER_CHILDS + 1], int d) __attribute__((always_inline));
    RtreeNode * addChild(RtreeNode* newChild);
    RtreeNode* addLeaveChild(DataPointFloat* child);
    inline void expandForNewChild(DataPointFloat* child) __attribute__((always_inline));
    inline void calculateVolume() __attribute__((always_inline));
#ifdef _DEBUG
    void checkIntegrity();
#endif

    char childCount = 0;
    float volume = 0;
    unsigned int dimensions;
    float * minBoundaries;
    float * maxBoundaries;
    RtreeNode* childNodes[R_TREE_NUMBER_CHILDS];
    DataPointFloat* childLeaves[R_TREE_NUMBER_CHILDS];

    inline void recalculateBoundaries() __attribute__((always_inline));

    inline float distanceToBoundaries(DataPointFloat *pFloat) __attribute__((always_inline));
};


#endif //STUDIENARBEITCODE_RTREENODE_H
