#pragma once

#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>
#include <vector>
#include <thread>
#include <SingleContext.h>

namespace Cone::Web {
        class EventLoop : boost::noncopyable {
        private:
            std::vector<std::thread> threads;
            std::shared_ptr<Cone::Web::Context::SingleContext> context;
            size_t threadNum;
            bool running;
        private:
            EventLoop();
        public:
            static std::shared_ptr<EventLoop> GetEventLoop();
            void StartEventLoop(size_t _threadNum);
            [[nodiscard]] bool CheckingRunning() const;
            void PutCallBack(std::function<void()>);
            void EndEventLoop();
            ~EventLoop() = default;
        };
    }