#ifndef PROJECT__HANDLER_H_
#define PROJECT__HANDLER_H_

#include <string>

class Handler {
 public:
  virtual Handler *SetNext(Handler *handler) = 0;
  virtual std::string Handle(std::string request) = 0;
};

class BaseHandler : public Handler {
 private:
  Handler *next_handler_;

 public:
  BaseHandler() : next_handler_(nullptr) {
  }
  Handler *SetNext(Handler *handler) override;
  std::string Handle(std::string request) override;
};



#endif //PROJECT__HANDLER_H_
