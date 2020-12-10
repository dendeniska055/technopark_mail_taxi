#include "../include/SHA1.h"

sha1::sha1(const std::string &s) {
  if (!s.empty()) this->process(s);
}

sha1 &sha1::process(const std::string &s) {
  h.process_bytes(s.c_str(), s.size());
  return *this;
}

sha1::operator std::string() {
  unsigned int digest[5];
  h.get_digest(digest);

  std::ostringstream buf;
  for(int i = 0; i < 5; ++i)
    buf << std::hex << std::setfill('0') << std::setw(8) << digest[i];

  return buf.str();
}
