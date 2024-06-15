#include <iostream>
#include "api/request.h"
#include "api/parser.h"

int main() {
	auto result = fetch("GET", "http://calapi.inadiutorium.cz/api/v0/en/calendars/general-en/2021/03");

	std::vector<std::string> strings = {"date", "celebrations"};
	auto parsed = parse(result, strings);
	for (auto& [k, v] : parsed) {
		std::cout << k << ": " << v << std::endl;
	}
	
	return 0;
}