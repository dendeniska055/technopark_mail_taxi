#include "../include/AuthorizationServer.h"

std::string AuthorizationServer::Registration(const UserInfo &user) {
  for (auto& [token, currentUser] : data) {
    if (currentUser.login == user.login) {
      return "";
    }
  }

  std::string token = std::string(sha1(user.login));
  token.resize(MAX_TOKEN_LENGHT);
  data[token] = user;

  return token;
}

UserResponse AuthorizationServer::CheckToken(const std::string &token) {
  UserInfo user;
  UserResponse res;

  try {
    user = data.at(token);
    res.login = user.login;
    res.isDriver = user.isDriver;

  } catch (std::exception& e) {
    std::cerr << "Auth :: BadToken";
    return res;
  }

  return res;
}

std::string AuthorizationServer::Login(const UserLogPassword &user) {
  for (auto& [token, currentUser] : data) {
    if (currentUser.login == user.login && currentUser.password == user.password) {
      return token;
    }
  }

  return "";
}
