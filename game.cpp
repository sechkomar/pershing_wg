#include <list>
#include "objects.h"
#include "game.h"

void game::get_static_map() {

	ResponseMessage resp;
	get_layer_response(layer::STATIC, resp);

	std::cout << "static_map:\n" << resp.data << std::endl;

	json jMapResp = json::parse(resp.data);

	std::list<Line> lines = jMapResp.at("line").get<std::list<Line>>();

	for (auto line : lines) {
		Endpoint in, out;
		in.line_idx = line.idx;
		in.end = line.points.second;
		in.length = line.length;
		in.direction = 1;
		map[line.points.first].push_back(in);

		out.line_idx = line.idx;
		out.end = line.points.first;
		out.length = line.length;
		out.direction = -1;
		map[line.points.second].push_back(out);
	}

	for (auto point : jMapResp.at("point")) {
		if (!point["post_id"].is_null() && point["idx"] != home.idx) {
			markets_location[point["post_id"]] = point["idx"];
		}
	}

	//std::cout << "markets:" << std::endl;
	//for (auto m : markets) {
	//	std::cout << m.first << "-" << json(m.second) << "-" << m.second.point_id << std::endl;
	//}
}

void game::init_dynamic_map() {
	ResponseMessage resp;
	get_layer_response(layer::DYNAMIC, resp);
	std::cout << "dynamic map:\n" << resp.data << std::endl;

	json jDynamicMap(resp.data);

	// it does not work :(
	//for (auto post : jDynamicMap.at("post")) {
	//	if (post["type"] == post_type::TOWN) {
	//		Town t(post);
	//		towns[t.id] = t;
	//	}
	//	else if (post["type"] == post_type::MARKET) {
	//		Market m(post);
	//		m.point_id = markets_location[m.idx];
	//		markets[m.idx] = m;
	//	}
	//}

	for (auto train : jDynamicMap.at("train")) {

	}
}

void game::get_layer_response(layer type, ResponseMessage& resp) {
	json jLayer;
	jLayer["layer"] = type;

	std::string strLayer = jLayer.dump();
	ActionMessage act(Action::MAP, strLayer);
	socket.make_move(act, resp);
}

bool game::init() {
	const char* HOST_NAME = "wgforge-srv.wargaming.net";
	const char* PORT = "443";
	std::string CLIENT_NAME = "Pershing";

	if (socket.Connect(HOST_NAME, PORT)) {
		std::cout << "Error while connecting." << std::endl;
		return false;
	}

	if (!login(CLIENT_NAME)) {
		std::cout << "Error while game::login()." << std::endl;
		return false;
	}

	//for (auto train : trains) {
	//	std::cout << json(train) << std::endl;
	//}

	get_static_map();
	init_dynamic_map();

	return true;
}

bool game::login(std::string name) {

	json jLogin;
	jLogin["name"] = name;

	std::string strLogin = jLogin.dump();
	ActionMessage act(Action::LOGIN, strLogin);

	ResponseMessage resp;
	socket.make_move(act, resp);

	if (resp.data_length == 0) {
		return false;
	}

	std::cout << "login response:\n" << resp.data << std::endl;

	json jLoginResp = json::parse(resp.data);
	get_login_response(jLoginResp);

	
	std::string current = ">> login as \"" + name + "\".";
	std::cout << current << std::endl;
	return true;
}

void game::move(uint32_t line_idx, int speed, uint32_t train_idx) {
	// TODO CHECK IT TOO
	json jMove = json({ { "line_idx", line_idx }, { "speed", speed }, { "train_idx", train_idx } });
	std::string strMove = jMove.dump();
	ActionMessage act = ActionMessage(Action::MOVE, strMove);
	socket.Send(act);
}

void game::turn() {
	// TODO: CHECK IT
	ActionMessage act = ActionMessage(Action::TURN, std::string(""));
	socket.Send(act);
}

void game::get_login_response(json jLoginResp) {

	if (!jLoginResp["home"].is_null()) {
		home.idx = jLoginResp["home"]["idx"].get<uint32_t>();
		home.post_id = jLoginResp["home"]["post_id"].get<uint32_t>();
	}

	idx = jLoginResp["idx"].get<std::string>();

	std::list<Train> trains_list;
	if (jLoginResp["train"].size() != 0) {
		trains_list = jLoginResp["train"].get<std::list<Train>>();
		for (auto train : trains_list) {
			trains[train.idx] = train;
		}
	}

}

int game::end() {
	ActionMessage act = ActionMessage(Action::LOGOUT, std::string());
	ResponseMessage resp;
	socket.make_move(act, resp);

	std::cout << ">> logout" << std::endl;

	socket.Disconnect();
	return EXIT_SUCCESS;
}

