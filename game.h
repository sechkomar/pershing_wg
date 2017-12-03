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

	static std::map<uint32_t, Market> markets;
	static std::map<uint32_t, Town> towns;

	static std::map<uint32_t, uint32_t> markets_location; //market_id -> post_id
	
	static Socket socket;


	void get_static_map();
	void init_dynamic_map();

	void get_layer_response(layer layer_type, ResponseMessage& resp);

	bool init();

	bool login(std::string name);

	int end();

	void move(uint32_t line_idx, int speed, uint32_t train_idx);

	void turn();

	void get_login_response(json jLoginResp);
};

