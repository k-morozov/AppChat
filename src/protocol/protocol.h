#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstdio>

class Message {
public:
    static constexpr std::size_t header_size    = 4;
    static constexpr std::size_t login_id_size  = 4;
    static constexpr std::size_t login_str_size = 32;
    static constexpr std::size_t room_id        = 4;
    static constexpr std::size_t max_body_size  = 512;

    static constexpr std::size_t General_zone   = header_size + login_id_size + login_str_size + room_id;
    static constexpr std::size_t Settings_zone  =               login_id_size + login_str_size + room_id;

    Message() : body_length(0)
    {
        set_room_id(0);
    };

    Message(std::string mes)
    {
        set_body_lenght(mes.length());
        encode_header();
        set_room_id(0);
        std::snprintf(get_buf_body(), body_length+1, "%s", mes.data());
    };

    void* get_buf_data()        { return data; }
    void* get_buf_id_login()    { return data + header_size; }
    char* get_buf_str_login()   { return data + header_size + login_id_size; }
    void* get_buf_id_room()     { return data + header_size + login_id_size + login_str_size; }
    char* get_buf_body()        { return data + General_zone ; }

    const void* get_buf_data()      const { return data; }
    const void* get_buf_id_login()  const { return data + header_size; }
    const char* get_buf_str_login() const { return data + header_size + login_id_size; }
    const void* get_buf_id_room()   const { return data + header_size + login_id_size + login_str_size; }
    const char* get_buf_body()      const { return data + General_zone ; }

    std::size_t get_mes_length()    const { return General_zone + body_length; }
    std::size_t get_body_length()   const { return body_length; }
    void set_body_lenght(std::size_t len) {
        body_length = len;
        if (len>max_body_size) {
            body_length = 0;
        }
    }

    void set_login_id(int32_t id)   { std::memcpy(data+header_size, &id, 4); }
    void set_room_id(int32_t id)    { std::memcpy(room_id_pos, &id, 4); }
    int32_t get_room_id()           const { return *reinterpret_cast<const int32_t*>(room_id_pos); }
    int32_t get_login_id()          const { return *reinterpret_cast<const int32_t*>(data+header_size); }

    void set_login(std::string from) {
        std::snprintf(data+header_size+login_id_size,
                 std::min(from.length(), login_str_size)+1, "%s", from.data());
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
    char data[General_zone + max_body_size];
    std::size_t body_length;

    void *room_id_pos = data + header_size + login_id_size + login_str_size;
};

//std::ostream& operator<<(std::ostream& os, const Message& m) {
//    os << m.get_buf_str_login() << ": " << m.get_buf_body() << std::endl;
//    return os;
//}

#endif // PROTOCOL_H
