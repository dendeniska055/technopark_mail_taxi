#ifndef UNTITLED1_PROJECT_AUTHORIZATION_SERVER_AUTHORIZATIONSERVER_H_
#define UNTITLED1_PROJECT_AUTHORIZATION_SERVER_AUTHORIZATIONSERVER_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <random>

#define MAX_TOKEN_LENGHT 12

#include "pgbackend.h"
#include "pgconnection.h"
#include "SHA1.h"

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
  AuthorizationServer(std::shared_ptr<PGBackend> _pgbackend) : pgbackend(_pgbackend) {}
  ~AuthorizationServer() = default;

  std::string Registration(const RegistrationRequest &user); // добавляет пользователя и возвращает токен пользователю
  std::string Login(const LoginRequest &user); // по логину и паролю возвращает токен пользователю или пустую строку, если токена нет
  UserInfoResponse CheckToken(const std::string &token); // возвращает данные по токену???(какие данные?)

 private:
  std::shared_ptr<PGBackend> pgbackend;
};

#endif //UNTITLED1_PROJECT_AUTHORIZATION_SERVER_AUTHORIZATIONSERVER_H_
