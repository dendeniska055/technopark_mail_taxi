#include "Quadrant.h"

Geo::Quadrant::Quadrant(Coordinate northWest, Coordinate northEast,
                   Coordinate southWest, Coordinate southEast) : _northWest(std::move(northWest)),
                                                                 _northEast(std::move(northEast)),
                                                                 _southWest(std::move(southWest)),
                                                                 _southEast(std::move(southEast)) {
    center = Coordinate((northWest.first + southWest.first) / 2, (northWest.second + northEast.second) / 2);
}

const Geo::Coordinate &Geo::Quadrant::GetNorthWest() const {
    return _northWest;
}

const Geo::Coordinate &Geo::Quadrant::GetSouthWest() const {
    return _southWest;
}

const Geo::Coordinate &Geo::Quadrant::GetNorthEast() const {
    return _northEast;
}

const Geo::Coordinate &Geo::Quadrant::GetSouthEast() const {
    return _southEast;
}

const Geo::Coordinate &Geo::Quadrant::GetCenter() const {
    return center;
}

Geo::Coordinate Geo::Quadrant::GetMidNorth() const {
    return Coordinate(_northWest.first, (_northWest.second + _northEast.second) / 2);
}

Geo::Coordinate Geo::Quadrant::GetMidSouth() const {
    return Coordinate(_southWest.first, (_southWest.second + _southEast.second) / 2);
}

Geo::Coordinate Geo::Quadrant::GetMidWest() const {
    return Coordinate((_northWest.first + _southWest.first) / 2, _northWest.second);
}

Geo::Coordinate Geo::Quadrant::GetMidEast() const {
    return Coordinate((_northEast.first + _southEast.first) / 2, _northEast.second);
}

[[maybe_unused]] bool Geo::Quadrant::Belonging(const Coordinate &coordinate) const {
    bool f1 = coordinate.first <= _northWest.first && coordinate.second >= _northWest.second;
    bool f2 =  coordinate.first <= _northEast.first && coordinate.second <= _northEast.second;
    bool f3 = coordinate.first >= _southWest.first && coordinate.second >= _southWest.second;
    bool f4 = coordinate.first >= _southEast.first && coordinate.second <= _southEast.second;
    return coordinate.first <= _northWest.first && coordinate.second >= _northWest.second &&
           coordinate.first <= _northEast.first && coordinate.second <= _northEast.second &&
           coordinate.first >= _southWest.first && coordinate.second >= _southWest.second &&
           coordinate.first >= _southEast.first && coordinate.second <= _southEast.second;
}

std::ostream &Geo::operator<<(std::ostream &out, const Geo::Quadrant &obj) {
    out << obj._northWest.first << ' ' << obj._northWest.second << std::endl <<
        obj._northEast.first << ' ' << obj._northEast.second << std::endl <<
        obj._southWest.first << ' ' << obj._southWest.second << std::endl <<
        obj._southEast.first << ' ' << obj._southEast.second << std::endl;
    return out;
}
