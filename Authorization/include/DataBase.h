#pragma once

#include <libpq-fe.h>
#include <memory>
#include <mutex>
#include <string>
#include <queue>
#include <condition_variable>
#include <libpq-fe.h>
#include "Authorization.grpc.pb.h"
class DatabaseConnection {
public:
    DatabaseConnection();
    std::shared_ptr<PGconn> connection() const;

private:
    std::string m_dbhost = "localhost";
    int m_dbport = 5432;
    std::string m_dbname = "postgres";
    std::string m_dbuser = "postgres";
    std::string m_dbpass = "";

    std::shared_ptr<PGconn> m_connection;

};

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

class RegistrationManager{
private:
    DatabaseManager databaseManager;
    RegistrationManager() = default;
public:
    static std::shared_ptr<RegistrationManager> GetRegistrationManagerSingleton();
    uint64_t RegistrationDriver(const std::string& name, const std::string& carMark, const std::string& login, const std::string& password,
                                const MailTaxi::Driver& driver);
    void RegistrationClient(const std::string& name, const std::string& login, const std::string& password);
    uint64_t DriverLogin(const std::string& login, const std::string& password);
    uint64_t ClientLogin(const std::string& login, const std::string& password);
    void GetDriverInformationForClientResponse(uint64_t driverId, MailTaxi::MakeOrderResponse& response);
    void GetDriverInformation(uint64_t driverId, MailTaxi::Driver& driver);
};