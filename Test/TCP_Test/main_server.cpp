#include "connection_details.hpp"
#include "TCP/Server.hpp"
#include "Socket.hpp"
#include <exception>



int main()
{
    try
    {
        std::unique_ptr<Networking::TCP::Server> server = std::make_unique<Networking::TCP::Server>(clientsCount, serverPort, serverAddress);

        server.get()->run();
        while (server.get()->running() == false) { }
        std::cout << "Server running..." << std::endl << std::endl;

        
        while (server.get()->running() == true)
        {
            Networking::Socket clientSocket = server.get()->acceptConnection();

            server.get()->runConnection(clientSocket, [clientSocket, &server](void)
            {
            	std::string message;
                while (server.get()->isClosedConnection(clientSocket) == false)
                {
                    message = std::move( server.get()->getMessage(clientSocket, packetSize) );
                    if (message.empty() == false) { std::cout << message << std::endl; }

                    server.get()->sendMessage(clientSocket, "Hello from server!", packetSize);
                }

                server.get()->stopConnection(clientSocket);
            });
        }

        server.get()->stop();
    }

    catch (const std::exception& err)
    {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }
    

    return EXIT_SUCCESS;
}
