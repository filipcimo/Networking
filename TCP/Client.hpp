#ifndef NETWORKING_TCP_CLIENT_HPP
#define NETWORKING_TCP_CLIENT_HPP

#include "../Port.hpp"
#include "../Socket.hpp"
#include "tcp_error.hpp"

#include <memory>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>



namespace Networking
{
    namespace TCP
    {
        class Client
        {
            public:
                Client() = default;
                Client(const std::string& ip_address_server, const Port& server_port);
                Client(const Client& client) = delete;
                Client(Client&& client);
                ~Client() = default;

                Client& operator=(const Client& client) = delete;
                Client& operator=(Client&& client);

                void doConnection();
                void disconnect();
                void sendMessage(const std::string& message, const std::size_t& size);
                std::string getMessage(const std::size_t& size);


            private:
                void errorCheck(int errorValue, const char* errorMessage);

            
            private:
                struct ClientParams
                {
                    sockaddr_in server;
                    Socket socket;
                };

                std::unique_ptr<ClientParams> clientParams;
        };


        Client::Client(const std::string& ip_address_server, const Port& server_port)
        {
            clientParams = std::move(std::make_unique<ClientParams>());
            memset(&clientParams.get()->server, 0, sizeof(sockaddr_in));

            clientParams.get()->server.sin_family = AF_INET;
            clientParams.get()->server.sin_addr.s_addr = inet_addr(ip_address_server.data());
            clientParams.get()->server.sin_port = htons(server_port.get());
        }


        Client::Client(Client&& client)
        {
            clientParams = std::move(client.clientParams);
        }


        Client& Client::operator=(Client&& client)
        {
            clientParams = std::move(client.clientParams);
            return *this;
        }


        void Client::errorCheck(int errorValue, const char* errorMessage)
        {
            if (errorValue == -1) {
                throw TCP::tcp_error(errorMessage);
            }
        }


        void Client::doConnection()
        {
            if (clientParams.get() != nullptr)
            {
                errorCheck(clientParams.get()->socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP), "Client error (Socket creating failed)");
                errorCheck(connect(clientParams.get()->socket.get(), (sockaddr*) &clientParams.get()->server, sizeof(sockaddr_in)), "Client error (Connection to server failed)");
            }
        }


        void Client::disconnect()
        {
            if (clientParams.get() != nullptr)
            {
                errorCheck(close(clientParams.get()->socket.get()), "Client error, closing socket failed");
                clientParams.reset();
            }
        }


        void Client::sendMessage(const std::string& message, const std::size_t& size)
        {
            if (clientParams.get() != nullptr)
            {
                std::vector<char> buffer(size);
                for (std::size_t i = 0; i < message.size(); ++i) { buffer.at(i) = message.at(i); }

                errorCheck(send(clientParams.get()->socket.get(), (void*) buffer.data(), size, 0), "Client error (Sending message failed)");
            }
        }


        std::string Client::getMessage(const std::size_t& size)
        {
            if (clientParams.get() != nullptr)
            {
                std::string buffer;
                buffer.resize(size);

                if (recv(clientParams.get()->socket.get(), (void*) buffer.data(), size, 0) > 0) { return buffer; }
            }

            return std::string();
        }
    }
}



#endif  // NETWORKING_TCP_CLIENT_HPP