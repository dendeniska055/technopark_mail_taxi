#ifndef UNTITLED1_PROJECT_AUTHORIZATION_SERVER_SHA1_H_
#define UNTITLED1_PROJECT_AUTHORIZATION_SERVER_SHA1_H_

#include <sstream>
#include <iomanip>
#include <boost/uuid/detail/sha1.hpp>

class sha1 {
 public:
  sha1(const std::string &s = "");

  sha1& process(const std::string &s);

  operator std::string();
 private:
  boost::uuids::detail::sha1 h;
};

#endif //UNTITLED1_PROJECT_AUTHORIZATION_SERVER_SHA1_H_
