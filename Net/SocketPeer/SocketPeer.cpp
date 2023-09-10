#include "../Socket.h"

int main(int argc, char* argv[])
{
	char ipAddress[32];

	DuckLib::Socket socket(DuckLib::Address("127.0.0.1", 42001));

	while (true)
	{ }

	return 0;
}
