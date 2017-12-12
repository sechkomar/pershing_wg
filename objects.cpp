#include "objects.h"
#include <list>
#include <string>

void to_json(json& j, const Train& t) {
	j = json{
		{ "event", t.events },
		{ "goods", t.goods },
		{ "goods_capacity", t.goods_capacity },
		{ "idx", t.idx },
		{ "level", t.level },
		{ "line_idx", t.line_idx },
		{ "next_level_price", t.next_level_price },
		{ "player_id", t.player_id },
		{ "position", t.position },
		{ "post_type", t.post_type },
		{ "speed", t.speed }
	};
}

void to_json(json & j, const Line & line) {
	j = json{
		{ "idx", line.idx },
		{ "length", line.length },
		{ "point", line.points }
	};
}

void to_json(json & j, const Market& mrk) {
	j = json{
		{ "event", mrk.events },
		{ "idx", mrk.idx },
		{ "name", mrk.name },
		{ "point_id", mrk.point_id },
		{ "product", mrk.product },
		{ "product_capacity", mrk.product_capacity },
		{ "replenishment", mrk.replenishment },
	};
}

void to_json(json & j, const Town& town) {
	j = json{
		{ "armor", town.armor },
		{ "armor_capacity", town.armor_capacity },
		{ "event", town.events },
		{ "idx", town.idx },
		{ "level", town.level },
		{ "name", town.name },
		{ "next_level_price", town.next_level_price },
		{ "player_id", town.player_id },
		{ "point_id", town.point_id },
		{ "population", town.population },
		{ "population_capacity", town.population_capacity },
		{ "product", town.product },
		{ "product_capacity", town.product_capacity }
	};
}

void to_json(json& j, const Event& e) {
	j = json{
		{ e.event_power_name, e.event_power },
		{ "type", e.type },
		{ "tick", e.tick }
	};
}

void from_json(const json& j, Event& e) {
	for (auto data = j.begin(); data != j.end(); ++data) {
		if (data.key() != "tick" && data.key() != "type") {
			e.event_power = data.value();
			e.event_power_name = data.key();
		}
	}
	e.tick = j.at("tick").get<uint32_t>();
	e.type = j.at("type").get<uint32_t>();
}

void from_json(const json& j, Train& t) {
	t.events = j.at("event").get<std::list<Event>>();
	t.goods = j.at("goods").get<uint32_t>();
	t.goods_capacity = j.at("goods_capacity").get<uint32_t>();
	t.idx = j.at("idx").get<uint32_t>();
	t.level = j.at("level").get<uint32_t>();
	t.line_idx = j.at("line_idx").get<uint32_t>();
	t.next_level_price = j.at("next_level_price").get<uint32_t>();
	t.player_id = j.at("player_id").get<std::string>();
	t.position = j.at("position").get<uint32_t>();
	t.post_type = j.at("post_type") == nullptr ? 0 : j.at("post_type").get<uint32_t>();
	t.speed = j.at("speed").get<uint32_t>();
}

void from_json(const json & j, Line & line) {
	line.idx = j.at("idx").get<uint32_t>();
	line.length = j.at("length").get<uint32_t>();
	line.points = j.at("point").get<std::pair<uint32_t, uint32_t>>();
}

void from_json(const json & j, Market & post) {
	post.events = j.at("event").get<std::list<Event>>();
	post.idx = j.at("idx").get<uint32_t>();
	post.name = j.at("name").get<std::string>();
	post.point_id = j.at("point_id").get<uint32_t>();
	post.product = j.at("product").get<uint32_t>();
	post.product_capacity = j.at("product_capacity").get<uint32_t>();
	post.replenishment = j.at("replenishment").get<uint32_t>();
}

void from_json(const json & j, Town & town) {
	town.armor = j.at("armor").get<uint32_t>();
	town.armor_capacity = j.at("armor_capacity").get<uint32_t>();
	town.idx = j.at("idx").get<uint32_t>();
	town.level = j.at("level").get<uint32_t>();
	town.name = j.at("name").get<std::string>();
	town.next_level_price = j.at("next_level_price").get<uint32_t>();
	town.player_id = j.at("player_id").get<std::string>();
	town.point_id = j.at("point_id").get<uint32_t>();
	town.population = j.at("population").get<uint32_t>();
	town.population_capacity = j.at("population_capacity").get<uint32_t>();
	town.product = j.at("product").get<uint32_t>();
	town.product_capacity = j.at("product_capacity").get<uint32_t>();
}
