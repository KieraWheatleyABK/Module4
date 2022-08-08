#pragma once

#include <enet/enet.h>
#include <string>
#include <iostream>

using std::string;

class UDP
{
public:
	UDP()
	{
		m_server = nullptr;
		m_client = nullptr;
	}
	~UDP() {}
	int HandleServerInput();
	int HandleClientInput();
	ENetAddress m_address;
	ENetHost* m_server;
	ENetHost* m_client;
private:
	bool CreateServer();
	bool CreateClient();
	const string HELLO = "hello";
	const string CLIENT = "Client> ";
	const string SERVER = "Server> ";
	const int PORT = 1234;
};