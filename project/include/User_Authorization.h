#ifndef PROJECT__USER_AUTHORIZATION_H_
#define PROJECT__USER_AUTHORIZATION_H_

#include <string>
#include "Handler.h"

using std::string;

struct sign_in_data{
  string login;
  string password;
};

class UserAuthorizationHandler : public BaseHandler{
 public:
  std::string Handle(std::string request) override;
 private:
  void getUserData(std::string request);
  bool compareSignInData (sign_in_data user);
};

#endif //PROJECT__USER_AUTHORIZATION_H_
