#ifndef UNTITLED1_PROJECT_AUTHORIZATION_SERVER_AUTHORIZATIONSERVER_H_
#define UNTITLED1_PROJECT_AUTHORIZATION_SERVER_AUTHORIZATIONSERVER_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>


#define MAX_TOKEN_LENGHT 12

#include "SHA1.h"

struct UserInfo {
  std::string login;
  std::string password;
  bool isDriver;
};

struct UserResponse {
  std::string login;
  bool isDriver;

  explicit UserResponse(const std::string& _login = "", bool v = false) : login(_login), isDriver(v) {}
};

struct UserLogPassword {
  std::string login;
  std::string password;
};


class AuthorizationServer {
 public:
  AuthorizationServer();
  ~AuthorizationServer() = default;

  std::string Registration(const UserInfo &user); // добавляет пользователя и возвращает токен
  std::string Login(const UserLogPassword &user); // по логину и паролю выдает токен
  UserResponse CheckToken(const std::string &token); // возвращает данные по токену

 private:
  std::map<std::string, UserInfo> data; // токен и инфа о пользователе
};

#endif //UNTITLED1_PROJECT_AUTHORIZATION_SERVER_AUTHORIZATIONSERVER_H_
