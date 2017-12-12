#include <list>
#include "objects.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <set>
#include "game.h"

void Game::init_static_map() {

	ResponseMessage resp;

	get_layer_response(layer::COORDINATES, resp);

	fcoord << "get coordinates:\n" << resp.data << std::endl;

	get_layer_response(layer::STATIC, resp);

//	fstat << "get static_map:\n" << resp.data << std::endl;


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
}

bool Game::init_dynamic_map() {

	ResponseMessage resp;
	get_layer_response(layer::DYNAMIC, resp);

	if (resp.resp_code != Response::OKEY) {
		return false;
	}

//	fdyn << "init dynamic map:\n" << resp.data << std::endl;


	json jMapResp = json::parse(resp.data);

	for (auto post : jMapResp.at("post")) {
		if (post["type"] == post_type::TOWN) {
			Town t(post);
			towns[t.idx] = t;
		}
		else if (post["type"] == post_type::MARKET) {
			Market m(post);
			//m.point_id = markets_location[m.idx];
			markets[m.idx] = m;
		}
	}

	std::list<Train> trainsList = jMapResp.at("train").get<std::list<Train>>();
	for (auto train : trainsList) {
		trains[train.idx] = train;
	}

	return true;
}

void Game::get_layer_response(layer type, ResponseMessage& resp) {
	json jLayer;
	jLayer["layer"] = type;

	std::string strLayer = jLayer.dump();
	ActionMessage act(Action::MAP, strLayer);
	socket.make_move(act, resp);
}

bool Game::init() {
	const char* HOST_NAME = "wgforge-srv.wargaming.net";
	const char* PORT = "443";
	std::string CLIENT_NAME = "pershing2";

	if (socket.Connect(HOST_NAME, PORT)) {
		std::cout << "Error while connecting." << std::endl;
		return false;
	}

	if (!login(CLIENT_NAME)) {
		std::cout << "Error while Game::login()." << std::endl;
		return false;
	}

	fcoord.open("coord.txt");
	fstatic.open("static.txt");
	fdynamic.open("dynamic.txt");
	log.open("log.txt");


	init_static_map();
	init_dynamic_map();
	return true;
}

bool Game::upgrade(std::list<Town> posts_, std::list<Train> trains_) {
	json jUpg;
	std::vector<uint32_t> postsIdx;
	for (auto post : posts_) {
		postsIdx.push_back(post.idx);
	}
	jUpg["post"] = postsIdx;

	std::vector<uint32_t> trainsIdx;
	for (auto train : trains_) {
		trainsIdx.push_back(train.idx);
	}
	jUpg["train"] = trainsIdx;

	std::string strUpg = jUpg.dump();

	ResponseMessage resp;
	ActionMessage act(Action::UPGRADE, strUpg);
	socket.make_move(act, resp);

	if (resp.resp_code != Response::OKEY) {
		return false;
	}

	return true;
}

bool Game::update() {
	ResponseMessage resp;
	get_layer_response(layer::DYNAMIC, resp);

	if (resp.resp_code != Response::OKEY) {
		return false;
	}

	json jMapResp = json::parse(resp.data);

	for (auto post : jMapResp.at("post")) {
		if (post["type"] == post_type::TOWN) {
			towns[post["idx"]].set(post["population"], post["product"]);
		}
		else if (post["type"] == post_type::MARKET) {
			markets[post["idx"]].set(post["product"]);
		}
	}

	for (auto train : jMapResp.at("train")) {
		trains[train["idx"]].set(train["line_idx"], train["position"], train["speed"], train["goods"]);
	}

	std::cout << ">> update" << std::endl;
	print_trains();
	print_markets();

	return true;
}

bool Game::login(std::string name) {
	json jLogin;
	jLogin["name"] = name;

	std::string strLogin = jLogin.dump();
	ActionMessage act(Action::LOGIN, strLogin);

	ResponseMessage resp;
	socket.make_move(act, resp);

	if (resp.data_length == 0) {
		return false;
	}

	std::cout << "login response:\n" << resp.data << std::endl << std::endl;

	json jLoginResp = json::parse(resp.data);
	get_login_response(jLoginResp);

	std::string current = ">> logged as \"" + name + "\".";
	std::cout << current << std::endl;
	return true;
}

bool Game::move(uint32_t line_idx, int speed, uint32_t train_idx) {
	json jMove({
		{ "line_idx", line_idx },
		{ "speed", speed },
		{ "train_idx", train_idx }
	});

	ActionMessage act = ActionMessage(Action::MOVE, jMove.dump());
	ResponseMessage resp;
	socket.make_move(act, resp);

	if (resp.resp_code != Response::OKEY) {
		return false;
	}
	std::cout << ">> move" << std::endl;
	return true;
}

bool Game::turn() {
	turn_number++;

	ActionMessage act = ActionMessage(Action::TURN, std::string("{}"));
	ResponseMessage resp;
	socket.make_move(act, resp);

	if (resp.resp_code != Response::OKEY) {
		return false;
	}

	std::cout << ">> turn " << turn_number << std::endl;
	return true;
}

void Game::get_login_response(json jLoginResp) {
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

int Game::end() {
	ActionMessage act = ActionMessage(Action::LOGOUT, std::string());
	ResponseMessage resp;
	socket.make_move(act, resp);

	std::cout << ">> logout" << std::endl;

	socket.Disconnect();
	return EXIT_SUCCESS;
}

void Game::print_trains() {
	std::cout << "trains:" << std::endl;
	for (auto tr : trains) {
		std::cout << tr.first << " - " << json(tr.second) << std::endl;
	}
}

void Game::print_markets() {
	std::cout << "markets:" << std::endl;
	for (auto mrk : markets) {
		std::cout << mrk.first << " - " << json(mrk.second) << std::endl;
	}
}

void Game::print_towns() {
	std::cout << "towns:" << std::endl;
	for (auto twn : towns) {
		std::cout << twn.first << " - " << json(twn.second) << std::endl;
	}
}

void Game::print_lines() {
	std::cout << "lines:" << std::endl;
	for (auto point : map) {
		std::cout << point.first << " - ";
		for (auto line : point.second) {
			if (line.direction == -1) std::cout << "-";
			std::cout << line.end << "(" << line.line_idx << ") ";
		}
		std::cout << std::endl;
	}
}

//SHOPPING PART-----------------------------------------------------------------

void Game::Dijkstra(const uint32_t &start)
{
	std::vector<uint32_t> path;
	path.resize(map.size() + 1);
	std::vector<std::vector<std::pair<int, int> > > edges;
	edges.resize(map.size() + 1);

	for (auto startpoint : map)
	{
		for (auto endpoint : startpoint.second)
		{
			edges[startpoint.first].push_back(std::make_pair(endpoint.end, endpoint.length));
		}
	}

	std::vector<int> dist;
	int n = (int)edges.size();
	dist.assign(n, 100000);
	dist[start] = 0;
	std::set<std::pair<int, int>> q;
	for (int i = 0; i < n; ++i)
	{
		q.insert(std::make_pair(dist[i], i));
	}
	// Главный цикл - пока есть необработанные вершины
	while (!q.empty())
	{
		// Достаем вершину с минимальным расстоянием
		std::pair<int, int> cur = *q.begin();
		q.erase(q.begin());
		// Проверяем всех ее соседей
		for (int i = 0; i < (int)edges[cur.second].size(); ++i)
		{
			// Делаем релаксацию
			if (dist[edges[cur.second][i].first] > cur.first + edges[cur.second][i].second)
			{
				q.erase(std::make_pair(dist[edges[cur.second][i].first], edges[cur.second][i].first));
				dist[edges[cur.second][i].first] = cur.first + edges[cur.second][i].second;
				path[edges[cur.second][i].first] = cur.second;
				q.insert(std::make_pair(dist[edges[cur.second][i].first], edges[cur.second][i].first));
			}
		}
	}

	for (uint32_t dest = 1; dest <= map.size(); ++dest)
	{
		std::vector<int> final_path;
		for (int node = dest; node != start; node = path[node])
			final_path.push_back(node);
		final_path.push_back(start);
		reverse(final_path.begin(), final_path.end());

		for (auto elem : final_path)
			min_pathes[start][dest][dist[dest]].push_back(elem);

	}
}

void Game::shopping(const std::vector<uint32_t> &points, const uint32_t &train_idx) {
	for (auto point = points.begin(); point != points.end() - 1; ++point) {
		for (auto endpoint : map[*point]) {
			if (endpoint.end == *(point + 1)) {
				for (int i = 0; i < endpoint.length; ++i) {
					move(endpoint.line_idx, endpoint.direction, train_idx);
					turn();
				}
			}
		}

	}
}

std::map<uint32_t, std::map<uint32_t, uint32_t>> Game::get_min_markets_pathes()
{
	std::map<uint32_t, std::map<uint32_t, uint32_t>> res;
	for (auto startpoint : min_pathes) {
		for (auto endpoint : startpoint.second) {
			for (auto elem : endpoint.second)
				res[startpoint.first][endpoint.first] = elem.first;
		}
	}
	return res;
}

std::map<uint32_t, uint32_t> Game::get_replenishments() {
	std::map<uint32_t, uint32_t> res;
	for (auto m : markets) {
		res[m.second.point_id] = m.second.replenishment;
	}
	return res;
}

std::map<uint32_t, uint32_t> Game::get_capacities() {
	std::map<uint32_t, uint32_t> res;
	for (auto m : markets) {
		res[m.second.point_id] = m.second.product_capacity;
	}
	return res;
}

std::map<uint32_t, uint32_t> Game::get_markets_product() {
	std::map<uint32_t, uint32_t> res;
	for (auto m : markets) {
		res[m.second.point_id] = m.second.product;
	}
	return res;
}

std::vector<uint32_t> Game::get_market_point_id() {
	std::vector<uint32_t> res;
	for (auto m : markets) {
		res.push_back(m.second.point_id);
	}
	return res;
}

std::vector<uint32_t> Game::get_full_path(std::vector<uint32_t> path)
{
	path.push_back(home.idx);
	path.insert(path.begin(), home.idx);
	std::vector<uint32_t> next_points;
	for (auto iter = path.begin(); iter != path.end() - 1; ++iter)
	{
		if (next_points.size())
			next_points.erase(next_points.end() - 1);
		for (auto points : min_pathes[*iter][*(iter + 1)])
		{
			for (auto point : points.second)
				next_points.emplace_back(point);
		}
	}
	return next_points;
}