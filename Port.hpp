#ifndef NETWORKING_PORT_HPP
#define NETWORKING_PORT_HPP

#include <cinttypes>



namespace Networking
{
    class Port
    {
        public:
            Port() = default;
            Port(const std::int32_t& port);
            Port(const Port& obj);
            Port(Port&& obj) = delete;
            ~Port() = default;

            Port& operator=(const std::int32_t& port);
            Port& operator=(const Port& obj);
            Port& operator=(Port&& obj) = delete;
            operator std::int32_t() const;
            std::int32_t get() const;


        public:
            enum PORT_TAGS
            {
                PORT_NONE = -1,
                PORT_MIN = 0,
                PORT_MAX = 65535
            };


        private:
            std::int32_t port = PORT_NONE;
    };


    Port::Port(const std::int32_t& port)
    {
        if (port > PORT_MAX || port < PORT_NONE) { this->port = PORT_NONE; }
        else { this->port = port; }
    }


    Port::Port(const Port& obj)
    {
        this->port = obj.port;
    }
    

    Port& Port::operator=(const std::int32_t& port)
    {
        if (port > PORT_MAX || port < PORT_NONE) { this->port = PORT_NONE; }
        else { this->port = port; }

        return *this;
    }


    Port& Port::operator=(const Port& obj)
    {
        this->port = obj.port;
        return *this;
    }

    
    Port::operator std::int32_t() const
    {
        return this->port;
    }

    
    std::int32_t Port::get() const
    {
    	return this->port;
    }
}




#endif  // NETWORKING_PORT_HPP
