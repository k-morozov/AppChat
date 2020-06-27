#ifndef CHANNEL_H
#define CHANNEL_H

#include <unordered_map>
#include <mutex>
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
    virtual void notification(const std::string&, const std::string&) override;
    virtual void notification(transport_response_ptr response) override;

    virtual identifier_t get_room_id() const override { return channel_id; }

private:
    std::mutex mutex_subs;
    std::unordered_map<int32_t, subscriber_ptr> subscribers;
    const identifier_t channel_id;
};

using channel_ptr = std::shared_ptr<Channel>;
#endif // CHANNEL_H
