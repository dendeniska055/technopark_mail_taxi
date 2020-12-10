#include "DatabaseManager.h"

void DatabaseManager::parseFile() {
   std::ifstream fin(fileDatabase);

   if (!fin) {
     throw "Error file open";
   }

   UserInfo user;

   while (!fin.eof()) {
     std::getline(fin, user.name, '|');
     std::getline(fin, user.pass, '|');
     std::getline(fin, user.cookie, '|');
     data.push_back(user);
   }
}

int DatabaseManager::Find(const std::vector<std::pair<std::string, std::string>> &v) {
  UserInfo user;

  std::for_each(v.begin(), v.end(), [&user](const std::pair<std::string, std::string>& pair) {
    if (pair.first == "login") {
      user.name = pair.second;
    } else {
      if (pair.first == "password") {
        user.pass = pair.second;
      } else {
        if (pair.first == "cookie") {
          user.cookie = pair.second;
        }
      }
    }
  });

  for (int i = 0; i < data.size(); ++i) {
    if (!user.name.empty() && user.name == data[i].name) {
      if (user.pass.empty() || user.pass == data[i].pass) {
        return i;
      } else {
        return -1;
      }
    }

    if (!user.cookie.empty() && user.cookie == data[i].cookie) {
      return i;
    }
  }

  return -1;
}
int DatabaseManager::Add(const std::vector<std::pair<std::string, std::string>> &v, int position) {
  UserInfo user;

  std::for_each(v.begin(), v.end(), [&user](const std::pair<std::string, std::string>& pair) {
    if (pair.first == "login") {
      user.name = pair.second;
    } else {
      if (pair.first == "password") {
        user.pass = pair.second;
      } else {
        if (pair.first == "cookie") {
          user.cookie = pair.second;
        }
      }
    }
  });

  if (position == -1) {
    data.push_back(user);
    return static_cast<int>(data.size()) - 1;
  }

  data[position] = user;
  return position;
}
