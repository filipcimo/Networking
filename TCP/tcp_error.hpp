#ifndef NETWORKING_TCP_ERROR_HPP
#define NETWORKING_TCP_ERROR_HPP

#include <string>
#include <exception>



namespace Networking
{
    namespace TCP
    {
        class tcp_error : public std::exception
        {
            public:
                tcp_error() = default;
                tcp_error(const char* error_message);
                tcp_error(const std::string& error_message);
                tcp_error(const tcp_error& obj);
                tcp_error(tcp_error&& obj);
                virtual ~tcp_error() = default;

                tcp_error& operator=(const tcp_error& obj);
                tcp_error& operator=(tcp_error&& obj);

                virtual const char* what() const noexcept;


            private:
                std::string error_message = "tcp_error: ";
        };


        tcp_error::tcp_error(const char* error_message)
        {
            this->error_message += error_message;
        }


        tcp_error::tcp_error(const std::string& error_message)
        {
            this->error_message += error_message;
        }


        tcp_error::tcp_error(const tcp_error& obj)
        {
            this->error_message = obj.error_message;
        }


        tcp_error::tcp_error(tcp_error&& obj)
        {
            this->error_message = std::move(obj.error_message);
        }


        tcp_error& tcp_error::operator=(const tcp_error& obj)
        {
            this->error_message = obj.error_message;
            return *this;
        }


        tcp_error& tcp_error::operator=(tcp_error&& obj)
        {
            this->error_message = std::move(obj.error_message);
            return *this;
        }


        const char* tcp_error::what() const noexcept
        {
            return this->error_message.data();
        }
    }
}



#endif  // NETWORKING_TCP_ERROR_HPP
