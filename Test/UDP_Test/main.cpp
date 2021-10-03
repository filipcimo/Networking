#include "UDP/Point.hpp"

#include <unistd.h>
#include <stdlib.h>



int main()
{
	try
	{
		Networking::UDP::Point server("127.0.0.1", 8000);
		Networking::UDP::Point client("127.0.0.2", 8000);

		server.config();

		std::cout << server.getSocket() << std::endl << server.getIPAddress() << std::endl << server.getPort() << std::endl << std::endl;
		std::cout << client.getSocket() << std::endl << client.getIPAddress() << std::endl << client.getPort() << std::endl << std::endl;
	
		while (true)
		{	
			std::string message = std::move(server.getMessage(client, 64));
			if (message.empty() == false) { std::cout << message << std::endl; }
			server.sendMessage(client, "Hello from 'main.cpp'");
		}
		
		server.reset();
	}

	catch (const Networking::UDP::udp_error& err)
	{
		std::cerr << err.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	
	return EXIT_SUCCESS;
}
