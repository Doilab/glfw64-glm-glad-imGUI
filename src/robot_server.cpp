#include <ixwebsocket/IXWebSocketServer.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <vector>

using json = nlohmann::json;

struct RobotState
{
    std::vector<float> joint;

    static RobotState from_json(const json& j)
    {
        RobotState rs;

        if(j.contains("joint"))
        {
            rs.joint = j["joint"].get<std::vector<float>>();
        }

        return rs;
    }

    void print() const
    {
        std::cout << "RobotState joints:\n";

        for(size_t i=0;i<joint.size();i++)
        {
            std::cout << "J" << i+1 << " = " << joint[i] << "\n";
        }

        std::cout << std::endl;
    }
};

int main()
{
    int port = 9002;

    ix::WebSocketServer server(port);

    server.setOnClientMessageCallback(
    [](std::shared_ptr<ix::ConnectionState> connection,
       ix::WebSocket& webSocket,
       const ix::WebSocketMessagePtr& msg)
    {
        if(msg->type == ix::WebSocketMessageType::Message)
        {
            try
            {
                json j = json::parse(msg->str);

                RobotState rs = RobotState::from_json(j);

                std::cout << "Received JSON\n";
                rs.print();
            }
            catch(std::exception& e)
            {
                std::cout << "JSON parse error: " << e.what() << std::endl;
            }
        }
    });

    auto res = server.listen();
    if(!res.first)
    {
        std::cerr << "Listen error\n";
        return 1;
    }

    server.start();

    std::cout << "WebSocket server running at ws://localhost:9002\n";

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}