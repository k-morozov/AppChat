#include "connection_manager.h"


connection_ptr ConnectionManager::get_connection(boost::asio::ip::tcp::socket&& _socket) {

    auto it = std::find_if(pool_connections.begin(), pool_connections.end(), [](const connection_ptr& ptr) {
       return !ptr->is_busy();
    });
    if (it!=pool_connections.end()) {
        (*it)->set_busy();
        (*it)->init(std::move(_socket));

        LOG4CPLUS_INFO(logger,
                       "use old connection, current size pool = " << pool_connections.size());
        print_pool();
        return (*it);
    }
    else {
        auto block = std::make_shared<Connection>(std::move(_socket), db);
        pool_connections.push_back(block);

        LOG4CPLUS_INFO(logger,
                       "create new connection, current size pool = " << pool_connections.size());
        print_pool();
        return block;
    }

}

void ConnectionManager::print_pool() const noexcept {
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

