#pragma once
#include <stdlib.h>
#include <iostream>

class ChatScreen {
public:
	ChatScreen();
	~ChatScreen();

	void Init();

	void PostMessage(char username[80], char msg[80]);

	std::string CheckBoxInput();


private:
	int msg_y = 0;
	//WINDOW* inputwin = nullptr;

};
