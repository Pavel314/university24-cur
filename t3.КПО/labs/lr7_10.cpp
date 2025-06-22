#include <print>
#include <array>
#include <algorithm>
#include <iterator>
#include "utils.hpp"
//Лабораторная работа 7. Вариант 10. Задание 10.
//10. В заданном массиве поменять местами наибольший и наименьший элементы

int main() {
	std::print(1 + R"(
Welcome to Laboratory work 7, task 10 (2025.01.04) by PavelPI
Goal: swap the minimum element with the maximum element in the array
Note: If there are several minima in the array, the first one will be selected.
If there are multiple maxima in the array, the last one will be selected.
================================================================
)");
	std::array<int, 10> arr;
	ranges::generate(arr, utils::thread_random().generator<int>(0, 2*arr.size()));
	std::println("Input data:\n{}", utils::space_joiner(arr));
	const auto [min, max]= ranges::minmax_element(arr);
	std::println("min: {}, max: {}", *min, *max);
	ranges::iter_swap(min, max);
	std::println("After swapping min with max:\n{}", utils::space_joiner(arr));
}