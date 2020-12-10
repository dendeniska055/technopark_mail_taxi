#include "Quadrant.h"

Cone::Geo::Quadrant::Quadrant(Coordinate northWest, Coordinate northEast,
                   Coordinate southWest, Coordinate southEast) : _northWest(std::move(northWest)),
                                                                 _northEast(std::move(northEast)),
                                                                 _southWest(std::move(southWest)),
                                                                 _southEast(std::move(southEast)) {
    center = Coordinate((northWest.first + southWest.first) / 2, (northWest.second + northEast.second) / 2);
}

const Cone::Geo::Coordinate &Cone::Geo::Quadrant::GetNorthWest() const {
    return _northWest;
}

const Cone::Geo::Coordinate &Cone::Geo::Quadrant::GetSouthWest() const {
    return _southWest;
}

const Cone::Geo::Coordinate &Cone::Geo::Quadrant::GetNorthEast() const {
    return _northEast;
}

const Cone::Geo::Coordinate &Cone::Geo::Quadrant::GetSouthEast() const {
    return _southEast;
}

const Cone::Geo::Coordinate &Cone::Geo::Quadrant::GetCenter() const {
    return center;
}

Cone::Geo::Coordinate Cone::Geo::Quadrant::GetMidNorth() const {
    return Coordinate(_northWest.first, (_northWest.second + _northEast.second) / 2);
}

Cone::Geo::Coordinate Cone::Geo::Quadrant::GetMidSouth() const {
    return Coordinate(_southWest.first, (_southWest.second + _southEast.second) / 2);
}

Cone::Geo::Coordinate Cone::Geo::Quadrant::GetMidWest() const {
    return Coordinate((_northWest.first + _southWest.first) / 2, _northWest.second);
}

Cone::Geo::Coordinate Cone::Geo::Quadrant::GetMidEast() const {
    return Coordinate((_northEast.first + _southEast.first) / 2, _northEast.second);
}

[[maybe_unused]] bool Cone::Geo::Quadrant::Belonging(const Coordinate &coordinate) const {
    return coordinate.first <= _northWest.first && coordinate.second >= _northWest.second &&
           coordinate.first <= _northEast.first && coordinate.second <= _northEast.second &&
           coordinate.first >= _southWest.first && coordinate.second >= _southWest.second &&
           coordinate.first >= _southEast.first && coordinate.second <= _southEast.second;
}

std::ostream &Cone::Geo::operator<<(std::ostream &out, const Cone::Geo::Quadrant &obj) {
    out << obj._northWest.first << ' ' << obj._northWest.second << std::endl <<
        obj._northEast.first << ' ' << obj._northEast.second << std::endl <<
        obj._southWest.first << ' ' << obj._southWest.second << std::endl <<
        obj._southEast.first << ' ' << obj._southEast.second << std::endl;
    return out;
}
