#include "connection_details.hpp"
#include "TCP/Client.hpp"
#include <iostream>
#include <vector>
#include <exception>



int main()
{
    try
    {
        std::vector<Networking::TCP::Client> clients(clientsCount);
        std::string message;
        
        for (Networking::TCP::Client& client : clients) 
        { 
            client = std::move(Networking::TCP::Client(serverAddress, serverPort));
        }

        for (Networking::TCP::Client& client : clients)
        { 
            client.doConnection();
        }

        for (std::uint32_t i = 0; i < clients.size(); ++i)
        {
            clients.at(i).sendMessage("Hello from client #" + std::to_string(i + 1), packetSize);

            message = std::move( clients.at(i).getMessage(packetSize) );
            if (message.empty() == false) { std::cout << message << std::endl; }
        }


        for (std::uint32_t i = 0; i < (clients.size() - 1); ++i) 
        { 
            clients.at(i).disconnect();
        }
        
        Networking::TCP::Client& lastClient = clients.at(clients.size() - 1);

        std::cout << std::endl;
        for (std::uint32_t i = 0; i < 3; ++i)
        {
            lastClient.sendMessage("After closing connection", packetSize);

            message = std::move( lastClient.getMessage(packetSize) );
            if (message.empty() == false) { std::cout << message << std::endl; }
        }

        lastClient.disconnect();
    }

    catch (const std::exception& err)
    {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }
    

    return EXIT_SUCCESS;
}
