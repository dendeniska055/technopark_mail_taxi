#ifndef UNTITLED1_PROJECT_DATABASE_DATABASEMANAGER_H_
#define UNTITLED1_PROJECT_DATABASE_DATABASEMANAGER_H_

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

class DatabaseManager {
 public:
  explicit DatabaseManager(const char* filename) : fileDatabase(filename) {
    parseFile();
  }

  ~DatabaseManager() = default;

  int Add(const std::vector<std::pair<std::string, std::string>>& v, int position = -1);
  int Find(const std::vector<std::pair<std::string, std::string>>& v);

 private:

  void parseFile(); // выполняется при создании объекта
//  void addNodeInFile(); // выполняется при AddUser

  const char* fileDatabase;
  std::vector<UserInfo> data;
};

#endif //UNTITLED1_PROJECT_DATABASE_DATABASEMANAGER_H_
