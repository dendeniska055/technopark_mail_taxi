#pragma once


//Интерфейс кэш таблицы, хранящей данные о паре водитель - клмент
template <typename KeyType, typename ValueType>
class ICashTable {
public:
    virtual bool Add(KeyType key, ValueType value) = 0;
    virtual ValueType Search(KeyType key) = 0;
    virtual bool Erase(KeyType key) = 0;
    virtual ~ICashTable() = 0;
};
