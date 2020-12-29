#include "DataBase.h"
#include "iostream"
#include "Sha1.h"
#include "boost/algorithm/string.hpp"
DatabaseConnection::DatabaseConnection(){
    m_connection.reset(PQsetdbLogin(m_dbhost.c_str(),
                                    std::to_string(m_dbport).c_str(),
                                    nullptr,
                                    nullptr,
                                    m_dbname.c_str(),
                                    m_dbuser.c_str(),
                                    m_dbpass.c_str()), &PQfinish);

    if (PQstatus(m_connection.get()) != CONNECTION_OK && PQsetnonblocking(m_connection.get(), 1) != 0) {
        throw std::runtime_error(PQerrorMessage(m_connection.get()));
    }

}

std::shared_ptr<PGconn> DatabaseConnection::connection() const {
    return m_connection;
}


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
            for (int i = 0; i < fieldCount; ++i) {
                if(PQgetvalue(res, 0, i) != NULL) {
                    requestResult.emplace_back(PQgetvalue(res, 0, i));
                }
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

std::shared_ptr<RegistrationManager> RegistrationManager::GetRegistrationManagerSingleton() {
    static std::shared_ptr<RegistrationManager> registrationManager(new RegistrationManager);
    return registrationManager;
}

const size_t maxCommandLen = 400;
uint64_t
RegistrationManager::RegistrationDriver(const std::string& name, const std::string& carMark, const std::string& login,
                                        const std::string& password,
                                        const MailTaxi::Driver &driver) {
    char buffer[maxCommandLen];
    sprintf(buffer, "INSERT INTO public.\"driver\" (id, name, carmark, login, password, children, animal, business) "
                    "VALUES (DEFAULT, '%s', '%s', '%s', '%s', '%d', '%d', '%d');", name.c_str(),
                    carMark.c_str(), login.c_str(), std::string(sha1(password)).c_str(), driver.children() ,
                    driver.animals(), driver.type());
    this->databaseManager.Request(buffer);

}

void RegistrationManager::RegistrationClient(const std::string& name, const std::string& login,
                                             const std::string& password) {
    char buffer[maxCommandLen];
    sprintf(buffer, "INSERT INTO public.\"client\" (id, name, login, password) "
                    "VALUES (DEFAULT, '%s', '%s', '%s');", name.c_str(),
                    login.c_str(), std::string(sha1(password)).c_str());
    this->databaseManager.Request(buffer);

}

uint64_t RegistrationManager::DriverLogin(const std::string &login, const std::string &password) {
    char buffer[maxCommandLen];
    sprintf(buffer, "SELECT (id, password) FROM public. \"driver\" WHERE (login) IN ('%s')", login.c_str());
    auto dbresp = this->databaseManager.Request(buffer)[0];
    dbresp.erase(dbresp.begin());
    dbresp.erase(--dbresp.end());
    std::vector<std::string> result;
    boost::algorithm::iter_split(result, dbresp, boost::algorithm::first_finder(","));
    if (!result.empty()){
        if (result[1] == std::string(sha1(password)))
            return atoi(result[0].c_str());
        else
            return 0;
    }
    else
        return 0;
}

uint64_t RegistrationManager::ClientLogin(const std::string &login, const std::string &password) {
    char buffer[maxCommandLen];
    sprintf(buffer, "SELECT (id, password) FROM public. \"client\" WHERE (login) IN ('%s')", login.c_str());
    auto dbresp =  this->databaseManager.Request(buffer)[0];
    dbresp.erase(dbresp.begin());
    dbresp.erase(--dbresp.end());
    std::vector<std::string> result;
    boost::algorithm::iter_split(result, dbresp, boost::algorithm::first_finder(","));
    if (!result.empty()){
        if (result[1] == std::string(sha1(password)))
            return atoi(result[0].c_str());
        else
            return 0;
    }
    else
        return 0;
}

void RegistrationManager::GetDriverInformationForClientResponse(uint64_t driverId, MailTaxi::MakeOrderResponse &response) {
    char buffer[maxCommandLen];
    sprintf(buffer, "SELECT (name, carmark) FROM public. \"driver\" WHERE (id) IN ('%lu')", driverId);
    auto dbresp =  this->databaseManager.Request(buffer)[0];
    dbresp.erase(dbresp.begin());
    dbresp.erase(--dbresp.end());
    std::vector<std::string> result;
    boost::algorithm::iter_split(result, dbresp, boost::algorithm::first_finder(","));
    response.set_drivername(result[0]);
    response.set_drivercarmark(result[1]);
}

void RegistrationManager::GetDriverInformation(uint64_t driverId, MailTaxi::Driver &driver) {
    char buffer[maxCommandLen];
    sprintf(buffer, "SELECT (children, animal, business) FROM public. \"driver\" WHERE (id) IN ('%lu')", driverId);
    auto result = this->databaseManager.Request(buffer);
    size_t count = 0;
    for (size_t i = 0; i < result[0].length(); i++){
        auto code = result[0][i];
        if (code == 't' || code == 'f'){
            bool indicator = false;
            if (code == 't')
                indicator = true;
            if (count == 0)
                driver.set_children(indicator);
            else if (count == 1)
                driver.set_animals(indicator);
            else if (count == 2)
                driver.set_type(static_cast<MailTaxi::OrderType>(indicator));
            count++;
        }
    }
}
