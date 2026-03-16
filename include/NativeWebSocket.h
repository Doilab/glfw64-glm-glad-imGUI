#ifndef __EMSCRIPTEN__

#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_STL_
#include <asio.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <queue>
#include "IWebSocket.h"

class NativeWebSocket : public IWebSocket
{
    typedef websocketpp::client<websocketpp::config::asio_client> client;

    client c;
    websocketpp::connection_hdl hdl;

    std::queue<std::string> queue;

    bool connected = false;

public:

    NativeWebSocket()
    {
        c.init_asio();

        c.set_message_handler(
        [this](auto, auto msg)
        {
            queue.push(msg->get_payload());
        });

        c.set_open_handler(
        [this](auto handle)
        {
            hdl = handle;
            connected = true;
        });

        c.set_close_handler(
        [this](auto)
        {
            connected = false;
        });
    }

    void connect(const std::string& uri) override
    {
        websocketpp::lib::error_code ec;

        auto con = c.get_connection(uri, ec);
        c.connect(con);
    }

    void poll() override
    {
        c.poll();
    }

    void send(const std::string& msg) override
    {
        if(!connected) return;

        c.send(hdl, msg,
               websocketpp::frame::opcode::text);
    }

    bool hasMessage() override
    {
        return !queue.empty();
    }

    std::string popMessage() override
    {
        auto m = queue.front();
        queue.pop();
        return m;
    }

    bool isConnected() const override
    {
        return connected;
    }

    void disconnect() override
    {
        c.close(hdl,
        websocketpp::close::status::normal,
        "");
    }
};

#endif