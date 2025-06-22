#include <iostream>
#include <string>
#include <set>
#include <print>
#include <limits>
#include <string_view>
#include "utils.hpp"


int main() {
	std::print(1+R"(
Welcome to Set Manipulator 1.0(2024.12.19) by PavelPI.
Available commands (where x - int):
push x - add an element to the set (Ex. push 42)
delete x - delete an element from the set (Ex. delete 42)
)");

	using data_t = int;
	std::set<data_t> set;
	const auto print_set = [&set]() { std::println("Current set is {}", set); };
	while (std::cin.good()) {
		const std::string input = utils::request_type<std::string>("Type command:\n");
		if (input == "push") {
			const auto [it, inserted] = set.insert(utils::request_type<data_t>());
			if (!inserted) std::println("Error. This value already exists");
			print_set();
		}
		else if (input == "delete")
		{
			const bool del = set.erase(utils::request_type<data_t>());
			if (!del) std::println("Error. This value does not exist");
			print_set();
		}
		else std::println("Unkown command");
	}
}