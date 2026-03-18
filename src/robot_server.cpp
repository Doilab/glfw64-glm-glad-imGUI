#include <ixwebsocket/IXWebSocketServer.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <set>

//std::set<std::shared_ptr<ix::WebSocket>> clients;
//std::mutex clientsMutex;

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

    json to_json() const
    {
        return json{
            {"joint", joint}
        };
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

    // ===== 共有 =====
    RobotState latestState;
    std::mutex stateMutex;

    std::set<std::shared_ptr<ix::WebSocket>> clients;
    std::mutex clientsMutex;

    latestState.joint = {0,0,0,0,0,0, 0,0,0,0,0,0}; // 12関節分

    // ===== 接続管理＋受信 =====
    server.setOnConnectionCallback(
    [&](std::weak_ptr<ix::WebSocket> webSocketWeak,
        std::shared_ptr<ix::ConnectionState> connectionState)
    {
        auto webSocket = webSocketWeak.lock();
        if (!webSocket) return;

        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.insert(webSocket);
        }

        std::cout << "Client connected\n";

        webSocket->setOnMessageCallback(
        [&](const ix::WebSocketMessagePtr& msg)
        {
            if(msg->type == ix::WebSocketMessageType::Message)
            {
                try
                {
                    json j = json::parse(msg->str);
                    RobotState rs = RobotState::from_json(j);

                    {
                        std::lock_guard<std::mutex> lock(stateMutex);
                        latestState = rs;
                    }

                    std::cout << "Received JSON\n";
                    rs.print();
                }
                catch(std::exception& e)
                {
                    std::cout << "JSON parse error: " << e.what() << std::endl;
                }
            }
            else if(msg->type == ix::WebSocketMessageType::Close)
            {
                std::lock_guard<std::mutex> lock(clientsMutex);
                clients.erase(webSocket);

                std::cout << "Client disconnected\n";
            }
        });
    });

    // ===== 起動 =====
    auto res = server.listen();
    if(!res.first)
    {
        std::cerr << "Listen error\n";
        return 1;
    }

    server.start();

    std::cout << "WebSocket server running at ws://localhost:9002\n";

    // ===== 送信スレッド =====
    std::thread sender([&]()
    {
        while(true)
        {
            RobotState copy;

            {
                std::lock_guard<std::mutex> lock(stateMutex);
                copy = latestState;
            }

            // テスト用に動かす
            for(auto& j : copy.joint)
            {
                j += 0.1f;
                ;
            }

            //copy.joint[4] = 0.5f; // J4は固定debug

            std::string msg = copy.to_json().dump();

            {
                std::lock_guard<std::mutex> lock(clientsMutex);

                for(auto& ws : clients)
                {
                    ws->send(msg);
                    std::cout << "SEND: [" << msg << "]\n";//debug
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    sender.join();

    return 0;
}