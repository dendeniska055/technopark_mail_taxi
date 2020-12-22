#include "../include/AuthorizationServer.h"

// добавление в бд пользователя
std::string AuthorizationServer::Registration(const RegistrationRequest &user) {

  std::string passHash = std::string(sha1(user.password));

  std::random_device rd;
  std::uniform_int_distribution<int> uid(0, INT_MAX);

  std::string token = std::string(sha1(std::to_string(uid(rd))));
  token.resize(MAX_TOKEN_LENGHT);

  std::string sqlRequest = "INSERT INTO public.\"Users\" (id, login, password, token, email) VALUES (DEFAULT, '"
      + user.login + "', '" + passHash + "', '" + token + "', '" + user.email + "');";

  auto conn = pgbackend->connection();

  PQsendQuery(conn->connection().get(), sqlRequest.c_str());

  while (auto res_ = PQgetResult(conn->connection().get())) {
    if (PQresultStatus(res_) == PGRES_FATAL_ERROR) {
      std::cout << PQresultErrorMessage(res_) << std::endl;
    }
    PQclear(res_);
  }

  pgbackend->freeConnection(conn);

  return token;
}

UserInfoResponse AuthorizationServer::CheckToken(const std::string &token) {
  UserInfoResponse response;

  auto conn = pgbackend->connection();

  std::string demo = "SELECT login FROM public.\"Users\" WHERE (token) IN ('" + token + "');";

  PQsendQuery(conn->connection().get(), demo.c_str());

  while (auto res_ = PQgetResult(conn->connection().get())) {
    if (PQresultStatus(res_) == PGRES_TUPLES_OK && PQntuples(res_)) {
      auto ID = PQgetvalue(res_, 0, 0);
      std::cout << ID << std::endl;
      pgbackend->freeConnection(conn);
      response.login = ID;
      return response;
    }

    if (PQresultStatus(res_) == PGRES_FATAL_ERROR) {
      std::cout << PQresultErrorMessage(res_) << std::endl;
    }

    PQclear(res_);
  }

  pgbackend->freeConnection(conn);

  return response;
}

std::string AuthorizationServer::Login(const LoginRequest &user) {
  auto conn = pgbackend->connection();

  std::string passHash = std::string(sha1(user.password));

  std::string demo = "SELECT token FROM public.\"Users\" WHERE (login) IN ('" + user.login + "') AND (password) IN ('" + passHash + "');";
  PQsendQuery(conn->connection().get(), demo.c_str());

  while (auto res_ = PQgetResult(conn->connection().get())) {
    if (PQresultStatus(res_) == PGRES_TUPLES_OK && PQntuples(res_)) {
      auto ID = PQgetvalue(res_, 0, 0);
      std::cout << ID << std::endl;
      pgbackend->freeConnection(conn);
      return ID;
    }

    if (PQresultStatus(res_) == PGRES_FATAL_ERROR) {
      std::cout << PQresultErrorMessage(res_) << std::endl;
    }

    PQclear(res_);
  }

  pgbackend->freeConnection(conn);

  return "";
}
