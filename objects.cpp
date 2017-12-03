#include "objects.h"


void to_json(json& j, const Train& t) {
	j = json{ { "idx", t.idx },{ "line_idx", t.line_idx },{ "player_id", t.player_id },
	{ "position", t.position },{ "speed", t.speed },
	{"capacity", t.capacity}, {"product", t.product} };
}

void to_json(json & j, const Line & line) {
	j = json{ { "idx", line.idx },{ "length", line.length },{ "point", line.points } };
}

void to_json(json & j, const Market & m) {
	j = json{
		{"idx", m.idx},
		{"name", m.name},
		{"product", m.product},
		{ "product_capacity", m.capacity },
		{ "replenishment", m.replenishment },
		{ "type", m.type },
	};
}

void to_json(json & j, const Town & t) {
	j = json{
		{ "id", t.id },
		{ "name", t.name },
		{ "product", t.product },
		{ "population", t.population },
		{ "type", t.type },
	};
}

void from_json(const json& j, Train& t) {
	t.idx = j.at("idx").get<uint32_t>();
	t.line_idx = j.at("line_idx").get<uint32_t>();
	t.player_id = j.at("player_id").get<std::string>();
	t.position = j.at("position").get<uint32_t>();
	t.speed = j.at("speed").get<uint32_t>();
	t.capacity = j.at("capacity").get<uint32_t>();
	t.product = j.at("product").get<uint32_t>();
}

void from_json(const json & j, Line & line) {
	line.idx = j.at("idx").get<uint32_t>();
	line.length = j.at("length").get<uint32_t>();
	line.points = j.at("point").get<std::pair<uint32_t, uint32_t>>();
}

void from_json(const json & j, Market & m){
	m.idx = j.at("idx").get<uint32_t>();
	m.name = j.at("name").get<std::string>();

	m.product = j.at("product").get<uint32_t>();
	m.capacity = j.at("product_capacity").get<uint32_t>();
	m.replenishment = j.at("replenishment").get<uint32_t>();
	m.type = j.at("type").get<uint32_t>();
}

void from_json(const json & j, Town & t) {
	t.id = j.at("id").get<uint32_t>();
	t.name = j.at("name").get<std::string>();

	t.product = j.at("product").get<uint32_t>();
	t.population = j.at("population").get<uint32_t>();
	t.type = j.at("type").get<uint32_t>();
}
