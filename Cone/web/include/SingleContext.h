#pragma once
#include "boost/asio.hpp"
#include <boost/core/noncopyable.hpp>

namespace Cone::Web::Context {
            class SingleContext {
            private:
                boost::asio::io_context context;
                SingleContext() = default;
            public:
                static std::shared_ptr<SingleContext> GetSingleContext();
                boost::asio::io_context &GetContext();
            };
        }