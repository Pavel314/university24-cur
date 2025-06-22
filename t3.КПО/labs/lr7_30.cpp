#include <print>
#include <vector>
#include <mdspan>
#include <array>
#include <cstddef>
#include <algorithm>
#include "utils.hpp"
//Лабораторная работа 7. Вариант 10. Задание 30.
//Дана квадратная матрица порядка n. Составить программу вычисления
//количества положительных элементов в нижнем левом треугольнике, включая диагональные элементы.

//C++23: use multidimensional subscript
//C++26: use mdarray
int main() {
	std::print(1 + R"(
Welcome to Laboratory work 7, task 30 (2025.01.05) by PavelPI
Goal: for a square matrix of size nxn, find the number of positive elements on the main diagonal and below
================================================================
)");

	const int n = utils::request_type<int>("Enter the order of the square matrix(0<n<10): ", [](int n) {return n > 0 && n < 10;});
	const int size = n * n;
	std::vector<int> storage(size);
	ranges::generate(storage, utils::thread_random().generator<int>(-size, size));

	const auto matr = std::mdspan(storage.data(), n, n);
	using ind = std::array<size_t, 2>;
	size_t poss = 0;

	for (size_t y : views::iota(0ULL, matr.extent(1)))
	{
		for (size_t x : views::iota(0ULL, matr.extent(0)))
		{
			if (x <= y && matr[ind{x, y}] > 0) {
				++poss;
			}
			std::print("{:3} ", matr[ind{x,y}]);			
		}
		std::println("");
	}
	std::println("Count of positive elements on the main diagonal and below: {}", poss);
}