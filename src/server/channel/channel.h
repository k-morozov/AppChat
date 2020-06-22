#ifndef CHANNEL_H
#define CHANNEL_H

#include <unordered_map>
#include <deque>

#include <server/channel/iroom.h>


class Channel : public IRoom
{
public:
    Channel() = default;

    virtual void join(subscriber_ptr) override;
    virtual void leave(subscriber_ptr) override;
    virtual void notification(const Message&) override;
private:
    std::unordered_map<int32_t, subscriber_ptr> subscribers;
    std::deque<Message> history;
};

using channel_ptr = std::shared_ptr<Channel>;
#endif // CHANNEL_H
