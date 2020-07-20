#ifndef CHANNEL_H
#define CHANNEL_H

#include <unordered_map>
#include <mutex>
#include <deque>
#include <channel/iroom.h>
#include <database.h>
#include <logger.h>

/**
 * @brief Channel class
 * 
 * @details Implement IRoom interface
 * 
 * @todo override virtual destructor to avoid potential problems in further
 */
class Channel : public IRoom
{
public:
    /**
     * @brief Construct a new Channel
     * 
     * @param room
     *
     * @param db
     */
    Channel(identifier_t room, database_ptr db) : channel_id(room)
    {
        if (db == nullptr) {
            LOG4CPLUS_ERROR(logger, "Failed to load history. Database pointer is nullptr.");
        } else {
            history_room = db->load_history(channel_id);
            LOG4CPLUS_INFO(logger, "Create channel_id=" << channel_id);
        }
    }

    /**
     * @brief Join a user to channel
     * 
     * @param subscriber sbscriber who is joining this channel
     */
    virtual void join(subscriber_ptr subscriber) override;
    
    /**
     * @brief Leave a user from channel
     * 
     * @param subscriber sbscriber who is leaving this channel
     */
    virtual void leave(subscriber_ptr subscriber) override;
    
    /**
     * @brief Notify all channel subscribers about new message
     * @param response 
     */
    virtual void notification(text_response_ptr response) override;


    /**
     * @brief Get the room id object
     * 
     * @return identifier_t 
     */
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
