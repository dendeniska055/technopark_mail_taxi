#pragma once
#include <vector>

//Интерфейс, отвечающий за взаимодействие с квадротри
template <typename ObjType, typename CoordinateType>
class IGeoIndexer {
public:
    virtual bool Add(const ObjType& object) = 0;
    virtual std::vector<ObjType> Search(const CoordinateType& coordinate) = 0;
    virtual bool Erase(const ObjType& object) = 0;
    virtual ~IGeoIndexer() = 0;
};