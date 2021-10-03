#include "UDP/Point.hpp"

#include <unistd.h>
#include <stdlib.h>



int main()
{
	try
	{
		Networking::UDP::Point server("127.0.0.1", 8000);
		Networking::UDP::Point client("127.0.0.2", 8000);

		client.config();

		std::cout << server.getSocket() << std::endl << server.getIPAddress() << std::endl << server.getPort() << std::endl << std::endl;
		std::cout << client.getSocket() << std::endl << client.getIPAddress() << std::endl << client.getPort() << std::endl << std::endl;
	
		for (size_t i = 0; i < 10; ++i)
		{	
			client.sendMessage(server, "Hello from 'main2.cpp'");
			std::string message = std::move(client.getMessage(server, 64));
			if (message.empty() == false) { std::cout << message << std::endl; }
		}
		
		client.reset();
	}

	catch (const Networking::UDP::udp_error& err)
	{
		std::cerr << err.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	
	return EXIT_SUCCESS;
}
