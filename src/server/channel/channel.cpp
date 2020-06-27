#include <server/channel/channel.h>
#include <sstream>
#include <sqlite3.h>

std::string Channel::create_table_query = std::string("create table if not exists history ")
								     + std::string("(login varchar[") + std::to_string(Message::login_str_size) + std::string("], ")
								     + std::string("login_id integer, ")
								     + std::string("room_id integer, ")
								     + std::string("message_body varchar[") + std::to_string(Message::max_body_size) + std::string("]);");


void Channel::join(subscriber_ptr new_subsciber) {

    subscribers.try_emplace(new_subsciber->get_client_id(), new_subsciber);

    std::string str("joined to room_id=" + std::to_string(channel_id));
    Message notify(str);
    notify.set_login(new_subsciber->get_login());
    notify.set_login_id(new_subsciber->get_client_id());
    notification(notify);
    std::cout << new_subsciber->get_login() << " " << str << std::endl;


    for(const auto& message:history) {
        new_subsciber->send(message);
    }
}

void Channel::leave(subscriber_ptr subsciber) {
    subscribers.erase(subsciber->get_client_id());
    Message mes(subsciber->get_login() + " leave from Chat_room");
    mes.set_login("server");

    notification(mes);
}

void Channel::notification(const Message& message) {
    if(message.get_body_length()) {
        history.push_back(message);
        for(auto [id, sub]:subscribers) {
            if (message.get_login_id() != sub->get_client_id()) {
                sub->send(message);
            }
        }
    }
}

void Channel::save_to_db(const Message& message) {
	const std::string file_name = "history.db";
	sqlite3* db;
	int rc = sqlite3_open(file_name.c_str(), &db);
	if(rc) {
		std::cerr << "Cannot open database " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	char* err_msg1 = 0;
	rc = sqlite3_exec(db, create_table_query.c_str(),
						 [](void*, int, char**, char**){ return 0;},
						 0, &err_msg1);
	if(rc != SQLITE_OK) {
		std::cerr << "SQL error " << err_msg1 << std::endl;
		sqlite3_free(err_msg1);
		sqlite3_close(db);
		return;
	}

	const std::string insert_query = std::string("insert into history values(")
									  + std::string(message.get_buf_str_login()) + std::string(", ")
									  + std::to_string(message.get_login_id()) + std::string(", ")
									  + std::to_string(message.get_room_id()) + std::string(", ")
									  + std::string(message.get_buf_body()) + std::string(")");

	char* err_msg2 = 0;
	rc = sqlite3_exec(db, create_table_query.c_str(),
						 [](void*, int, char**, char**){ return 0;},
						 0, &err_msg2);
	if(rc != SQLITE_OK) {
		std::cerr << "SQL error " << err_msg2 << std::endl;
		sqlite3_free(err_msg2);
		sqlite3_close(db);
		return;
	}
	sqlite3_close(db);
}
