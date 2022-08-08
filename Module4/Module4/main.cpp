#include "UDP.h"

using std::cin;
using std::cout;
using std::endl;

int main()
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		cout << "An error occurred while initializing ENet." << endl;
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);

	cout << "1) Create Server " << endl;
	cout << "2) Create Client " << endl;

	int UserInput;
	UDP udp;
	cin >> UserInput;

	if (UserInput == 1)
	{
		udp.HandleServerInput();
	}
	else if (UserInput == 2)
	{
		udp.HandleClientInput();
	}
	else
	{
		cout << "Invalid Input" << endl;
	}
	if (udp.m_server != nullptr)
	{
		enet_host_destroy(udp.m_server);
	}
	if (udp.m_client != nullptr)
	{
		enet_host_destroy(udp.m_client);
	}
	return EXIT_SUCCESS;
}