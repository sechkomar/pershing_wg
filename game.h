#pragma once

#include <map>
#include <string>
#include <vector>
#include "objects.h"
#include "socket.h"


namespace game
{

	static Home home;
	static std::map<uint32_t, Train> trains;
	static std::string idx;
	static std::map<uint32_t, std::vector<Endpoint>> map;

	static Socket socket;


	void get_map();

	bool init();

	bool login(std::string name);

	int end();

	void move(uint32_t line_idx, int speed, uint32_t train_idx);

	void turn();

	void get_login_response(json jLoginResp);
};

