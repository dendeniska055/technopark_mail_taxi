#ifndef UNTITLED1_PROJECT_DATABASE_DATABASEMANAGER_H_
#define UNTITLED1_PROJECT_DATABASE_DATABASEMANAGER_H_

#include <memory>
#include <mutex>
#include <string>
#include <queue>
#include <condition_variable>
#include <libpq-fe.h>
#include "DatabaseConnection.h"


class DatabaseManager {
 public:
  explicit DatabaseManager(int poolCount = 10) : POOL(poolCount) {
    createPool();
  }

  std::vector<std::string> Request(const std::string& request);

 private:
  void createPool();

  std::shared_ptr<DatabaseConnection> connection();
  void freeConnection(const std::shared_ptr<DatabaseConnection>&);

  std::mutex m_mutex;
  std::condition_variable m_condition;
  std::queue<std::shared_ptr<DatabaseConnection>> m_pool;

  const int POOL;
};

#endif //UNTITLED1_PROJECT_DATABASE_DATABASEMANAGER_H_
