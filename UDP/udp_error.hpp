#ifndef NETWORKING_UDP_ERROR_HPP
#define NETWORKING_UDP_ERROR_HPP

#include <string>
#include <exception>



namespace Networking
{
    namespace UDP
    {
        class udp_error : public std::exception
        {
            public:
                udp_error() = default;
                udp_error(const char* error_message);
                udp_error(const std::string& error_message);
                udp_error(const udp_error& obj);
                udp_error(udp_error&& obj);
                virtual ~udp_error() = default;

                udp_error& operator=(const udp_error& obj);
                udp_error& operator=(udp_error&& obj);

                virtual const char* what() const noexcept;


            private:
                std::string error_message = "udp_error: ";
        };


        udp_error::udp_error(const char* error_message)
        {
            this->error_message = error_message;
        }


        udp_error::udp_error(const std::string& error_message)
        {
            this->error_message = error_message;
        }


        udp_error::udp_error(const udp_error& obj)
        {
            this->error_message = obj.error_message;
        }


        udp_error::udp_error(udp_error&& obj)
        {
            this->error_message = std::move(obj.error_message);
        }


        udp_error& udp_error::operator=(const udp_error& obj)
        {
            this->error_message = obj.error_message;
            return *this;
        }


        udp_error& udp_error::operator=(udp_error&& obj)
        {
            this->error_message = std::move(obj.error_message);
            return *this;
        }


        const char* udp_error::what() const noexcept
        {
            return this->error_message.data();
        }
    }
}



#endif  // NETWORKING_UDP_ERROR_HPP