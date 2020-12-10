#pragma once
#include <iostream>

namespace Cone::Geo {
    typedef std::pair<double, double> Coordinate;
    class Quadrant {
    private:
        Coordinate _northWest;
        Coordinate _northEast;
        Coordinate _southWest;
        Coordinate _southEast;
        Coordinate center;
    public:
        Quadrant(Coordinate northWest, Coordinate northEast, Coordinate southWest, Coordinate southEast);
        [[nodiscard]] const Coordinate &GetNorthWest() const;
        [[nodiscard]] const Coordinate &GetSouthWest() const;
        [[nodiscard]] const Coordinate &GetNorthEast() const;
        [[nodiscard]] const Coordinate &GetSouthEast() const;
        [[nodiscard]] const Coordinate &GetCenter() const;
        [[nodiscard]] Coordinate GetMidNorth() const;
        [[nodiscard]] Coordinate GetMidSouth() const;
        [[nodiscard]] Coordinate GetMidWest() const;
        [[nodiscard]] Coordinate GetMidEast() const;
        [[nodiscard]] bool Belonging(const Coordinate &coordinate) const;
        Quadrant &operator=(const Quadrant &quad) = default;
        friend std::ostream &operator<<(std::ostream &out, const Quadrant &obj);
        ~Quadrant() = default;
    };
}