#pragma once
#include <string>
#include <list>
#include "nlohmann-json\json.hpp"
using json = nlohmann::json;

struct Home {
	uint32_t idx;
	uint32_t post_id;
};

struct Event {
	uint32_t type;
	uint32_t tick;
	uint32_t event_power;
	std::string event_power_name;
};

struct Train {
	std::list<Event> events;
	uint32_t goods;
	uint32_t goods_capacity;
	uint32_t idx;
	uint32_t level;
	uint32_t line_idx;
	uint32_t next_level_price;
	std::string player_id;
	uint32_t position;
	uint32_t post_type;
	uint32_t speed;

	void set(uint32_t line_idx_, uint32_t position_, uint32_t speed_, uint32_t goods_) {
		line_idx = line_idx_;
		position = position_;
		speed = speed_;
		goods = goods_;
	}
};

struct Line {
	uint32_t idx;
	uint32_t length;
	std::pair<uint32_t, uint32_t> points;
};

struct Endpoint {
	uint32_t end;
	uint32_t line_idx;
	uint32_t length;
	uint32_t direction;
};

struct Market {		//post.type == 2
	std::list<Event> events;
	uint32_t idx;
	std::string name;
	uint32_t point_id;
	uint32_t product;
	uint32_t product_capacity;
	uint32_t replenishment;

	void set(uint32_t product_) {
		product = product_;
	}
};

struct Town {		//post.type == 1
	uint32_t armor;
	uint32_t armor_capacity;
	std::list<std::string> events;
	uint32_t idx;
	uint32_t level;
	std::string name;
	uint32_t next_level_price;
	std::string player_id;
	uint32_t point_id;
	uint32_t population;
	uint32_t population_capacity;
	uint32_t product;
	uint32_t product_capacity;

	void set(uint32_t population_, uint32_t product_) {
		population = population_;
		product = product_;
	}
};

void to_json(json& j, const Train& t);
void to_json(json& j, const Line& t);
void to_json(json& j, const Market& t);
void to_json(json& j, const Town& t);

void from_json(const json& j, Train& t);
void from_json(const json& j, Line& t);
void from_json(const json& j, Market& t);
void from_json(const json& j, Town& t);

enum layer : uint32_t {
	STATIC = 0,
	DYNAMIC = 1,
	COORDINATES = 10
};

enum post_type : uint32_t {
	TOWN = 1,
	MARKET = 2
};

enum event_type : uint32_t {
	TRAIN_COLLISION = 1,
	HIJACKERS_ASSAULT = 2,
	PARASITES_ASSAULT = 3,
	REFUGEES_ARRIVAL = 4,
	RESOURCE_OVERFLOW = 5,
	RESOURCE_LACK = 6,
	GAME_OVER = 100
};