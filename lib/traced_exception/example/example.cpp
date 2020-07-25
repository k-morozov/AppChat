#include "traced_exception.h"

#include <iostream>
#include <exception>

void foo(int i)
{
    if (i >= 4)
        throw util::with_trace<std::out_of_range>("i must be less than 4");
    if (i == 0)
        throw util::with_trace<std::logic_error>("i must not be zero");
}

int main()
{
    try {
        foo(5);
    } catch (const util::traced& e) {
        std::cerr << e.what() << "\nStacktrace:\n" << e.trace << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
