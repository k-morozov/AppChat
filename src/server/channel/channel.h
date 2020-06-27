#ifndef CHANNEL_H
#define CHANNEL_H

#include <unordered_map>
#include <deque>

#include <server/channel/iroom.h>


class Channel : public IRoom
{
public:
    Channel(identifier_t room = 0) : channel_id(room)
    {
        std::cout << "Create channel_id=" << channel_id << std::endl;
    }

    virtual void join(subscriber_ptr) override;
    virtual void leave(subscriber_ptr) override;
    virtual void notification(const Message&) override;
    virtual void save_to_db(const Message&) override;

    virtual identifier_t get_room_id() const override { return channel_id; }

private:
    std::unordered_map<int32_t, subscriber_ptr> subscribers;
    std::deque<Message> history;
    static std::string create_table_query;

    const identifier_t channel_id;
};

using channel_ptr = std::shared_ptr<Channel>;
#endif // CHANNEL_H
