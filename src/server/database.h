#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include <algorithm>

class Database {
public:
    static Database& Instance() {
        static Database db;
        return db;
    }

    int get_new_id(std::string s) {
        id_name[current_id] = s;
        return current_id++;
    }

    std::string get_name(int32_t find_id) {
        auto it = id_name.find(find_id);
        if (it!=id_name.end()) {
            return it->second;
        }

        return "None";
    }
private:
    Database() = default;
    int32_t current_id = 1;
    std::unordered_map<int32_t, std::string> id_name;
};

#endif // DATABASE_H
