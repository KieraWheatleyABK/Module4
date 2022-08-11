#include <enet/enet.h>
#include <string>
#include <iostream>
#include <thread>

using std::cout;
using std::cin;
using std::endl;
using std::thread;
using std::string;
using std::to_string;

ENetHost* NetHost = nullptr;
ENetPeer* Peer = nullptr;
bool IsServer = false;
thread* PacketThread = nullptr;
int num = 0;
bool solved = false;
bool first = true;

void BroadcastMessageToClient(string message)
{
	ENetPacket* packet = enet_packet_create(message.c_str(), strlen(message.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(NetHost, 0, packet);
	enet_host_flush(NetHost);
}

bool ClientGuessResponse(int guess)
{
	if (guess > num)
	{
		string message = "Too high!\n";
		BroadcastMessageToClient(message);
		cout << message << endl;
		return false;
	}
	else if (guess < num)
	{
		string message = "Too low!\n";
		BroadcastMessageToClient(message);
		cout << message << endl;
		return false;
	}
	else
	{
		string message = "Correct! You got it!\n";
		BroadcastMessageToClient(message);
		cout << message << endl;
		solved = true;
		return true;
	}
}

void SendGuess(ENetEvent event)
{
	int guess;
	cout << "Enter a guess between 1 and 10: ";
	cin >> guess;
	string message = to_string(guess);
	if (guess >= 1 && guess <= 10)
	{
		if (strlen(message.c_str()) > 0)
		{
			ENetPacket* packet = enet_packet_create(message.c_str(), strlen(message.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(event.peer, 0, packet);
		}
	}
}

//can pass in a peer connection if wanting to limit
bool CreateServer()
{
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = 1234;
	NetHost = enet_host_create(&address /* the address to bind the server host to */,
		32      /* allow up to 32 clients and/or outgoing connections */,
		2      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	srand(time(0)); //seed random number generator
	num = rand() % 10 + 1;
	return NetHost != nullptr;
}

bool CreateClient()
{
	NetHost = enet_host_create(NULL /* create a client host */,
		1 /* only allow 1 outgoing connection */,
		2 /* allow up 2 channels to be used, 0 and 1 */,
		0 /* assume any amount of incoming bandwidth */,
		0 /* assume any amount of outgoing bandwidth */);
	return NetHost != nullptr;
}

bool AttemptConnectToServer()
{
	ENetAddress address;
	/* Connect to some.server.net:1234. */
	enet_address_set_host(&address, "127.0.0.1");
	address.port = 1234;
	/* Initiate the connection, allocating the two channels 0 and 1. */
	Peer = enet_host_connect(NetHost, &address, 2, 0);
	return Peer != nullptr;
}

void HandleReceivePacket(const ENetEvent& event)
{
	/* Clean up the packet now that we're done using it. */
	enet_packet_destroy(event.packet);
	{
		enet_host_flush(NetHost);
	}
}

void ServerProcessPackets()
{
	while (!solved)
	{
		ENetEvent event;
		while (enet_host_service(NetHost, &event, 1000) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					cout << "A new client connected from "
						<< event.peer->address.host
						<< ":" << event.peer->address.port
						<< endl;
					/* Store any relevant client information here. */
					event.peer->data = (void*)("Client information");
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE:
				{
					int temp = *(event.packet->data);
					int guess = temp - 48;
					bool response = ClientGuessResponse(guess);
					string s_response = to_string((int)response);
					if (strlen(s_response.c_str()) > 0 && s_response == "0")
					{
						ENetPacket* packet = enet_packet_create(s_response.c_str(), strlen(s_response.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(event.peer, 0, packet);
					}
					HandleReceivePacket(event);
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					cout << (char*)event.peer->data << "disconnected." << endl;
					/* Reset the peer's client information. */
					event.peer->data = NULL;
					//notify remaining player that the game is done due to player leaving
					break;
				}
			}
		}
	}
}

void ClientProcessPackets()
{
	while (!solved)
	{
		ENetEvent event;
		/* Wait up to 1000 milliseconds for an event. */
		while (enet_host_service(NetHost, &event, 1000) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					cout << "Connection succeeded " << endl;
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE:
				{
					SendGuess(event);
					HandleReceivePacket(event);
					break;
				}
			}
			if (first)
			{
				SendGuess(event);
				first = false;
			}
		}
	}
}

int main(int argc, char** argv)
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
	cin >> UserInput;
	if (UserInput == 1)
	{
		if (!CreateServer())
		{
			fprintf(stderr,
				"An error occurred while trying to create an ENet server.\n");
			exit(EXIT_FAILURE);
		}
		IsServer = true;
		cout << "waiting for players to join..." << endl;
		PacketThread = new thread(ServerProcessPackets);
	}
	else if (UserInput == 2)
	{
		if (!CreateClient())
		{
			fprintf(stderr,
				"An error occurred while trying to create an ENet client host.\n");
			exit(EXIT_FAILURE);
		}
		// ENetEvent event;
		if (!AttemptConnectToServer())
		{
			fprintf(stderr,
				"No available peers for initiating an ENet connection.\n");
			exit(EXIT_FAILURE);
		}
		PacketThread = new thread(ClientProcessPackets);
	}
	else
	{
		cout << "Invalid Input" << endl;
	}
	if (PacketThread)
	{
		PacketThread->join();
	}
	delete PacketThread;
	if (NetHost != nullptr)
	{
		enet_host_destroy(NetHost);
	}
	return EXIT_SUCCESS;
}