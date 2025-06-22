#include <print>
#include <string>
#include <algorithm>
#include <utility>
#include "utils.hpp"
//Лабораторная работа 8. Вариант 10. Задание 20
//Удалить в строке все лишние пробелы, то есть серии подряд идущих пробелов заменить на одиночные пробелы. 
//Крайние пробелы в строке удалить.
//Ex1.        Lorem    ipsum     dolor  sit         amet, consectetur    adipiscing                 elit, sed do     eiusmod tempor incididunt ut     labore et dolore  magna aliqua      
int main() {
	std::print(1 + R"(
Welcome to Laboratory work 8, task 20 (2025.01.05) by PavelPI
Goal: Delete all unnecessary spaces in a line, that is, replace consecutive series of spaces with single spaces. 
Delete the last spaces in the line.
================================================================
)");
	std::string str = utils::request_type<utils::whole_line>("Enter a string with spaces: ");

	const auto [first, last] = std::ranges::remove_if(str, [p = ' '](char c) mutable {
		return std::exchange(p, c) == c && c == ' ';
	});
	str.erase(first, last);
	if (!str.empty() && str.back() == ' ') str.pop_back();
	std::println("Result is: {:?}", str);
}