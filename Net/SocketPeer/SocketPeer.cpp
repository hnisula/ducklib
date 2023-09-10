#include <iostream>

#include "../Socket.h"

int main(int argc, char* argv[])
{
	char ipAddress[32];

	std::cout << "Enter address to send message to: ";
	std::cin >> ipAddress;

	DuckLib::Address address((DuckLib::char8*)ipAddress);

	while (true)
	{ }

	return 0;
}
