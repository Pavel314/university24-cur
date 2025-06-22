#include <print>
#include <array>
#include <algorithm>
#include "utils.hpp"
//Лабораторная работа 8. Вариант 10. Задание 4
//Сформировать строку из 10 символов. На четных позициях должны находится четные цифры, на нечетных позициях - буквы.

int main() {
	std::print(1 + R"(
Welcome to Laboratory work 8, task 4 (2025.01.05) by PavelPI
Goal: build a string with even digits in even positions, otherwise - letters.
================================================================
)");

	//std::string str(10, '.');
	std::array<char, 10> str{};

	auto& rnd = utils::thread_random();
	ranges::generate(str, [&rnd, even = true]()  mutable -> char  { return (even=!even) ?  rnd.next<short>('a', 'z') : ('0' + 2 * rnd.next<short>(0, 4)); });

	std::println("Result is: {:s}", str);
}