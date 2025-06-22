#include <print>
#include <array>
#include <algorithm>
#include <ranges>
#include <functional>
#include "utils.hpp"
//Лабораторная работа 7. Вариант 10. Задание 20.
//Задан одномерный массив А[1..15].
//Определить сумму четных положительных элементов массива с n - го по k - й.

int main() {
	constexpr int size = 15;

	std::print(1 + R"(
Welcome to Laboratory work 7, task 20 (2025.01.04) by PavelPI
Goal: Determine the sum of even positive elements of the array from n to k, array length is {}
================================================================
)", size);
	auto& rnd = utils::thread_random();

	std::array<int, size> arr;
	ranges::generate(arr, rnd.generator(-size, size));
	const auto n = rnd.next(0, size - 1);
	const auto k = rnd.next(n, size - 1);
	
	std::println("Input data:\n{}", utils::space_joiner(arr));

	const auto beg = ranges::begin(arr);
	const auto poss_even = [](int i) { return (i>0) && (i % 2 == 0); };
	const int sum = ranges::fold_left(ranges::subrange(beg+n, beg+k+1) | views::filter(poss_even), 0, std::plus<>{});
	std::println("starting: {}, ending: {}, sum: {}",n,k, sum);
}