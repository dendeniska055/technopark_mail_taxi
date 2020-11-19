#include "WorldQuadTree.h"

const Coordinate northWest {90.0, -180.0};
const Coordinate northEast {90.0, 180.0};
const Coordinate southWest {-90.0, - 180.0};
const Coordinate southEast {-90.0, 180.0};
const uint optimalDeep = 14;

int main() {
    WorldQuadTree<int> QuadTree(northWest, northEast, southWest, southEast, optimalDeep);
    QuadTree.Insert(5, {55.9156087, 37.7632358});
    std::vector<int> test = QuadTree.Search({55.915114, 37.763052});
    return 0;
}
