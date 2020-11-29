#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <functional>

struct Coordinate{
    double _lat;
    double _long;
    friend bool operator== (const Coordinate &left, const Coordinate &right);
    //оператор для тестирования
    friend std::ostream& operator << (std::ostream& out, const Coordinate& coordinate);
};

bool operator==(const Coordinate &left, const Coordinate &right) {
    return left._lat == right._lat && left._long == right._long;
}

std::ostream &operator<<(std::ostream &out, const Coordinate &coordinate) {
    return out << coordinate._lat << ' ' << coordinate._long;
}

struct Borders{
    Coordinate a;
    Coordinate b;
    Coordinate c;
    Coordinate d;
    Borders(const Coordinate &a, const Coordinate &b, const Coordinate &c, const Coordinate &d);
    bool Belonging(const Coordinate& coordinate) const;
    friend bool operator== (const Borders &left, const Borders &right);
};

Borders::Borders(const Coordinate &a, const Coordinate &b,
                 const Coordinate &c, const Coordinate &d) : a(a), b(b), c(c), d(d)
{
}

bool Borders::Belonging(const Coordinate &coordinate) const {
    return coordinate._lat <= a._lat && coordinate._long >= a._long &&
           coordinate._lat <= b._lat && coordinate._long <= b._long &&
           coordinate._lat >= c._lat && coordinate._long >= c._long &&
           coordinate._lat >= d._lat && coordinate._long <= d._long;
}

bool operator==(const Borders &left, const Borders &right) {
    return left.a == right.a && left.b == right.b &&
           left.c == right.c && left.d == right.d;
}

enum class CardinalPoints{
    NorthWest,
    NorthEast,
    SouthWest,
    SouthEast
};

template<typename T>
struct Quadrant{
    Quadrant *northWest;
    Quadrant *northEast;
    Quadrant *southWest;
    Quadrant *southEast;
    Quadrant *parent;
    Borders cornerPoints;
    bool divided;
    std::vector<T> data;
    Quadrant(const Coordinate& a, const Coordinate& b, const Coordinate& c,
             const Coordinate& d, Quadrant* parent = nullptr);
    ~Quadrant();
};

template<typename T>
Quadrant<T>::Quadrant(const Coordinate &a, const Coordinate &b, const Coordinate &c, const Coordinate &d,
                       Quadrant *parent) : cornerPoints(a, b, c, d), parent(parent), divided(false)
{
    northWest = nullptr;
    northEast = nullptr;
    southWest = nullptr;
    southEast = nullptr;
}

template<typename T>
Quadrant<T>::~Quadrant() {
    delete northWest;
    delete northEast;
    delete southWest;
    delete southEast;
}

template<typename T>
class WorldQuadTree {
private:
    Quadrant<T> *root;
    uint deep;
private:
    Quadrant<T> *CreateCardinalPoint(Quadrant<T> *currentNode, CardinalPoints side);
    const std::vector<T>& GetQuadrantData(const Borders& quadrantBorders);
public:
    WorldQuadTree(const Coordinate& northWest, const Coordinate& northEast,
                  const Coordinate& southWest, const Coordinate& southEast, uint deep);
    ~WorldQuadTree();
    void Insert(const T& data, const Coordinate& coordinate);
    std::vector<T> Search (const Coordinate &coordinate) const;
    std::vector<T> SearchInDistrict(const Coordinate &coordinate);
};

template<typename T>
WorldQuadTree<T>::WorldQuadTree(const Coordinate &northWest, const Coordinate &northEast, const Coordinate &southWest,
                                const Coordinate &southEast, uint deep) : deep(deep)
{
    root = new Quadrant<T>(northWest, northEast, southWest, southEast);
}

template<typename T>
WorldQuadTree<T>::~WorldQuadTree() {
    delete root;
}

template<typename T>
Quadrant<T> *WorldQuadTree<T>::CreateCardinalPoint(Quadrant<T> *currentNode, CardinalPoints side) {
    const int8_t cutSize = 2;
    const Coordinate& a = currentNode->cornerPoints.a;
    const Coordinate& b = currentNode->cornerPoints.b;
    const Coordinate& c = currentNode->cornerPoints.c;
    const Coordinate& d = currentNode->cornerPoints.d;
    Coordinate midNorth{a._lat, (a._long + b._long) / cutSize};
    Coordinate midSouth{c._lat, (c._long + d._long) / cutSize};
    Coordinate midWest{(a._lat + c._lat) / cutSize, a._long};
    Coordinate midEast{( b._lat + d._lat) /cutSize, b._long};
    Coordinate center{(a._lat + c._lat) / cutSize, (a._long + b._long) / cutSize};
    switch (side){
        case CardinalPoints::NorthWest:
            return new Quadrant(a, midNorth, midWest, center, currentNode);
        case CardinalPoints::NorthEast:
            return new Quadrant(midNorth, b, center, midEast, currentNode);
        case CardinalPoints::SouthWest:
            return new Quadrant(midWest, center, c, midSouth, currentNode);
        case CardinalPoints::SouthEast:
            return new Quadrant(center, midEast, midSouth, d, currentNode);
    }
}

template<typename T>
void WorldQuadTree<T>::Insert(const T &data, const Coordinate &coordinate) {
    Quadrant<T> *current = root;
    for (size_t i = 0; i < deep; i++){
        if (!current->divided){
            current->northWest = CreateCardinalPoint(current, CardinalPoints::NorthWest);
            current->northEast = CreateCardinalPoint(current, CardinalPoints::NorthEast);
            current->southWest = CreateCardinalPoint(current, CardinalPoints::SouthWest);
            current->southEast = CreateCardinalPoint(current, CardinalPoints::SouthEast);
            current->divided = true;
        }
        if (current->northWest->cornerPoints.Belonging(coordinate))
            current = current->northWest;
        else if (current->northEast->cornerPoints.Belonging(coordinate))
            current= current->northEast;
        else if (current->southWest->cornerPoints.Belonging(coordinate))
            current = current->southWest;
        else if (current->southEast->cornerPoints.Belonging(coordinate))
            current = current->southEast;
    }
    current->data.push_back(data);
}

template<typename T>
std::vector<T> WorldQuadTree<T>::Search(const Coordinate &coordinate) const{
    Quadrant<T> *current = root;
    for (size_t i = 0; i < deep; i++){
        if (current->northWest->cornerPoints.Belonging(coordinate))
            current = current->northWest;
        else if (current->northEast->cornerPoints.Belonging(coordinate))
            current= current->northEast;
        else if (current->southWest->cornerPoints.Belonging(coordinate))
            current = current->southWest;
        else if (current->southEast->cornerPoints.Belonging(coordinate))
            current = current->southEast;
        if (current == nullptr)
            break;
    }
    if (current == nullptr)
        return std::vector<T>();
    else
        return current->data;
}
//
//template<typename T>
//std::vector<T> WorldQuadTree<T>::SearchInDistrict(const Coordinate &coordinate) {
//    Quadrant<T> *current = root;
//    for (size_t i = 0; i < deep - 1, current->northWest != nullptr; i++){
//        if (current->northWest->cornerPoints.Belonging(coordinate))
//            current = current->northWest;
//        else if (current->northEast->cornerPoints.Belonging(coordinate))
//            current= current->northEast;
//        else if (current->southWest->cornerPoints.Belonging(coordinate))
//            current = current->southWest;
//        else if (current->southEast->cornerPoints.Belonging(coordinate))
//            current = current->southEast;
//    }
//
//
//}
