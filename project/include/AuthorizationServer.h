#ifndef UNTITLED1_PROJECT_AUTHORIZATION_SERVER_AUTHORIZATIONSERVER_H_
#define UNTITLED1_PROJECT_AUTHORIZATION_SERVER_AUTHORIZATIONSERVER_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <random>


#include "DatabaseManager.h"
#include "SHA1.h"

#define MAX_TOKEN_LENGHT 12

struct RegistrationRequest{
  std::string login;
  std::string password;
  std::string email;
};

struct LoginRequest{
  std::string login;
  std::string password;
};

struct UserInfoResponse{
  std::string login;
};


class AuthorizationServer {
 public:
  AuthorizationServer(std::shared_ptr<DatabaseManager> _database) : database(_database) {}
  ~AuthorizationServer() = default;

  std::string Registration(const RegistrationRequest &user); // добавляет пользователя и возвращает токен пользователю
  std::string Login(const LoginRequest &user); // по логину и паролю возвращает токен пользователю или пустую строку, если токена нет
  UserInfoResponse CheckToken(const std::string &token);

 private:
  std::shared_ptr<DatabaseManager> database;
};

#endif //UNTITLED1_PROJECT_AUTHORIZATION_SERVER_AUTHORIZATIONSERVER_H_
