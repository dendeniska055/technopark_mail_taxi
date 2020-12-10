#include "SingleContext.h"


std::shared_ptr<Cone::Web::Context::SingleContext> Cone::Web::Context::SingleContext::GetSingleContext() {
    static std::shared_ptr<SingleContext> Context(new SingleContext);
    return Context;
}

boost::asio::io_context &Cone::Web::Context::SingleContext::GetContext() {
    return context;
}
