#ifndef CHANNEL_H
#define CHANNEL_H

#include <unordered_map>
#include <mutex>
#include <deque>
#include "iroom.h"
#include "storage/sqlitedatabase.h"
#include "log/logger.h"

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
    Channel(identifier_t room, Storage::database_ptr db) : channel_id(room)
    {
        if (db == nullptr) {
            BOOST_LOG_TRIVIAL(info) << "Failed to load history. Database pointer is nullptr.";
        } else {
            // @todo add check history
            history_room = db->get_history(channel_id);
            BOOST_LOG_TRIVIAL(info) << "Create channel_id=" << channel_id;
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
    void leave(identifier_t client_id) override ;
    /**
     * @brief Notify all channel subscribers about new message
     * @param response 
     */
    virtual void notification(TextSendData data) override;


    /**
     * @brief Get the room id object
     * 
     * @return identifier_t 
     */
    virtual identifier_t get_room_id() const override { return channel_id; }

    ~Channel() {
        for(auto it:subscribers) {
            leave(it.second);
        }
    }
private:
    std::mutex mutex_subs;
    std::unordered_map<identifier_t, subscriber_ptr> subscribers;


    const identifier_t channel_id;

    std::deque<TextSendData> history_room;
};

using channel_ptr = std::shared_ptr<Channel>;

#endif // CHANNEL_H
