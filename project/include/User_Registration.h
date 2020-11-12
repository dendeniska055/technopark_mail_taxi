#ifndef PROJECT__USER_REGISTRATION_H_
#define PROJECT__USER_REGISTRATION_H_

#include <string>

class User_Registration {
 public:
  User_Registration() = default;

  void GetDataFromUser();

 private:
  void sendEmail(std::string email);
  void uploadUser(std::string);
};

#endif //PROJECT__USER_REGISTRATION_H_
