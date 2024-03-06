#include <iostream>

#include "../../Net.h"
#include "../../Socket.h"

int main(int, char*[])
{
	// Winsock init
	ducklib::InitializeNet();

	uint16_t listenPort;
	char ipAddress[32];
	char message[512];

	std::cout << "Enter which port to listen on (0 is any): ";
	std::cin >> listenPort;

	ducklib::Socket socket(listenPort);
	std::cout << "Bound to port: " << socket.get_port() << "\n";

	std::cout << "Enter address to send message to: ";
	std::cin >> ipAddress;

	auto [ip, port] = ducklib::split_address(ipAddress);
	ducklib::Address destAddress(ip, port);
	ducklib::Address fromAddress;
	uint8_t receiveBuffer[512];

	while (true)
	{
		const int bytesReceived = socket.receive(fromAddress, receiveBuffer, sizeof receiveBuffer);

		if (bytesReceived > 0)
			std::cout << "Incoming message: " << receiveBuffer << "\n";

		std::cout << "Enter message (\"quit\" will stop program and \"receive\" will check for incoming messages): \n";
		std::cin >> message;

		if (strcmp(message, "quit") == 0)
			break;

		if (strcmp(message, "receive") == 0)
			continue;

		socket.send(destAddress, (uint8_t*)message, 1 + (unsigned int)strlen(message) * sizeof(char));
	}

	ducklib::ShutdownNet();

	return 0;
}