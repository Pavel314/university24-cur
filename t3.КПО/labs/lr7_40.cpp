#include <print>
#include <array>
#include <algorithm>
#include <mdspan>
#include <cstddef>
#include "utils.hpp"
//Лабораторная работа 7. Вариант 10. Задание 40.
//Вывести на экран матрицу 4x5.Определить номера столбцов, содержащих более половины положительных элементов

//C++23: use multidimensional subscript
//C++26: use mdarray
int main() {
	constexpr int width = 5;
	constexpr int height = 4;
	constexpr int size = width * height;

	std::print(1 + R"(
Welcome to Laboratory work 7, task 40 (2025.01.05) by PavelPI
Goal: Display a {}x{} matrix on the terminal.
Determine the column numbers containing more than half of the positive elements
================================================================
)", height, width);

	std::array<int, size> storage{};
	ranges::generate(storage, utils::thread_random().generator<int>(-size, size));

	const auto matr = std::mdspan(storage.data(), width, height);
	using ind = std::array<size_t, 2>;

	for (size_t y : views::iota(0ULL, matr.extent(1)))
	{
		for (size_t x : views::iota(0ULL, matr.extent(0)))
		{
			std::print("{:3} ", matr[ind{ x,y }]);
		}
		std::println("");
	}

	std::print("Column indexes where the number of positives is more than half the count of rows: ");

	for (size_t x : views::iota(0ULL, matr.extent(0)))
	{
		size_t poss = 0;
		for (size_t y : views::iota(0ULL, matr.extent(1)))
		{
			if (matr[ind{ x,y }] > 0) 
				++poss;
		}
		if (poss > height / 2) {
			std::print("{} ", x);
		}
	}
}