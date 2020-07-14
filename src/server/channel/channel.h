#ifndef CHANNEL_H
#define CHANNEL_H

#include <unordered_map>
#include <mutex>
#include <deque>
#include <channel/iroom.h>
#include <database.h>

/**
 * @brief Channel class
 * 
 * @details Implement IRoom interface
 */
class Channel : public IRoom
{
public:

    /**
     * @brief Construct a new Channel
     * 
     * @param room 
     */
    Channel(identifier_t room = 0) : channel_id(room)
    {
        history_room = Database::Instance().load_history(channel_id);
        std::cout << "Create channel_id=" << channel_id << std::endl;
    }

    /**
     * @brief Join a user to channel
     */
    virtual void join(subscriber_ptr) override;
    
    /**
     * @brief Leave a user from channel
     */
    virtual void leave(subscriber_ptr) override;
    
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
};

using channel_ptr = std::shared_ptr<Channel>;
#endif // CHANNEL_H
