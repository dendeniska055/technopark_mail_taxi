#include "EventLoop.h"
#include "CDSHelper.h"
std::shared_ptr<Cone::Web::EventLoop> Cone::Web::EventLoop::GetEventLoop() {
    static std::shared_ptr<EventLoop> singleEventLoop(new EventLoop);
    return singleEventLoop;
}

void Cone::Web::EventLoop::StartEventLoop(size_t _threadNum) {
    threadNum = _threadNum;
    for (size_t i = 0; i < _threadNum; i++){
        threads.emplace_back(std::thread([this](){
            Cone::CDSHelper::CDSThreadInitializer init;
            context->GetContext().run();
        }));
    }
    running = true;
}

void Cone::Web::EventLoop::EndEventLoop() {
    context->GetContext().stop();
    for (size_t i = 0; i < threadNum; i++)
        threads[i].join();
    threads = std::vector<std::thread>();
    threadNum = 0;
}

Cone::Web::EventLoop::EventLoop() {
    threadNum = 0;
    running = false;
    context = Cone::Web::Context::SingleContext::GetSingleContext();
}

[[maybe_unused]] bool Cone::Web::EventLoop::CheckingRunning() const {
    return running;
}

void Cone::Web::EventLoop::PutCallBack(std::function<void()> handler) {
    context->GetContext().post(handler);
}
