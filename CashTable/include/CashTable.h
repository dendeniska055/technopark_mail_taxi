#pragma once
#include <functional>
#include <atomic>
#include <cds/init.h>
#include <cds/container/bronson_avltree_map_rcu.h>
#include <cds/urcu/general_buffered.h>

template <class Key, class Val>
class ICashTable{
public:
    virtual bool Find(const Key &key, std::function<void(const Key &, Val &)> handler) = 0;
    virtual bool Insert(const Key &key, const Val& val) = 0;
    virtual bool Update(const Key &key, std::function<void(bool, const Key &, Val &)> handler) = 0;
    virtual bool Contains(const Key& key) = 0;
    virtual bool Erase(const Key& key) = 0;
    [[nodiscard]] virtual size_t Size() const = 0;
    [[nodiscard]] virtual bool Empty() const = 0;
    virtual ~ICashTable() = default;

};

typedef cds::urcu::gc< cds::urcu::general_buffered<> >  rcu_gpb;

template <class Key, class Val>
class CashTableLockFree final : public ICashTable<Key, Val> {
private:
    cds::container::BronsonAVLTreeMap<rcu_gpb, Key, Val> map;
public:
    CashTableLockFree() = default;
    ~CashTableLockFree() override;
    bool Find(const Key& key, std::function<void(const Key& key, Val& val)> handler) override;
    bool Insert(const Key& key, const Val& val) override;
    bool Update(const Key &key, std::function<void(bool, const Key &, Val &)> handler) override;
    bool Contains(const Key& key) override;
    bool Erase(const Key& key) override;
    [[nodiscard]] size_t Size() const override;
    [[nodiscard]] bool Empty() const override;
};

template<class Key, class Val>
CashTableLockFree<Key, Val>::~CashTableLockFree() {
    map.clear();
}

template<class Key, class Val>
bool CashTableLockFree<Key, Val>::Find(const Key &key, std::function<void(const Key&, Val&)> handler) {
    return map.find(key, handler);
}

template<class Key, class Val>
bool CashTableLockFree<Key, Val>::Insert(const Key &key, const Val& val) {
    bool success = map.insert(key, val);
    return success;
}

template<class Key, class Val>
bool
CashTableLockFree<Key, Val>::Update(const Key &key, std::function<void(bool ,const Key &, Val &)> handler) {
    std::pair<bool, bool> res = map.update(key, handler);
    return res.first;
}

template<class Key, class Val>
bool CashTableLockFree<Key, Val>::Erase(const Key &key) {
    bool success = map.erase(key);
    return success;
}

template<class Key, class Val>
size_t CashTableLockFree<Key, Val>::Size() const {
    return 0;
}

template<class Key, class Val>
bool CashTableLockFree<Key, Val>::Empty() const{
    return map.empty();
}

template<class Key, class Val>
bool CashTableLockFree<Key, Val>::Contains(const Key &key) {
    return map.contains(key);
}