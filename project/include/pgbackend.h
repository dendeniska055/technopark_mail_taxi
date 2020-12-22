#ifndef UNTITLED2__PGBACKEND_H_
#define UNTITLED2__PGBACKEND_H_

#include <memory>
#include <mutex>
#include <string>
#include <queue>
#include <condition_variable>
#include <libpq-fe.h>
#include "pgconnection.h"

class PGBackend {
 public:
  PGBackend();
  std::shared_ptr<PGConnection> connection();
  void freeConnection(std::shared_ptr<PGConnection>);

 private:
  void createPool();

  std::mutex m_mutex;
  std::condition_variable m_condition;
  std::queue<std::shared_ptr<PGConnection>> m_pool;

  const int POOL = 10;
};

#endif //UNTITLED2__PGBACKEND_H_
