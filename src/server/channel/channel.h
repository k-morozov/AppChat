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
    Channel(identifier_t room, database_ptr db) : channel_id(room)
    {
        if (db == nullptr) {
            LOG4CPLUS_ERROR(logger, "Failed to load history. Database pointer is nullptr.");
        } else {
            history_room = db->load_history(channel_id);
            LOG4CPLUS_INFO(logger, "Create channel_id=" << channel_id);
        }
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
    log4cplus::Logger logger = LOGGER("Channel");
};

using channel_ptr = std::shared_ptr<Channel>;
#endif // CHANNEL_H
