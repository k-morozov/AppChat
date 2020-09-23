#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "command_table.h"
#include "msgfactory.h"
#include "datetime.h"


struct ClientTextMsg {
    std::string author;
    std::string text;
    identifier_t channel_id;
    DateTime dt;
};

#endif // PROTOCOL_H
