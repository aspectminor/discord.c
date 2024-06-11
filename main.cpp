#include <iostream>
#include "api/request.h"
#include "api/parser.h"

int main() {
	auto result = fetch("GET", "http://calapi.inadiutorium.cz/api/v0/en/calendars/general-en/2021/03");

	std::vector<std::string> strings = {"date", "celebrations"};
	auto parsed = parse(result, strings);
	std::cout << parsed << std::endl;
}