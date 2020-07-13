#ifndef CHANNEL_H
#define CHANNEL_H

#include <unordered_map>
#include <mutex>
#include <deque>
#include <channel/iroom.h>
#include <database.h>
#include <logger.h>

class Channel : public IRoom
{
public:
    Channel(identifier_t room = 0) : channel_id(room)
    {
        history_room = Database::Instance().load_history(channel_id);
        auto logger = LOGGER("Channel");
        LOG4CPLUS_INFO(logger, "Create channel_id=" << channel_id);
    }

    virtual void join(subscriber_ptr) override;
    virtual void leave(subscriber_ptr) override;
    virtual void notification(text_response_ptr response) override;


    virtual identifier_t get_room_id() const override { return channel_id; }

private:
    std::mutex mutex_subs;
    std::unordered_map<identifier_t, subscriber_ptr> subscribers;


    const identifier_t channel_id;

    std::deque<text_response_ptr> history_room;
};

using channel_ptr = std::shared_ptr<Channel>;
#endif // CHANNEL_H
