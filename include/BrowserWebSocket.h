#ifdef __EMSCRIPTEN__

#include <queue>
#include <emscripten/websocket.h>
#include "IWebSocket.h"

class BrowserWebSocket : public IWebSocket
{
    EMSCRIPTEN_WEBSOCKET_T ws;

    std::queue<std::string> queue;

    bool connected=false;

public:

    void connect(const std::string& uri) override
    {
        EmscriptenWebSocketCreateAttributes attr;

        attr.url = uri.c_str();
        attr.protocols = nullptr;
        attr.createOnMainThread = true;

        ws = emscripten_websocket_new(&attr);

        emscripten_websocket_set_onmessage_callback(
            ws,
            this,
            [](int, const EmscriptenWebSocketMessageEvent* e, void* u)
            {
                auto self = (BrowserWebSocket*)u;

                std::string s(
                    (char*)e->data,
                    e->numBytes);

                self->queue.push(s);

                return EM_TRUE;
            });
    
        emscripten_websocket_set_onopen_callback(
            ws,
            this,
            [](int, const EmscriptenWebSocketOpenEvent*, void* u)
            {
                ((BrowserWebSocket*)u)->connected = true;
                return EM_TRUE;
            });
    }

    void send(const std::string& msg) override
    {
        emscripten_websocket_send_utf8_text(
            ws,
            msg.c_str());
    }

    void poll() override {}

    bool hasMessage() override
    {
        return !queue.empty();
    }

    std::string popMessage() override
    {
        auto m=queue.front();
        queue.pop();
        return m;
    }

    bool isConnected() const override
    {
        return connected;
    }

    void disconnect() override
    {
        emscripten_websocket_close(ws,1000,"");
    }
};

#endif