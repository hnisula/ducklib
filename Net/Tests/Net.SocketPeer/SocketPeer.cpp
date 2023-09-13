#include <iostream>

#include "../../Net.h"
#include "../../Socket.h"

int main(int, char*[])
{
	// Winsock init
	ducklib::InitilizeNet();

	char listenAddressStr[32] = "127.0.0.1:";
	char listenPortStr[32];
	char ipAddress[32];
	char message[512];

	std::cout << "Enter which port to listen on (0 is any): ";
	std::cin >> listenPortStr;

	strcat_s(&listenAddressStr[10], 32, listenPortStr);
	ducklib::Address listenAddress(listenAddressStr);
	ducklib::Socket socket(&listenAddress);
	std::cout << "Bound to port: " << socket.GetPort() << "\n";

	std::cout << "Enter address to send message to: ";
	std::cin >> ipAddress;

	ducklib::Address destAddress(ipAddress);
	ducklib::Address fromAddress;
	char receiveBuffer[512];

	while (true)
	{
		const int bytesReceived = socket.Receive(&fromAddress, receiveBuffer, sizeof receiveBuffer);

		if (bytesReceived > 0)
			std::cout << "Incoming message: " << receiveBuffer << "\n";

		std::cout << "Enter message (\"quit\" will stop program and \"receive\" will check for incoming messages): \n";
		std::cin >> message;

		if (strcmp(message, "quit") == 0)
			break;

		if (strcmp(message, "receive") == 0)
			continue;

		socket.Send(destAddress, message, 1 + (unsigned int)strlen(message));
	}

	ducklib::ShutdownNet();

	return 0;
}
