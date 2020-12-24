#pragma once

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