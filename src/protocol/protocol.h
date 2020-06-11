#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdio>
#include <cstdlib>
#include <cstring>

class Messages {
public:
    enum { header_size = 4};
    enum { max_body_size = 512};

    Messages() : body_length(0) {};
    Messages(char *mes)
    {
            set_body_lenght(std::strlen(mes));
            std::memcpy(get_body(), mes, body_length);
            encode_header();
    };

    const char* get_data() const { return data; }
    char * get_data() { return data; }

    const char* get_body() const { return data+header_size; }
    char * get_body() { return data+header_size; }

    std::size_t get_mes_length() const { return header_size + body_length; }
    std::size_t get_body_length() const { return body_length; }
    void set_body_lenght(std::size_t len) {
        body_length = len;
        if (len>max_body_size) {
            body_length = 0;
        }
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
    char data[header_size + max_body_size];
    std::size_t body_length;
};

#endif // PROTOCOL_H
