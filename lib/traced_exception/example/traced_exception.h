#ifndef TRACED_H
#define TRACED_H

#include <boost/stacktrace.hpp>
#include <utility> // std::forward

namespace util
{

struct traced
{
    virtual ~traced() {}
    virtual const char* what() const noexcept = 0;

    const boost::stacktrace::stacktrace trace;
};

template <class Exception>
struct with_trace : public Exception, public traced
{
    template <class... Args>
    with_trace(Args&&... args)
        : Exception(std::forward<Args>(args)...)
    { }

    virtual const char* what() const noexcept override { return Exception::what(); }
};

} // namespace util
#endif // TRACED_H
