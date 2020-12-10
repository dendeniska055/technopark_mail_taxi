#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <string>
#include <map>

#include "include/AuthorizationServer.h"
#include "include/SHA1.h"

struct HTTPRequest{
  std::string request;
  std::string ip;
  std::string cookie;
  std::string login;
  std::string password;
};

namespace bpt = boost::property_tree;

//int main() {
//  // init db
//  DatabaseManager data("../database/user.dat");
//  // !init db
//
//  const char* httpRequest = "../input/registration_request.JSON";
//  //parse JSON
//  bpt::ptree request;
//  bpt::read_json(httpRequest, request);
//
//  HTTPRequest req;
//
//  try {
//    req.request = request.get_child("Request").data();
//    req.cookie = request.get_child("Cookie").data();
//    req.ip = request.get_child("IP").data();
//  } catch (boost::exception &e) {
//    std::cerr << "BAD REQUEST";
//  }
//
//  AuthorizationServer auth(data);
//
//  if (req.request == "LogIn") {
//    try {
//      req.login = request.get_child("login").data();
//      req.password = request.get_child("pass").data();
//    } catch (boost::exception &e) {
//      std::cerr << "BAD AUTHORIZATION REQUEST";
//    }
//    Response authAns = auth.Login(UserLogPass{req.login, req.password});
//    sendResponse(req, authAns);
//    return 0;
//  }
//
//  if (req.request == "SignUp") {
//    try {
//      req.login = request.get_child("login").data();
//      req.password = request.get_child("pass").data();
//    } catch (boost::exception &e) {
//      std::cerr << "BAD AUTHORIZATION REQUEST";
//    }
//    Response authAns = auth.Registration(UserLogPass{req.login, req.password});
//    sendResponse(req, authAns);
//    return 0;
//  }
//
//  return auth.CheckCookie(req.cookie); // пересылаем дальше
//}
