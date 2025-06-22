#include <print>
#include <sstream>
#include <string>
#include <iterator>
#include <ranges>
#include <algorithm>
#include "utils.hpp"
//Лабораторная работа 8. Вариант 10. Задание 29
//Вывести текст, составленный из первых букв всех слов; все слова, содержащие больше одной буквы «s»
//Ex1. Lorem ipsum dolor sit amet, (swords1) consectetur adipiscing elit, sed (worss2) do eiusmod tempor incididunt ut labore (ss3) et dolore magna aliqua
int main() {
	std::print(1 + R"(
Welcome to Laboratory work 8, task 29 (2025.01.06) by PavelPI
Print the text made up of the first letters of all words;
all words containing more than one letter "s".
================================================================
)");

	std::istringstream iss(utils::request_type<utils::whole_line>("Enter a sentence: "));

	std::print("1. A string built with first letters is: ");

	auto words = ranges::subrange(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()) | 
		views::filter([](const std::string& s) {
			std::print("{}", s[0]); 
			return ranges::any_of(s, [cnt = 0](char c) mutable {return (cnt += (c == 's')) == 2;});
		});

	std::println("\n2. Words with two \"s\" is: {}", words);
}