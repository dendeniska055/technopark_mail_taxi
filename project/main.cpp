#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <string>
#include <map>
#include <limits>

#include "include/pgbackend.h"
#include "include/DatabaseManager.h"
#include "include/pgconnection.h"
#include "include/AuthorizationServer.h"


int main(){
  auto pgbackend = std::make_shared<PGBackend>();
  AuthorizationServer authServ(pgbackend);

//  std::string login = "login";
//  std::string password = "pass";
//  std::string email = "mail";
//
//
//  for (int i = 0; i < 100; ++i) {
//    RegistrationRequest req = {login + std::to_string(i), password + std::to_string(i), email + std::to_string(i) + "@mail.ru"};
//    authServ.Registration(req);
//  }

//  std::cout << authServ.Login({"login5", "pass5"}) << "\n" << "OK";
//  std::cout << authServ.CheckToken("ba50c99c923f").login << "\n" << "OK";
}
