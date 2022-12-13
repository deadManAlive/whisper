#pragma once

const char* ip2cstr(const IPAddress& ip) {
    return ip.toString().c_str();
}