#ifndef DATABASELIBRARY__DATABASECONNECTION_H_
#define DATABASELIBRARY__DATABASECONNECTION_H_

#include <memory>
#include <mutex>
#include <libpq-fe.h>

class DatabaseConnection {
 public:
  DatabaseConnection();
  std::shared_ptr<PGconn> connection() const;

 private:
  std::string m_dbhost = "localhost";
  int m_dbport = 5432;
  std::string m_dbname = "MailTaxi";
  std::string m_dbuser = "nick_nak";
  std::string m_dbpass = "";

  std::shared_ptr<PGconn> m_connection;

};

#endif //DATABASELIBRARY__DATABASECONNECTION_H_
