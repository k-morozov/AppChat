#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

class Message {
public:
    static constexpr std::size_t header_size    = 4;
    static constexpr std::size_t login_id_size  = 4;
    static constexpr std::size_t login_str_size = 32;
    static constexpr std::size_t max_body_size  = 512;

    static constexpr std::size_t General_zone   = header_size + login_id_size + login_str_size;
    static constexpr std::size_t Settings_zone  = login_id_size + login_str_size;

    Message() : body_length(0) {
         std::memset(data, 0, header_size + login_id_size + login_str_size);
    };
    Message(std::string mes)
    {
        std::memset(data, 0, header_size + login_id_size + login_str_size);
        set_body_lenght(mes.length());
        encode_header();

        std::strncpy(get_body(), mes.data(), body_length);
        get_body()[body_length] = '\0';
    };

    char* get_data()                    { return data; }
    char* get_id_body()                 { return data + header_size; }
    char* get_str_login()               { return data + header_size + login_id_size; }
    char* get_body()                    { return data + header_size + login_id_size + login_str_size; }

    const char* get_data() const        { return data; }
    const char* get_id_body() const     { return data + header_size; }
    const char* get_str_login() const   { return data + header_size + login_id_size; }
    const char* get_body() const        { return data + header_size + login_id_size + login_str_size; }

    std::size_t get_mes_length() const { return General_zone + body_length; }
    std::size_t get_body_length() const { return body_length; }
    void set_body_lenght(std::size_t len) {
        body_length = len;
        if (len>max_body_size) {
            body_length = 0;
        }
    }

    void set_id(int32_t id) { std::memcpy(data+header_size, &id, 4); }
    int32_t get_id() const { return *reinterpret_cast<const int32_t*>(data+header_size); }

    void set_login(std::string from) {
        std::size_t len = std::min(from.length(), login_str_size);
        std::strncpy(data+header_size+login_id_size, from.data(), len);
        *(data+header_size+login_id_size+len) = '\0';
    }


    void encode_header() {
        char header[header_size+1] = "";
        std::sprintf(header, "%4d", static_cast<int>(body_length));
        std::memcpy(data, header, header_size);
    }

    bool decode_header() {
        char header[header_size+1] = "";
        std::strncat(header, data, header_size);
        body_length = atoi(header);
        if (body_length>max_body_size) {
            body_length = 0;
            return false;
        }
        return true;
    }

private:
    char data[header_size + login_id_size + login_str_size + max_body_size];
    std::size_t body_length;
};

#endif // PROTOCOL_H
