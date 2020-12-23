#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <string>
#include <map>
#include <limits>

#include "include/DatabaseManager.h"
#include "include/AuthorizationServer.h"


int main(){
  auto data = std::make_shared<DatabaseManager>();
  AuthorizationServer authServ(data);

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
//  authServ.CheckToken("ba50c99c923f");

  std::string login;
  std::string password;
  std::string id;

  std::string request1 = "SELECT id, carName, carModel, canAnimalDrive, canChildDrive FROM public.\"Drivers\" WHERE (login) IN ('" + login + "') AND (password) IN ('" + password + "');";
  std::string request2 = "SELECT carName, carModel FROM public.\"Drivers\" WHERE (id) IN ('" + id + "');";
}
