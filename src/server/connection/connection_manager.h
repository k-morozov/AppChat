#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "connection.h"

/**
 * @brief ConnectionManager
 * @details Manager to control connections from clinets. Use ObjectPool.
 */
class ConnectionManager
{
public:
    ConnectionManager(std::shared_ptr<boost::asio::thread_pool> a_thread_pool, database_ptr n_db):
        thread_pool(a_thread_pool),
        db(n_db)
    {
        BOOST_LOG_TRIVIAL(info) << "create ConnectionManager";
    }
    ConnectionManager(const ConnectionManager&) = delete;

    /**
     * @brief get new or old connection for new client
     * @param _socket
     */
    connection_ptr get_connection(boost::asio::ip::tcp::socket&& _socket);

    void close_all() {
        for (auto& value_con: pool_connections) {
            value_con->set_busy(false);
        }
        pool_connections.clear();
    }

    ~ConnectionManager() {
        close_all();
    }
private:
    std::shared_ptr<boost::asio::thread_pool> thread_pool;
    std::vector<connection_ptr> pool_connections;
    database_ptr db;

    /**
     * @brief print all connections
     * */
    void print_pool() const noexcept;
};

#endif // CONNECTIONMANAGER_H
