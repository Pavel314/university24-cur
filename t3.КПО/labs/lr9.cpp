#include <cstddef>
#include <array>
#include <ranges>
#include <algorithm>
#include <istream>
#include <cctype>
#include <cstdint>
#include <print>
#include <optional>
#include "utils.hpp"

//Лабораторная работа 9. Вариант 10. Задание 10
/*
1. Описать структуру с именем ORDER, содержащую следующие поля:
a) PLAT – расчетный счет плательщика;
b) POL – расчетный счет получателя;
c) SUMMA – перечисляемая сумма в руб.
2. Написать программу, выполняющую следующие действия:
a) ввод с клавиатуры данных в массив SPISOK, состоящий из восьми
элементов типа ORDER; записи должны быть размещены в алфавитном порядке по
расчетным счетам плательщиков;
b) вывод на экран информации о сумме, снятой с расчетного счета
плательщика, введенного с клавиатуры;
c) если такого расчетного счета нет, выдать на дисплей соответствующее
сообщение.
*/

struct bank_account {
	constexpr static size_t length = 3;
	std::array<char, length> data;

	constexpr bank_account() noexcept { ranges::fill(data, '0'); }
	template<size_t N> requires (N == length+1)//Null Term
	constexpr bank_account(const char(&str)[N]) noexcept {
		ranges::copy_n(str, length, ranges::begin(data));
	}

	friend auto& operator>>(std::istream& is, bank_account& acc) {
		using sentry_t = std::istream::sentry;
		if (!sentry_t{ is }) return is;

		for (char& c : acc.data) {
			const int peek = is.peek();
			if (!std::isdigit(peek)) {
				is.setstate(std::ios::failbit);
				break;
			}
			c = static_cast<char>(peek);
			is.ignore();
		}

		return is;
	}
	auto operator<=>(const bank_account&) const = default;
};

struct order {
	bank_account plat;
	bank_account pol;
	std::intmax_t summa;

	void interactive_read() {
		plat = utils::request_type<bank_account>("plat: ");
		pol = utils::request_type<bank_account>("pol: ");
		summa = utils::request_type<std::intmax_t>("summa: ");
	}

	constexpr static auto plat_less = [](const auto& l, const auto& r) {return l.plat < r.plat;};
};

template<>
struct std::formatter<order> {
	constexpr auto parse(auto& ctx) {return ctx.begin();}

	constexpr auto format(const order& ord, auto& ctx) const {
		return std::format_to(ctx.out(), "[plat: {:s}, pol: {:s}, summa: {}]", ord.plat.data, ord.pol.data, ord.summa);
	}
};

int main() {
	std::print(1 + R"(
Welcome to Laboratory work 9, task 36 (2025.01.07) by PavelPI
We have an ORDER structure with the fields PLAT, POL, SUMMA,
where PLAN and PLOT are the bank account of the payer and the payee and SUMMA is the number of rubles.
Note: A bank account is defined by {} digits.
================================================================
)", bank_account::length);

	std::array<order, 8> spisok{};
	
	std::println("Input {} orders:", spisok.size());
	
	ranges::for_each(spisok, [ind = 0](order& ord) mutable {std::println("Order #{}:", ++ind); ord.interactive_read(); std::println("");});
	
	ranges::sort(spisok, order::plat_less);
	std::println("After sorting by PLAT:\n{:n}", utils::make_joiner<"\n">(spisok));

	const bank_account plat_acc = utils::request_type<bank_account>("Enter the payer's bank account: ");
	//const bank_account plat_acc = {"123"};

	std::optional<std::intmax_t> sum;
	for (const order& ord : spisok) {
		if (ord.plat == plat_acc)
			sum = (sum?*sum:0)+ ord.summa;
	}

	if (sum)
		std::println("The amount spent from this account: {}", *sum);
	else
		std::println("This bank account was not found");
}
//https://stackoverflow.com/questions/24879417/avoiding-the-first-newline-in-a-c11-raw-string-literal
/*
Order #1:
plat: 231
pol: 333
summa: 1
Order #2:
plat: 231
pol: 333
summa: 3
Order #3:
plat: 231
pol: 444
summa: 5
Order #4:
plat: 2
Error, please try again
plat: 222
pol: 333
summa: 4
Order #5:
plat: 444
pol: 555
summa: 6
Order #6:
plat: 777
pol: 888
summa: 9
Order #7:
plat: 333
pol: 444
summa: 3
Order #8:
plat: 231
pol: 222
summa: 0
*/