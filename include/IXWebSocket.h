#ifndef __EMSCRIPTEN__

#pragma once

#include <ixwebsocket/IXWebSocket.h>
#include <queue>
#include <string>
#include <mutex>

#include "IWebSocket.h"

class NativeWebSocket : public IWebSocket
{
    ix::WebSocket ws;

    std::queue<std::string> queue;
    std::mutex mutex;

    bool connected = false;

public:

    void connect(const std::string& uri) override
    {
        ws.setUrl(uri);

        ws.setOnMessageCallback(
        [this](const ix::WebSocketMessagePtr& msg)
        {
            if (msg->type == ix::WebSocketMessageType::Open)
            {
                connected = true;
            }
            else if (msg->type == ix::WebSocketMessageType::Close)
            {
                connected = false;
            }
            else if (msg->type == ix::WebSocketMessageType::Message)
            {
                std::lock_guard<std::mutex> lock(mutex);
                queue.push(msg->str);
            }
        });

        ws.start();   // 内部スレッド開始
    }

    void disconnect() override
    {
        ws.stop();
        connected = false;
    }

    void send(const std::string& msg) override
    {
        ws.send(msg);
    }

    bool isConnected() const override
    {
        return connected;
    }

    bool hasMessage() override
    {
        std::lock_guard<std::mutex> lock(mutex);
        return !queue.empty();
    }

    std::string popMessage() override
    {
        std::lock_guard<std::mutex> lock(mutex);

        auto m = queue.front();
        queue.pop();

        return m;
    }

    void poll() override
    {
        // IXWebSocketは内部スレッドなので不要
    }
};

#endif