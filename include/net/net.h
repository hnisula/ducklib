#ifndef NET_H
#define NET_H

#include <string>

namespace ducklib::net
{
void net_initialize();
void net_shutdown();

template <typename... Args>
void net_log_error(const char* text, Args... args);
template <typename... Args>
void net_fail(const std::string& text, Args... args);
}

#endif // NET_H