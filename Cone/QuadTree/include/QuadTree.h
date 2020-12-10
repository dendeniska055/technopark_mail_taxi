
#pragma once
#include <functional>
#include <queue>
#include <stack>
#include <iostream>
#include <atomic>
#include "Quadrant.h"

namespace Cone::Geo {
        template<typename DataType, typename LockType>
        class QuadTreeNode {
        public:
            static const size_t cardinalPointsNum = 4;
            static const size_t northWest = 0;
            static const size_t northEast = 1;
            static const size_t southWest = 2;
            static const size_t southEast = 3;
        private:
            std::atomic<QuadTreeNode<DataType, LockType> *> cardinalPoints[cardinalPointsNum];
            Quadrant quadrant;
            std::atomic<bool> childrenIndicator;
            DataType data;
            LockType lock;
        public:
            explicit QuadTreeNode(Quadrant quad);
            void ChildrenSpawn();
            size_t ChoiceNextNode(const Coordinate &coordinate);
            [[nodiscard]] const Quadrant &GetQuadrant() const;
            [[nodiscard]] bool ChildrenCheck() const;
            QuadTreeNode<DataType, LockType> *GetCardinalPoint(size_t cardinalPoint);
            LockType &GetLock();
            DataType &GetData();
            [[nodiscard]] bool Belonging(const Coordinate &coordinate) const;
            ~QuadTreeNode() = default;
        };

        template<typename DataType, typename LockType>
        void QuadTreeNode<DataType, LockType>::ChildrenSpawn() {
            QuadTreeNode<DataType, LockType> *expectation = nullptr;
            Quadrant quad(quadrant.GetNorthWest(), quadrant.GetMidNorth(),
                          quadrant.GetMidWest(), quadrant.GetCenter());
            auto *child = new QuadTreeNode<DataType, LockType>(quad);
            quad = Quadrant(
                    quadrant.GetMidNorth(), quadrant.GetNorthEast(),
                    quadrant.GetCenter(), quadrant.GetMidEast());
            if (cardinalPoints[northWest].compare_exchange_strong(expectation, child))
                child = new QuadTreeNode<DataType, LockType>(quad);
            else {
                child->quadrant = quad;
            }
            quad = Quadrant(quadrant.GetMidWest(), quadrant.GetCenter(),
                            quadrant.GetSouthWest(), quadrant.GetMidSouth());
            if (cardinalPoints[northEast].compare_exchange_strong(expectation, child))
                child = new QuadTreeNode<DataType, LockType>(quad);
            else {
                child->quadrant = quad;
            }
            quad = Quadrant(quadrant.GetCenter(), quadrant.GetMidEast(),
                            quadrant.GetMidSouth(), quadrant.GetSouthEast());
            if (cardinalPoints[southWest].compare_exchange_strong(expectation, child))
                child = new QuadTreeNode<DataType, LockType>(quad);
            else {
                child->quadrant = quad;
            }
            if (!cardinalPoints[southEast].compare_exchange_weak(expectation, child)) {
                delete child;
            }
            childrenIndicator.store(true, std::memory_order_release);
        }

        template<typename DataType, typename LockType>
        size_t QuadTreeNode<DataType, LockType>::ChoiceNextNode(const Coordinate &coordinate) {
            Coordinate center = quadrant.GetCenter();
            if (coordinate.first >= center.first && coordinate.second <= center.second)
                return northWest;
            else if (coordinate.first >= center.first && coordinate.second >= center.second)
                return northEast;
            else if (coordinate.first <= center.first && coordinate.second <= center.second)
                return southWest;
            else if (coordinate.first <= center.first && coordinate.second >= center.second)
                return southEast;
        }

        template<typename DataType, typename LockType>
        const Quadrant &QuadTreeNode<DataType, LockType>::GetQuadrant() const {
            return quadrant;
        }

        template<typename DataType, typename LockType>
        bool QuadTreeNode<DataType, LockType>::ChildrenCheck() const {
            return childrenIndicator.load(std::memory_order_acquire);
        }

        template<typename DataType, typename LockType>
        DataType &QuadTreeNode<DataType, LockType>::GetData() {
            return data;
        }

        template<typename DataType, typename LockType>
        QuadTreeNode<DataType, LockType>::QuadTreeNode(Quadrant quad) : quadrant(quad) {
            cardinalPoints[northWest].store(nullptr, std::memory_order_relaxed);
            cardinalPoints[northEast].store(nullptr, std::memory_order_relaxed);
            cardinalPoints[southWest].store(nullptr, std::memory_order_relaxed);
            cardinalPoints[southEast].store(nullptr, std::memory_order_relaxed);
            childrenIndicator.store(false, std::memory_order_relaxed);
        }

        template<typename DataType, typename LockType>
        LockType &QuadTreeNode<DataType, LockType>::GetLock() {
            return lock;
        }

        template<typename DataType, typename LockType>
        QuadTreeNode<DataType, LockType> *QuadTreeNode<DataType, LockType>::GetCardinalPoint(size_t cardinalPoint) {
            return cardinalPoints[cardinalPoint].load();
        }

        template<typename DataType, typename LockType>
        bool QuadTreeNode<DataType, LockType>::Belonging(const Coordinate &coordinate) const {
            return quadrant.Belonging(coordinate);
        }

        template<typename DataType, typename LockType>
        class QuadTreeObjectMover {
        private:
            std::stack<QuadTreeNode<DataType, LockType> *> callStack;
            const QuadTreeNode<DataType, LockType> *root;
            size_t deep;
        public:
            QuadTreeObjectMover(std::stack<QuadTreeNode<DataType, LockType> *> callStack,
                                const QuadTreeNode<DataType, LockType> *root,
                                size_t deep);

            bool operator()(Coordinate coordinate, std::function<void(DataType &, const Quadrant &,
                    LockType &, bool)>);
            DataType &GetData();
            const Quadrant &GetQuadrant();
            ~QuadTreeObjectMover() = default;
        };

        template<typename DataType, typename LockType>
        const Quadrant &QuadTreeObjectMover<DataType, LockType>::GetQuadrant() {
            return callStack.top()->GetQuadrant();
        }
        template<typename DataType, typename LockType>
        bool QuadTreeObjectMover<DataType, LockType>::operator()(Coordinate coordinate,
                                                                 std::function<void(DataType &, const Quadrant &,
                                                                                    LockType &, bool)> handler) {
            if (root->Belonging(coordinate)) {
                QuadTreeNode<DataType, LockType> *current = callStack.top();
                if (current->Belonging(coordinate))
                    return false;
                else {
                    size_t currentDeep = deep;
                    handler(current->GetData(), current->GetQuadrant(), current->GetLock(), true);
                    while (!callStack.top()->Belonging(coordinate)) {
                        callStack.pop();
                        currentDeep--;
                    }
                    current = callStack.top();
                    for (; currentDeep < deep; currentDeep++) {
                        if (!current->ChildrenCheck())
                            current->ChildrenSpawn();
                        size_t cardinalPoint = current->ChoiceNextNode(coordinate);
                        QuadTreeNode<DataType, LockType> *nextNode = nullptr;
                        while (nextNode == nullptr)
                            nextNode = current->GetCardinalPoint(cardinalPoint);
                        current = nextNode;
                        callStack.push(current);
                    }
                    handler(current->GetData(), current->GetQuadrant(), current->GetLock(), false);
                    return true;
                }
            } else {
                return false;
            }
        }
        template<typename DataType, typename LockType>
        QuadTreeObjectMover<DataType, LockType>::QuadTreeObjectMover(
                std::stack<QuadTreeNode<DataType, LockType> *> callStack,
                const QuadTreeNode<DataType, LockType> *root,
                size_t deep) : callStack(std::move(callStack)),
                               root(root), deep(deep) {
        }

        template<typename DataType, typename LockType>
        DataType &QuadTreeObjectMover<DataType, LockType>::GetData() {
            return callStack.top()->GetData();
        }

        template<typename DataType, typename LockType>
        class QuadTree {
        private:
            std::atomic<QuadTreeNode<DataType, LockType> *> root;
            size_t deep;
        public:
            QuadTree(Coordinate northWest, Coordinate northEast,
                     Coordinate southWest, Coordinate southEast, size_t deep);
            void Update(Coordinate coordinate, std::function<void(DataType &, const Quadrant &, LockType &)>);
            QuadTreeObjectMover<DataType, LockType> UpdateAndGetMover(Coordinate coordinate,
                                                                      std::function<void(DataType &, const Quadrant &,
                                                                                         LockType &)>);
            void BruteForceAndUpdate(std::queue<Coordinate>, std::function<void(DataType &, const Quadrant &,
                                                                                LockType &, std::queue<Coordinate> &)>);
            void BruteForce(std::queue<Coordinate>, std::function<void(DataType &, const Quadrant &,
                                                                       LockType &, std::queue<Coordinate> &)>);
        };

        template<typename DataType, typename LockType>
        QuadTree<DataType, LockType>::QuadTree(Coordinate northWest, Coordinate northEast, Coordinate southWest,
                                               Coordinate southEast, size_t deep) : deep(deep) {

            auto *temp = new QuadTreeNode<DataType, LockType>(
                    Quadrant(northWest, northEast, southWest, southEast));
            root.store(temp);
        }

        template<typename DataType, typename LockType>
        void QuadTree<DataType, LockType>::Update(Coordinate coordinate,
                                                  std::function<void(DataType &, const Quadrant &,
                                                                     LockType &)> handler) {
            QuadTreeNode<DataType, LockType> *current = root.load();
            if (current->Belonging(coordinate)) {
                for (size_t i = 0; i < deep; i++) {
                    if (!current->ChildrenCheck())
                        current->ChildrenSpawn();
                    size_t cardinalPoint = current->ChoiceNextNode(coordinate);
                    QuadTreeNode<DataType, LockType> *nextNode = nullptr;
                    while (nextNode == nullptr)
                        nextNode = current->GetCardinalPoint(cardinalPoint);
                    current = nextNode;
                }
                handler(current->GetData(), current->GetQuadrant(), current->GetLock());
            }
        }


        template<typename DataType, typename LockType>
        QuadTreeObjectMover<DataType, LockType> QuadTree<DataType, LockType>::UpdateAndGetMover(Coordinate coordinate,
                                                                                                std::function<void(
                                                                                                        DataType &,
                                                                                                        const Quadrant &,
                                                                                                        LockType &)> handler) {
            std::stack<QuadTreeNode<DataType, LockType> *> callStack;
            QuadTreeNode<DataType, LockType> *current = root.load();
            callStack.push(current);
            if (current->Belonging(coordinate)) {
                for (size_t i = 0; i < deep; i++) {
                    if (!current->ChildrenCheck())
                        current->ChildrenSpawn();
                    size_t cardinalPoint = current->ChoiceNextNode(coordinate);
                    QuadTreeNode<DataType, LockType> *nextNode = nullptr;
                    while (nextNode == nullptr)
                        nextNode = current->GetCardinalPoint(cardinalPoint);
                    current = nextNode;
                    callStack.push(current);
                }
                handler(current->GetData(), current->GetQuadrant(), current->GetLock());
            }
            return QuadTreeObjectMover<DataType, LockType>(std::move(callStack), root, deep);
        }

        template<typename DataType, typename LockType>
        void QuadTree<DataType, LockType>::BruteForceAndUpdate(std::queue<Coordinate> coordinates,
                                                               std::function<void(DataType &, const Quadrant &,
                                                                                  LockType &,
                                                                                  std::queue<Coordinate> &)> handler) {
            std::stack<QuadTreeNode<DataType, LockType> *> callStack;
            auto current = root.load();
            auto _root = current;
            size_t currentDeep = 0;
            while (!coordinates.empty()) {
                if (_root->Belonging(coordinates.front())) {
                    for (; currentDeep < deep; currentDeep++) {
                        if (!current->ChildrenCheck())
                            current->ChildrenSpawn();
                        size_t cardinalPoint = current->ChoiceNextNode(coordinates.front());
                        QuadTreeNode<DataType, LockType> *nextNode = nullptr;
                        while (nextNode == nullptr)
                            nextNode = current->GetCardinalPoint(cardinalPoint);
                        current = nextNode;
                        callStack.push(current);
                    }
                    handler(current->GetData(), current->GetQuadrant(), current->GetLock(), coordinates);
                    coordinates.pop();
                    if (!coordinates.empty()) {
                        while (!callStack.top()->Belonging(coordinates.front())) {
                            callStack.pop();
                            currentDeep--;
                        }
                        current = callStack.top();
                    }
                } else {
                    coordinates.pop();
                }
            }
        }

        template<typename DataType, typename LockType>
        void QuadTree<DataType, LockType>::BruteForce(std::queue<Coordinate> coordinates,
                                                      std::function<void(DataType &, const Quadrant &, LockType &,
                                                                         std::queue<Coordinate> &)> handler) {
            std::stack<QuadTreeNode<DataType, LockType> *> callStack;
            auto current = root.load();
            auto _root = current;
            size_t currentDeep = 0;
            while (!coordinates.empty()) {
                if (_root->Belonging(coordinates.front())) {
                    for (; currentDeep < deep; currentDeep++) {
                        if (current->ChildrenCheck()) {
                            size_t cardinalPoint = current->ChoiceNextNode(coordinates.front());
                            current = current->GetCardinalPoint(cardinalPoint);
                            callStack.push(current);
                        } else
                            break;
                    }
                    if (currentDeep == deep)
                        handler(current->GetData(), current->GetQuadrant(), current->GetLock(), coordinates);
                    coordinates.pop();
                    if (!coordinates.empty()) {
                        while (!callStack.top()->Belonging(coordinates.front())) {
                            callStack.pop();
                            currentDeep--;
                        }
                        current = callStack.top();
                    }
                } else {
                    coordinates.pop();
                }
            }
        }
    }