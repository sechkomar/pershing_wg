#include "game.h"

#include <string>
#include <iostream>
#include <fstream>


#pragma execution_character_set("utf-8")

int main(int argc, char* argv[]) {
	
	std::ofstream log("log.txt");

	if (!game::init()) {
		return EXIT_FAILURE;
	}

	game::update();

	game::move(13, 1, 0);
	game::turn();
	game::move(13, 1, 0);
	game::turn();
	game::update();


	int result = game::end();
	
	system("pause");
	return 0;
}
