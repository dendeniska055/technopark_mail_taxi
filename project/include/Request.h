#ifndef PROJECT__REQUEST_H_
#define PROJECT__REQUEST_H_

#include <string>

class ParseRequest {
 public:
  ParseRequest(std::string request);

  void OrderCreation();
  void TakeOrder();
  void TakeInfoByOrder();
  void RegistrationUser();

 private:
  std::string request;
};

#endif //PROJECT__REQUEST_H_
