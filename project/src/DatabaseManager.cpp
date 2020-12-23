#include "../include/DatabaseManager.h"
#include <iostream>

void DatabaseManager::createPool() {
  std::lock_guard<std::mutex> locker(m_mutex);

  for (auto i = 0; i < POOL; ++i) {
    m_pool.emplace(std::make_shared<DatabaseConnection>());
  }
}

std::shared_ptr<DatabaseConnection> DatabaseManager::connection() {
  std::unique_lock<std::mutex> lock_(m_mutex);

  while (m_pool.empty()) {
    m_condition.wait(lock_);
  }

  auto conn_ = m_pool.front();
  m_pool.pop();

  return conn_;
}

void DatabaseManager::freeConnection(const std::shared_ptr<DatabaseConnection>& conn_) {
  std::unique_lock<std::mutex> lock_(m_mutex);
  m_pool.push(conn_);
  lock_.unlock();
  m_condition.notify_one();
}

std::vector<std::string> DatabaseManager::Request(const std::string &request) {
  auto currentConnection = connection();

  std::vector<std::string> requestResult;

  PQsendQuery(currentConnection->connection().get(), request.c_str());

  while (auto res = PQgetResult(currentConnection->connection().get())) {
    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res)) {
      int fieldCount = PQntuples(res);
      for (int i = 0; i <= fieldCount; ++i) {
        requestResult.emplace_back(PQgetvalue(res, 0, i));
      }
    }

    if (PQresultStatus(res) == PGRES_FATAL_ERROR) {
      std::cout << PQresultErrorMessage(res) << std::endl;
    }

    PQclear(res);
  }

  freeConnection(currentConnection);

  return requestResult;
}