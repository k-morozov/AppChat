#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "connection.h"


/**
 * @brief ConnectionManager
 *
 * @details Manager to control connections from clinets. Use ObjectPool.
 */
class ConnectionManager
{
public:
    ConnectionManager() = default;
    ConnectionManager(const ConnectionManager&) = delete;

    /**
     * @brief get new or old connection for new client
     * @param _socket
     * @param _db
     */
    connection_ptr get(boost::asio::ip::tcp::socket&& _socket, database_ptr _db) {

        auto it = std::find_if(pool_connections.begin(), pool_connections.end(), [](const connection_ptr& ptr) {
           return !ptr->is_busy();
        });
        if (it!=pool_connections.end()) {
            (*it)->set_busy();
            (*it)->init(std::move(_socket), _db);

            LOG4CPLUS_INFO(logger,
                           "use old connection, size pool = " << pool_connections.size());
            print_pool();
            return (*it);
        }
        else {
            auto block = std::make_shared<Connection>(std::move(_socket), _db);
            pool_connections.push_back(block);

            LOG4CPLUS_INFO(logger,
                           "create new connection, size pool = " << pool_connections.size());
            print_pool();
            return block;
        }

    }

    ~ConnectionManager() {
        for (auto& value_con: pool_connections) {
            value_con->set_busy(false);
        }
        pool_connections.clear();
    }
private:
    std::vector<connection_ptr> pool_connections;
    log4cplus::Logger logger = LOGGER("ConnectionManager");

    /**
     * @brief print all connections
     * */
    void print_pool() const noexcept {
        LOG4CPLUS_INFO(logger, "[connections]");
        for(const auto& obj:pool_connections) {
            if (obj->get_client_id()>0) {
                LOG4CPLUS_INFO(logger,
                           "obj: login=" << obj->get_login() << ", id=" << obj->get_client_id() << ", status: " <<
                            (obj->is_busy() ? "busy" : "free"));
            } else {
                LOG4CPLUS_INFO(logger,
                           "obj: current connection not init");
            }
        }
    }
};

#endif // CONNECTIONMANAGER_H
