#include "connection_manager.h"


connection_ptr ConnectionManager::get_connection(boost::asio::ip::tcp::socket&& _socket) {

    auto it = std::find_if(pool_connections.begin(), pool_connections.end(), [](const connection_ptr& ptr) {
       return !ptr->is_busy();
    });
    if (it!=pool_connections.end()) {
        (*it)->set_busy();
        (*it)->init(std::move(_socket));

        BOOST_LOG_TRIVIAL(info) << "use old connection, current size pool = " << pool_connections.size();
        print_pool();
        return (*it);
    }
    else {
        auto block = std::make_shared<Connection>(std::move(_socket), db);
        pool_connections.push_back(block);

        BOOST_LOG_TRIVIAL(info) << "create new connection, current size pool = " << pool_connections.size();
        print_pool();
        return block;
    }

}

void ConnectionManager::print_pool() const noexcept {
    BOOST_LOG_TRIVIAL(info) << "[connections]";
    for(const auto& obj:pool_connections) {
        if (obj->get_client_id()>0) {
            BOOST_LOG_TRIVIAL(info) << "obj: login=" << obj->get_login() << ", id=" << obj->get_client_id() << ", status: " <<
                                       (obj->is_busy() ? "busy" : "free");
        } else {
            BOOST_LOG_TRIVIAL(info) << "obj: current connection not init";
        }
    }
}

