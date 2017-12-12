#include "game.h"
#include "path.h"
#include <string>
#include <iostream>
#include <fstream>


#pragma execution_character_set("utf-8")

int main(int argc, char* argv[]) {

	std::ofstream log("log.txt");
	Game game;
	if (!game.init()) {
		return EXIT_FAILURE;
	}

	//std::list<Train> trains_to_upgrade;
	//std::list<Town> posts_to_upgrade;
	//trains_to_upgrade.push_back(game.trains.at(1));
	//posts_to_upgrade.push_back(game.towns.at(1));
	//game.upgrade(posts_to_upgrade, towns_to_upgrade);		 upgrade costs both armor and product

	game.Dijkstra(game.home.idx);
	for (auto market : game.markets) {
		game.Dijkstra(market.second.point_id);
	}

	auto repls = game.get_replenishments();
	auto capas = game.get_capacities();
	auto markets_point_id = game.get_market_point_id();

	auto min_markets_pathes = game.get_min_markets_pathes();

	CalculatePath path(min_markets_pathes, game.trains[0].goods, repls, capas,
		game.towns[1].population, game.home.idx, game.get_market_point_id());

	while (game.turn_number < 10) {

		path.set_init_products(game.get_markets_product());
		path.set_product_in_town(game.towns[1].product);

		auto best_path = path.find_pathways();
		auto next_points = game.get_full_path(best_path);

		game.shopping(next_points, game.trains[0].idx);
		game.update();
		game.turn();
	}

	int result = game.end();
	log.close();
	return 0;
}
