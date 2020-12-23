#include "../include/AuthorizationServer.h"

std::string AuthorizationServer::Registration(const RegistrationRequest &user) {
  std::string passHash = std::string(sha1(user.password));

  std::random_device rd;
  std::uniform_int_distribution<int> uid(0, INT_MAX);

  std::string token = std::string(sha1(std::to_string(uid(rd))));
  token.resize(MAX_TOKEN_LENGHT);

  std::string sqlRequest = "INSERT INTO public.\"Users\" (id, login, password, token, email) VALUES (DEFAULT, '"
      + user.login + "', '" + passHash + "', '" + token + "', '" + user.email + "');";

  database->Request(sqlRequest);

  return token;
}

UserInfoResponse AuthorizationServer::CheckToken(const std::string &token) {
  UserInfoResponse response;

  std::string request = "SELECT id, login FROM public.\"Users\" WHERE (token) IN ('" + token + "');";

  std::vector<std::string> a = database->Request(request);

  for (const auto& i : a) {
    std::cout << i << " ";
  }

  return response;
}

std::string AuthorizationServer::Login(const LoginRequest &user) {
  std::string passHash = std::string(sha1(user.password));

  std::string request = "SELECT token FROM public.\"Users\" WHERE (login) IN ('" + user.login + "') AND (password) IN ('" + passHash + "');";

  std::vector<std::string> res = database->Request(request);

  return res[0];
}
