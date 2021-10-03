#ifndef NETWORKING_SOCKET_HPP
#define NETWORKING_SOCKET_HPP

#include <cinttypes>



namespace Networking
{
    class Socket
    {
        public:
            Socket() = default;
            Socket(const std::int32_t& socket);
            Socket(const Socket& obj);
            Socket(Socket&& obj) = delete;
            ~Socket() = default;

            Socket& operator=(const std::int32_t& socket);
            Socket& operator=(const Socket& obj);
            Socket& operator=(Socket&& obj) = delete;
            operator std::int32_t() const;
            std::int32_t get() const;


        public:
            enum SOCKET_CONSTANTS
            {
                SOCKET_NONE = -1,
                SOCKET_MIN = 0,
                SOCKET_MAX = 65535
            };


        private:
            std::int32_t socket = SOCKET_NONE;
    };


    Socket::Socket(const std::int32_t& socket)
    {
        if (socket > SOCKET_MAX || socket < SOCKET_NONE) { this->socket = SOCKET_NONE; }
        else { this->socket = socket; }
    }


    Socket::Socket(const Socket& obj)
    {
        this->socket = obj.socket;
    }


    Socket& Socket::operator=(const std::int32_t& socket)
    {
        if (socket > SOCKET_MAX || socket < SOCKET_NONE) { this->socket = SOCKET_NONE; }
        else { this->socket = socket; }

        return *this;
    }


    Socket& Socket::operator=(const Socket& obj)
    {
        this->socket = obj.socket;
        return *this;
    }
    
    
    Socket::operator std::int32_t() const
    {
        return this->socket;
    }
    
    
    std::int32_t Socket::get() const
    {
    	return this->socket;
    }
}




#endif  // NETWORKING_SOCKET_HPP
