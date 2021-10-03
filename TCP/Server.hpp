#ifndef NETWORKING_TCP_SERVER_HPP
#define NETWORKING_TCP_SERVER_HPP

#include "../ThreadPool.hpp"
#include "../Port.hpp"
#include "../Socket.hpp"
#include "tcp_error.hpp"

#include <memory>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>



namespace Networking
{
    namespace TCP
    {
        class Server
        {
            public:
                Server() = default;
                Server(const std::uint16_t& maxConnections, const Port& port, const std::string& ip_address = "");
                Server(const Server& server) = delete;
                Server(Server&& server) = delete;
                ~Server() = default;

                Server& operator=(const Server& server) = delete;
                Server& operator=(Server&& server) = delete;

                void run();
                void stop();
                Socket acceptConnection();
                template<typename Function> void runConnection(const Socket& socket, Function&& function);
                void stopConnection(const Socket& socket);
                bool isClosedConnection(const Socket& socket);
                void sendMessage(const Socket& socket, const std::string& message, const std::size_t& size);
                std::string getMessage(const Socket& socket, const std::size_t& size);

                bool running() const;
                std::uint16_t getMaxConnections() const;

                Socket getSocket() const;
                std::string getIPAddress() const;
                Port getPort() const;

            
            private:
                bool getFreeConnectionID(std::uint16_t& result);
                bool getConnectionIDBySocket(const Socket& socket, std::uint16_t& result);
                void errorCheck(int errorValue, const char* errorMessage);


            private:
                struct ServerParams
                {
                    sockaddr_in server;
                    Socket socket;
                };


                bool isRunning = false;
                std::uint16_t maxConnections = 0;

                std::unique_ptr<ServerParams> serverParams;
                std::unique_ptr<ThreadPool> connections;
                std::vector<std::pair<std::uint16_t, Socket>> connectionsID;  // connectionsID::first = ID, connectionsID::second = socket
        };


        Server::Server(const std::uint16_t& maxConnections, const Port& port, const std::string& ip_address)
        {
            serverParams = std::move(std::make_unique<ServerParams>());
            memset(&serverParams.get()->server, 0, sizeof(sockaddr_in));

            serverParams.get()->server.sin_family = AF_INET;
            serverParams.get()->server.sin_port = htons(port.get());

            if (ip_address.empty() == false) { serverParams.get()->server.sin_addr.s_addr = inet_addr(ip_address.data()); }
            else { serverParams.get()->server.sin_addr.s_addr = htonl(INADDR_ANY); }

            this->maxConnections = maxConnections;
        }


        void Server::errorCheck(int errorValue, const char* errorMessage)
        {
            if (errorValue == -1) {
                throw TCP::tcp_error(errorMessage);
            }
        }


        bool Server::getConnectionIDBySocket(const Socket& socket, std::uint16_t& result)
        {
            for (std::pair<std::uint16_t, Socket>& fd : connectionsID)
            {
                if (socket.get() == fd.second.get())
                {
                    result = fd.first;
                    return true;
                }
            }

            return false;
        }


        bool Server::getFreeConnectionID(std::uint16_t& result)
        {
            for (std::pair<std::uint16_t, Socket>& connectionID : connectionsID)
            {
                if (connectionID.second.get() == Socket::SOCKET_NONE) { return true; }
                else { result += 1; }
            }

            result = 0;
            return false;
        }


        void Server::run()
        {
            if (serverParams.get() != nullptr)
            {
                const int optionValue = 1;

                errorCheck(serverParams.get()->socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP), "Server error (Socket creating failed)");
                errorCheck(setsockopt(serverParams.get()->socket.get(), SOL_SOCKET, SO_REUSEPORT, &optionValue, sizeof(optionValue)), "Server error (Socket options setting failed)");
                errorCheck(bind(serverParams.get()->socket.get(), (sockaddr*) &serverParams.get()->server, sizeof(sockaddr_in)), "Server error (Binding socket to a port failed)");
                errorCheck(listen(serverParams.get()->socket.get(), maxConnections), "Server error (Listening to socket failed)");

                connectionsID.resize(maxConnections);
                for (std::pair<std::uint16_t, Socket>& connectionID : connectionsID) { connectionID = std::move(std::pair<std::uint16_t, Socket>(0, Socket::SOCKET_NONE)); }

                connections = std::move( std::make_unique<ThreadPool>(maxConnections) );
                while (connections.get()->running() == false) { }

                isRunning = true;
            }
        }


        void Server::stop()
        {
            if (isRunning == true)
            {
                isRunning = false;

                for (std::pair<std::uint16_t, Socket>& client : connectionsID) 
                {
                    if (client.second != Socket::SOCKET_NONE) {
                        errorCheck(close(client.second.get()), "Server error (Closing client socket failed)");
                    }
                }

                errorCheck(close(serverParams.get()->socket.get()), "Server error (Closing socket failed)");

                serverParams.reset();
                connectionsID.clear();
                connections.reset();
            }
        }


        Socket Server::acceptConnection()
        {
            if (isRunning == true)
            {
                std::unique_ptr<sockaddr_in> client = std::make_unique<sockaddr_in>();
                Socket client_socket;
                std::uint16_t connectionID = 0;
                socklen_t size = sizeof(sockaddr_in);

                errorCheck(client_socket = accept(serverParams.get()->socket.get(), (sockaddr*) client.get(), &size), "Server error (Accepting connection failed)");
		
                while (getFreeConnectionID(connectionID) == false) { }
                connectionsID.at(connectionID) = std::move( std::pair<std::uint16_t, Socket>(connectionID, client_socket) );

                return client_socket;
            }

            return Socket::SOCKET_NONE;
        }


        template<typename Function>
        void Server::runConnection(const Socket& socket, Function&& function)
        {
            for (std::pair<std::uint16_t, Socket>& connection : connectionsID)
            {
                if (socket.get() == connection.second.get()) { connections.get()->add(function); }
            }
        }


        void Server::stopConnection(const Socket& socket)
        {
            if (isRunning == true)
            {
                std::uint16_t connectionID = 0;
                if (getConnectionIDBySocket(socket, connectionID) == true)
                {
                    errorCheck(close(socket.get()), "Server error (Closing client socket failed)");
                    connectionsID.at(connectionID) = std::move(std::pair<std::uint16_t, Socket>(0, Socket::SOCKET_NONE));
                }
            }
        }


        bool Server::isClosedConnection(const Socket& socket)
        {
            int error = 0;
            socklen_t len = sizeof(error);
            errorCheck(getsockopt(socket.get(), SOL_SOCKET, SO_ERROR, &error, &len), "Server error (Getting socket options failed)");

            return !(error == 0);
        }


        void Server::sendMessage(const Socket& socket, const std::string& message, const std::size_t& size)
        {
            if (isRunning == true)
            {
                std::uint16_t connectionID = 0;
                if (getConnectionIDBySocket(socket, connectionID) == true)
                {
                    std::vector<char> buffer(size);
                    for (std::size_t i = 0; i < message.size(); ++i) { buffer.at(i) = message.at(i); }

                    errorCheck(send(socket.get(), (void*) buffer.data(), size, 0), "Server error (Sending message failed)");
                }
            }
        }


        std::string Server::getMessage(const Socket& socket, const std::size_t& size)
        {
            if (isRunning == true)
            {
                std::uint16_t connectionID = 0;
                if (getConnectionIDBySocket(socket, connectionID) == true)
                {
                    std::string buffer;
                    buffer.resize(size);

                    if (recv(socket.get(), (void*) buffer.data(), size, 0) > 0) { return buffer; }
                }
            }

            return std::string();
        }


        bool Server::running() const
        {
            return isRunning;
        }


        std::uint16_t Server::getMaxConnections() const
        {
            return maxConnections;
        }


        Socket Server::getSocket() const
        {
            if (serverParams.get() != nullptr) { return serverParams->socket; }
            return Socket::SOCKET_NONE;
        }

        
        std::string Server::getIPAddress() const
        {
            if (serverParams.get() != nullptr) { return inet_ntoa(serverParams->server.sin_addr); }
            return std::string();
        }


        Port Server::getPort() const
        {
            if (serverParams.get() != nullptr) { return ntohs(serverParams->server.sin_port); }
            return Port::PORT_NONE;
        }
    }
}



#endif  // NETWORKING_TCP_SERVER_HPP
