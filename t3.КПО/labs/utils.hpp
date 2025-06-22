#pragma once
#include <iostream>
#include <ranges>
#include <iterator>
#include <filesystem>
#include <string>
#include <string_view>
#include <format>
#include <vector>
#include <type_traits>
#include <cstdlib>
#include <limits>
#include <random>
#include <cassert>
#include <array>
#include <algorithm>

#if defined _WIN32 || defined WIN32 || defined _WIN64
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#endif

namespace ranges = std::ranges;
namespace views = std::views;
namespace fs = std::filesystem;
using std::size_t;

#ifndef __cpp_lib_format_path
template <>
struct std::formatter<fs::path> : std::formatter<std::string_view> {
	using base_t = std::formatter<std::string_view>;
	auto format(const fs::path& path, auto& ctx) const {
		return base_t::format(reinterpret_cast<char const*>(path.u8string().data()), ctx);
	}
};
#endif

namespace utils {
	auto get_u8args(const int argc, char* argv[]) {
#if defined _WINDOWS_
		int num_args;
		LPWSTR* arg_list = CommandLineToArgvW(GetCommandLineW(), &num_args);
		std::vector<std::u8string> ret(num_args);

		const auto wstr_utf8 = [](LPCWSTR lpcwstr) {
			if (lpcwstr == NULL) return std::u8string{};
			std::u8string str(WideCharToMultiByte(CP_UTF8, 0, lpcwstr, -1, NULL, 0, NULL, NULL), u8'\0');
			WideCharToMultiByte(CP_UTF8, 0, lpcwstr, -1, reinterpret_cast<char*>(str.data()), str.size(), NULL, NULL);
			return str;
			};

		for (int i : views::iota(0, num_args))
			ret[i] = wstr_utf8(arg_list[i]);

		return ret;
#else
		//TODO
		//Is there anything productive we can do on other platforms? Dunno
		std::vector<std::u8string> ret(argc);
		std::ranges::generate(ret, [args = argv]() mutable {
			std::string_view ansi{ *args++ };
			std::u8string u8(ansi.size(), u8'\0');
			ranges::transform(ansi, u8.begin(), [](char c)->char8_t {return c;});
			return u8;
			});
		return ret;
		/*std::vector<std::string> ret(argc);
		  std::ranges::generate(ret, [args = argv]() mutable {return *args++;});
		  return ret;*/
#endif

	}

	struct whole_line {
		std::string line;
		friend auto& operator>>(std::istream& is, whole_line& v) {
			return std::getline(is, v.line);
		}
		constexpr std::string operator()() { return std::move(line); }
	};
	struct yesbool {
		bool accepted;
		constexpr static std::array<std::string_view, 4> yes_ans{"", "y", "ye","yes" };
		constexpr static auto buf_size = ranges::max(yesbool::yes_ans, {}, ranges::size).size();

		friend auto& operator>>(std::istream& is, yesbool& v) {
			std::array<char, buf_size + 1> str;//+1 null term

			is.getline(str.data(), str.size());
			if (!is) return is;
			const auto readed = is.gcount() - (is.gcount() > 0 && is.good());
			auto ans = ranges::subrange{str.begin(), str.begin() + readed };
			ranges::transform(ans, ranges::begin(ans), [](unsigned char c) {return std::tolower(c);});

			v.accepted = ranges::contains(yesbool::yes_ans, std::string_view{ ans });

			return is;
		}
		constexpr bool operator()() { return accepted; }
	};
	//https://stackoverflow.com/questions/23202022/shell-user-prompt-y-n

	template<class T> struct is_requested:std::bool_constant<false> {};
	template<> struct is_requested<whole_line>:std::bool_constant<true> {};
	template<> struct is_requested<yesbool> :std::bool_constant<true> {};

	template <class Type, class RestrictFunc = decltype([](const auto&) {return true;}) >
	auto request_type(const std::string_view welcome = "", RestrictFunc restrictf = {}, const std::string_view err_str = "Error, please try again\n")
	{
		using std::cin, std::cout;
		Type v{};
		while (cin.good() && cout.good()) {
			cout << welcome;
			cin >> v;
			if (cin.bad()) break;
			bool ok;
			if (cin.fail()) {
				cout << err_str;
				cin.clear();
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				ok = false;
			}
			else {
				ok = std::invoke(restrictf, v);
			}
			if (ok) {
				if constexpr (is_requested<std::decay_t<Type>>())
					return v();
				 else
					return v;
			}
		}
		std::exit(7321);
	}

	template<size_t N>
	struct str_literal {
		std::array<char, N> stor;
		consteval std::string_view sv() const { return { stor.data(), N - 1 }; }
		consteval operator std::string_view() const { return sv(); }
		consteval str_literal(const char(&str)[N]) {
			ranges::copy_n(ranges::begin(str), N, ranges::begin(stor));
		}
	};

	template <str_literal Sep, str_literal Open, str_literal Close, ranges::input_range Rng>
	struct joiner {
		constexpr static auto sep = Sep;
		constexpr static auto open = Open;
		constexpr static auto close = Close;
		Rng rng;
	};

	//example: std::print("{}", utils::make_joiner<"->">(std::vector{1,2,3}));//[1->2->3]
	template <str_literal Sep = ", ", str_literal Open = "[", str_literal Close = "]", std::ranges::input_range Rng>
	auto make_joiner(Rng&& rng) {
		//TODO не будет ли необязательного копирования для диапазонов с |
		return joiner<Sep, Open, Close, Rng>{ std::forward<Rng>(rng) };
	}
	template <str_literal Open = "[", str_literal Close = "]">
	auto space_joiner(std::ranges::input_range auto&& rng) { return make_joiner<" ", Open, Close>(std::forward<decltype(rng)>(rng)); }

	template <str_literal Sep, str_literal Open, str_literal Close, ranges::input_range R>
	struct std::formatter<joiner<Sep, Open, Close, R>> : range_formatter<ranges::range_value_t<R>> {
		using joiner_t = utils::joiner<Sep, Open, Close, R>;
		using base_t = range_formatter<ranges::range_value_t<R>>;
		constexpr formatter() {
			base_t::set_separator(joiner_t::sep);
			base_t::set_brackets(joiner_t::open, joiner_t::close);
		}
		auto format(const joiner_t& joiner, auto& ctx) const {
			return base_t::format(joiner.rng, ctx);
		}
	};

	template <class T, class... Ts>
	concept is_any_same = std::disjunction<std::is_same<T, Ts>...>::value;

	template<class T>
	concept uniform_int_input = is_any_same<T,
		short, int, long, long long,
		unsigned short, unsigned int, unsigned long, unsigned long long>;

	template<class T>
	concept uniform_real_input = is_any_same<T, float, double, long double>;

	template<std::uniform_random_bit_generator TEngine = std::default_random_engine>
	class random :private TEngine {
		using max_type = unsigned long long;//max type of support by uniform_int_distibution
		constexpr static auto max_type_digs = std::numeric_limits<max_type>::digits;
	public:
		using engine_t = TEngine;
	private:
		engine_t& engine() noexcept {
			return *this;
		}
		template<class TDistr>
		auto any_next(auto&&... prms) {
			thread_local TDistr distr{};
			return distr(engine(), typename TDistr::param_type{ std::forward<decltype(prms)>(prms)... });
		}
	public:
		using engine_t::engine_t;

		template<class TDistr>
		auto any_generator(auto&&... prms) {
			return [this, distr = TDistr{ std::forward<decltype(prms)>(prms)... }]() mutable {return distr(this->engine());};
		}

		//closed interval [a,b]
		template<uniform_int_input T>
		T next(T a, T b) {
			assert(a <= b);
			return any_next<std::uniform_int_distribution<T>>(a, b);
		}
		template<uniform_int_input T>
		auto generator(T a, T b) {
			assert(a <= b);
			return any_generator<std::uniform_int_distribution<T>>(a, b);
		}

		//semiopen interval [a,b)
		template<uniform_real_input T>
		T next(T a, T b) {
			assert(a < b);
			return any_next<std::uniform_real_distribution<T>>(a, b);
		}
		template<uniform_real_input T>
		auto generator(T a, T b) {
			assert(a < b);
			return any_generator<std::uniform_real_distribution<T>>(a, b);
		}

		//closed interval [0,b]
		template<std::integral T> requires uniform_int_input<T>
		T next(T b = std::numeric_limits<T>::max()) {
			return next<T>(static_cast<T>(0), b);
		}
		//closed interval [0,b]
		template<std::integral T> requires uniform_int_input<T>
		auto generator(T b = std::numeric_limits<T>::max()) {
			return generator<T>(static_cast<T>(0), b);
		}

		//completely fills the integral type with random bits
		template<std::integral T> requires (!uniform_int_input<T>)
			T next() {
			const auto val = get_bits<sizeof(T) * std::numeric_limits<T>::digits>();
			if constexpr (sizeof(T) <= sizeof(unsigned long)) return static_cast<T>(val.to_ulong());
			else return static_cast<T>(val.to_ullong());
		}
		//semiopen interval [0,1)
		template<std::floating_point T>
		T next() {
			return next<T>(static_cast<T>(0), static_cast<T>(1));
		}
		template<std::floating_point T>
		auto generator() {
			return generator<T>(static_cast<T>(0), static_cast<T>(1));
		}
		bool operator ==(const random&) const = default;
	};

	template<class SeedFunc = std::random_device, std::uniform_random_bit_generator TEngine = std::default_random_engine>
	auto& thread_random() {
		thread_local random<TEngine> rnd{ std::invoke(SeedFunc{}) };
		return rnd;
	}

	template<size_t seed = 0, std::uniform_random_bit_generator TEngine = std::default_random_engine>
	auto& stable_random() {
		return thread_random<decltype([]() {return seed;}), TEngine > ();
	}
}