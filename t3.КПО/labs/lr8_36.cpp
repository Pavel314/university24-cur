#include <iterator>
#include <ranges>
#include <algorithm>
#include <print>
#include <string>
#include <cctype>
#include "utils.hpp"	
//Лабораторная работа 8. Вариант 10. Задание 36
//Даны две строки. Выделить из каждой строки наибольшей длины подстроки, 
//состоящие только из цифр, и объедините эти подстроки в одну новую строку.
/*
Ex1.
Str1: Lorem11 ipsum22 3333dolor s4567it a12345met, con12345678sectetur adipiscing elit
Str2: sed do eiusmod 271828tempor i3141592ncididunt ut labore et dolore magna aliqua
*/

template<std::input_iterator S, std::sentinel_for<S> I, std::indirect_unary_predicate<S> Pred>
constexpr auto next_seq(S fst, I lst, Pred pred) {
	fst = ranges::find_if(fst, lst, std::ref(pred));
	lst = ranges::find_if_not(fst, lst, std::ref(pred));
	return ranges::subrange(fst, lst);
}

template<ranges::random_access_range Rng >
constexpr std::ranges::borrowed_subrange_t<Rng> find_max_seq(Rng&& rng, auto pred) {
	const auto end = ranges::end(rng);
	auto max_rng = ranges::subrange(end, end);
	for (auto it = ranges::begin(rng); it != end;) {
		auto seq = next_seq(it, end, std::ref(pred));
		it = ranges::end(seq);
		if (ranges::size(seq) > ranges::size(max_rng)) max_rng = seq;
	}
	return max_rng;

}
int main() {
	std::print(1 + R"(
Welcome to Laboratory work 8, task 36 (2025.01.06) by PavelPI
Two lines are given.
Select the substrings consisting only of digits from each string of the longest length and combine these substrings into one new one.
================================================================
)");

	const std::string str1 = utils::request_type<utils::whole_line>("Enter the first line(with digits): ");
	const std::string str2 = utils::request_type<utils::whole_line>("Enter the second line(with digits): ");
	const auto dig = [](unsigned char c) {return std::isdigit(c);};

	const auto seq1 = find_max_seq(str1, dig);
	std::println("Maximum substring #1 consisting of digits: {:s}", seq1);

	const auto seq2 = find_max_seq(str2, dig);
	std::println("Maximum substring #2 consisting of digits: {:s}", seq2);

	//C++26: std::ranges::views::concat
	const std::string sum = ranges::to<std::string>(seq1).append_range(seq2);
	std::println("Concat of these substrings: {}", sum);
}