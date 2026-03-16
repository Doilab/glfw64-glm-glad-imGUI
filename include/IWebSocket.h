#pragma once
#include <string>
#include <queue>

class IWebSocket
{
public:

    virtual void connect(const std::string& uri) = 0;
    virtual void disconnect() = 0;

    virtual void send(const std::string& msg) = 0;

    virtual bool isConnected() const = 0;

    virtual bool hasMessage() = 0;
    virtual std::string popMessage() = 0;

    virtual void poll() = 0;

    virtual ~IWebSocket(){}
};