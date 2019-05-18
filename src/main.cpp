#include "main.h"

int main() {
    //dbscan("../irisShuffeled.data", 4, ';');
    DBSCAN::dbscan("../complex9Schuffeled.data", 2, ',', 5, 5);
    return 0;
}