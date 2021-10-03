#ifndef NETWORKING_UDP_POINT_HPP
#define NETWORKING_UDP_POINT_HPP

#include "../Socket.hpp"
#include "../Port.hpp"
#include "udp_error.hpp"

#include <iostream>
#include <string>
#include <string.h>
#include <memory>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>



namespace Networking
{
    namespace UDP
    {
        class Point
        {
            public:
                Point() = default;
                Point(const std::string& ip_address, const Port& port);
                Point(const Point& obj) = delete;
                Point(Point&& obj);
                ~Point() = default;

                Point& operator=(const Point& obj) = delete;
                Point& operator=(Point&& obj);

                void config();
                void reset();
                void sendMessage(const Point& point, const std::string& message);
                std::string getMessage(const Point& point, const std::size_t& size);

                Socket getSocket() const;
                std::string getIPAddress() const;
                Port getPort() const;


            private:
                void errorCheck(int errorValue, const char* errorMessage);


            private:
                struct Params
                {
                    sockaddr_in point;
                    Socket socket;
                };


            private:
                std::unique_ptr<Params> params;
        };


        void Point::errorCheck(int errorValue, const char* errorMessage)
        {
            if (errorValue == -1)
            {
                throw udp_error(errorMessage);
            }
        }


        Point::Point(const std::string& ip_address, const Port& port)
        {
            params = std::move(std::make_unique<Params>());
            memset(&params->point, 0, sizeof(sockaddr_in));
            
            params->point.sin_family = AF_INET;
            params->point.sin_addr.s_addr = inet_addr(ip_address.data());
            params->point.sin_port = htons(port.get());
        }


        Point::Point(Point&& obj)
        {
            params = std::move(obj.params);
        }


        Point& Point::operator=(Point&& obj)
        {
            params = std::move(obj.params);
            return *this;
        }


        void Point::config()
        {
            if (params.get() != nullptr)
            {
                const int optionValue = 1;

                errorCheck(params->socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP), "Socket creating failed");
                errorCheck(setsockopt(params->socket.get(), SOL_SOCKET, SO_REUSEPORT, &optionValue, sizeof(optionValue)), "Socket options setting failed");
                errorCheck(bind(params->socket.get(), (sockaddr*) &params.get()->point, sizeof(sockaddr_in)), "Binding socket to a port failed");
            }
        }


        void Point::reset()
        {
            if (params.get() != nullptr)
            {
                errorCheck(close(params->socket.get()), "Closing socket failed");
                params.reset();
            }
        }


        void Point::sendMessage(const Point& point, const std::string& message)
        {
            if (params.get() != nullptr && point.params.get() != nullptr)
            {
                errorCheck(sendto(params->socket.get(), (void*) message.data(), message.size(), 0, (sockaddr*) &point.params->point, sizeof(sockaddr_in)), "Sending message failed");
            }
        }


        std::string Point::getMessage(const Point& point, const std::size_t& size)
        {
            if (params.get() != nullptr && point.params.get() != nullptr)
            {
                std::string message(size, ' ');
                socklen_t clientSize = sizeof(sockaddr_in);

                if (recvfrom(params->socket.get(), (void*) message.data(), size, 0, (sockaddr*) &point.params->point, &clientSize) != -1) { return message.data(); }
            }

            return std::string();
        }


        Socket Point::getSocket() const
        {
            if (params.get() != nullptr) { return params->socket; }
            return Socket::SOCKET_NONE;
        }

        
        std::string Point::getIPAddress() const
        {
            if (params.get() != nullptr) { return inet_ntoa(params->point.sin_addr); }
            return std::string();
        }


        Port Point::getPort() const
        {
            if (params.get() != nullptr) { return ntohs(params->point.sin_port); }
            return Port::PORT_NONE;
        }
    }
}



#endif  // NETWORKING_UDP_POINT_HPP
