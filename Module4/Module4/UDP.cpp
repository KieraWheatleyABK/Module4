#include "UDP.h"

using std::cin;
using std::cout;
using std::endl;
using std::getline;

bool UDP::CreateServer()
{
	m_address.host = ENET_HOST_ANY;
	m_address.port = PORT;
	m_server = enet_host_create(&m_address, 32, 2, 0, 0);
	return m_server != nullptr;
}

bool UDP::CreateClient()
{
	m_client = enet_host_create(NULL, 1, 2, 0, 0);
	return m_client != nullptr;
}

int UDP::HandleServerInput()
{
	if (!CreateServer())
	{
		fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}
	bool first = true;
	while (1)
	{
		ENetEvent event;
		/* Wait up to 1000 milliseconds for an event. */
		while (enet_host_service(m_server, &event, 1000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				cout << "A new client connected from "
					<< event.peer->address.host
					<< ":" << event.peer->address.port
					<< endl;
				cout << "Press q to quit." << endl;
				event.peer->data = (void*)("Client");
				{
					ENetPacket* packet = enet_packet_create(
						"hello",
						strlen("hello") + 1,
						ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(m_server, 0, packet);
					enet_host_flush(m_server);
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				cout << CLIENT << (char*)event.packet->data << endl;
				{
					cout << SERVER;
					string message = HELLO;
					if (first)
					{
						getline(cin, message);
						first = false;
					}
					getline(cin, message);
					if (message == "q") return 0;
					if (strlen(message.c_str()) > 0)
					{
						ENetPacket* packet = enet_packet_create(
							message.c_str(), 
							strlen(message.c_str()) + 1, 
							ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(event.peer, 0, packet);
					}
				}
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				cout << (char*)event.peer->data << " disconnected." << endl;
				/* Reset the peer's client information. */
				event.peer->data = NULL;
			}
		}
	}
}

int UDP::HandleClientInput()
{
	if (!CreateClient())
	{
		fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
		exit(EXIT_FAILURE);
	}
	ENetAddress address;
	ENetEvent event;
	ENetPeer* peer;
	enet_address_set_host(&address, "127.0.0.1");
	address.port = PORT;
	/* Initiate the connection, allocating the two channels 0 and 1. */
	peer = enet_host_connect(m_client, &address, 2, 0);
	if (peer == NULL)
	{
		fprintf(stderr, "No available peers for initiating an ENet connection.\n");
		exit(EXIT_FAILURE);
	}
	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(m_client, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		cout << "Connection to 127.0.0.1:" << PORT << " succeeded." << endl;
		cout << "Press q to quit." << endl;
	}
	else
	{
		enet_peer_reset(peer);
		cout << "Connection to 127.0.0.1:" << PORT << " failed." << endl;
	}
	bool first = true;
	while (1)
	{
		ENetEvent event;
		while (enet_host_service(m_client, &event, 1000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				cout << SERVER << (char*)event.packet->data << endl;
				{
					cout << CLIENT;
					string message;
					if (first)
					{
						getline(cin, message);
						first = false;
					}
					getline(cin, message);
					if (message == "q") return 0;
					if (strlen(message.c_str()) > 0 && peer)
					{
						ENetPacket* packet = enet_packet_create(
							message.c_str(), 
							strlen(message.c_str()) + 1, 
							ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packet);
					}
				}
				enet_packet_destroy(event.packet);
				break;
			}
		}
	}
}