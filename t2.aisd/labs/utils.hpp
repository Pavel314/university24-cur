#pragma once

#include <bit>
#include <iomanip>
#include <iostream>
#include <format>
#include <set>
#include <sstream>
#include <list>
#include <locale>

#include <string_view>
#include <array>

#include <fstream>
#include <random>
#include <concepts>

#include <bitset>
#include <cassert>
#include <print>
#include <ranges>
#include <vector>
#include <algorithm>
#include <limits>
#include <optional>
#include <numeric>
#include <memory>
#include <queue>
#include <stack>
#include <unordered_map>
#include <chrono>
#include <ratio>

//TODO перепроверить заголовки в каждом файле проекта
//#include <spanstream>

#if defined _WIN32 || defined WIN32 || defined _WIN64
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace ranges = std::ranges;

namespace std_ext {

	template<class T, class U>
	constexpr decltype(auto) rvmax(T&& a, U&& b) {
		return (a < b) ? std::forward<U>(b) : std::forward<T>(a);
	}

	constexpr void first_foreach(std::ranges::input_range auto&& rng, auto&& first, auto&& after) {
		auto beg = std::ranges::begin(rng);
		auto end = std::ranges::end(rng);
		if (beg != end)
		{
			std::invoke(std::forward<decltype(first)>(first), *beg++);
			std::ranges::for_each(beg, end, std::forward<decltype(after)>(after));
		}
	}

	struct discard { constexpr void operator()(auto&&...) const noexcept {} };

	template <class T, class... Ts>
	concept is_any_same = std::disjunction<std::is_same<T, Ts>...>::value;

	template<std::intmax_t num, std::intmax_t den, bool up>
	struct ratio_t:std::ratio<num, den> {
		friend constexpr auto& operator*=(std::integral auto& n, ratio_t r) noexcept {
			//TODO improve algorithm, see https://stackoverflow.com/questions/57300788/fast-method-to-multiply-integer-by-proper-fraction-without-floats-or-overflow
			//assert(n <= std::numeric_limits<decltype(num)>::max() / num);
			if constexpr (!up)
				return n = n * num / den;
			else
				return n = (n * num + den - 1) / den;
		}

		friend constexpr auto operator/(ratio_t r, std::integral auto n) noexcept {
			if constexpr (!up)
				return num / (n * den);
			else
				return (num + (n * den) - 1) / (n * den);
		}

		friend constexpr auto& operator/=(std::integral auto& n, ratio_t r) noexcept {return n *= ratio_t<den, num, up>{};}
		friend constexpr auto operator*(std::integral auto n, ratio_t r) noexcept {return n *= r;}
		friend constexpr auto operator*(ratio_t r, std::integral auto n) noexcept {return n * r;}
		friend constexpr auto operator/(std::integral auto n, ratio_t r) noexcept { return n /= r; }
	};

	template<bool up, std::intmax_t n, std::intmax_t d> consteval auto get_ratio() { return ratio_t<n,d, up>{}; }
	template<bool up, auto Rat > consteval auto get_ratio() { return ratio_t<Rat.num, Rat.den, up>{};}

	//template<std::intmax_t num, std::intmax_t den> constexpr ratio_t<num, den, false> ratio{};
	//template<std::intmax_t num, std::intmax_t den> constexpr ratio_t<num, den, true> up_ratio{};

	template<auto... Ts> constexpr auto ratio = get_ratio<false, Ts...>();
	template<auto... Ts> constexpr auto up_ratio = get_ratio<true, Ts...>();
	//template<std::intmax_t num, std::intmax_t den> constexpr ratio_t<num, den, true> up_ratio2{};

//	template<class Rat> constexpr auto cratio = raio<Rat::num, Rat::den>;
//	template<class Rat> constexpr auto cratio = up_ratio<Rat::num, Rat::den>;

	static_assert(5 * ratio <3, 2> == 7);
	static_assert(5 * up_ratio<ratio <3, 2>> == 8);
	static_assert(up_ratio < 3, 2> * 5 == 8);
	static_assert(5 / ratio <3, 2> == 3);
	static_assert(ratio <3, 2> / 5 == 0);
	static_assert(up_ratio <3, 2> / 5 == 1);
	static_assert(5 / up_ratio <3, 2> == 4);


}

template<template<class> class TNode>
struct binary_tree2 {
	class ptr;
	using node_t = TNode<ptr>;
	std::list<node_t> storage;

	class ptr : private decltype(storage)::iterator{
		using base_t = decltype(storage)::iterator;
		friend binary_tree2;
		bool is_null = false;
		ptr(const base_t& b) :base_t(b) {}
	public:
		ptr() :base_t() { is_null = true; }
		auto& operator *() { return base_t::operator*(); }
		auto* operator ->() { return base_t::operator->(); }
		auto const* operator ->() const { return base_t::operator->(); }
		//TODO если is_null true то не сравнивать итераторы, поскльку будет ошибка
		bool operator ==(const ptr&) const = default;//C++20: правило переписывания операторов сравнения
		explicit operator bool() const { return !is_null; }
	};

	const static inline ptr null_node{};

	ptr new_node(auto&&... args) {
		storage.emplace_front(std::forward<decltype(args)>(args)...);
		return storage.begin();
	}

	void delete_node(const ptr& n) { storage.erase(n); }
	//TODO операции копирования, перемещения требуют отдельной обработки этого поля
	ptr root = null_node;//controlled by user
};



namespace utils {
	class speed_counter {
		std::chrono::time_point<std::chrono::high_resolution_clock> run_point;
		static auto now() noexcept {
			return std::chrono::high_resolution_clock::now();
		}
	public:
		static speed_counter start_new() {
			speed_counter s{};
			s.start();
			return s;
		}

		void start() {
			run_point = now();
		}

		template<class duraction = std::chrono::duration<double>>
		auto stop() const noexcept {
			return std::chrono::duration_cast<duraction>(now() - run_point);
		}
	};

	template <class Type, class RestrictFunc = decltype([](const auto&) {return true;}) >
	Type request_type(const std::string_view welcome = "", RestrictFunc restrictf = {}, const std::string_view err_str = "Error, please try again\n")
	{
		Type v{};
		using std::cin, std::cout;
		while (cin.good()) {
			cout << welcome;
			cin >> v;
			bool ok;
			if (cin.bad()) std::exit(1);
			if (cin.fail()) {
				cout << err_str;
				cin.clear();
				//std::string ignore;
				//cin >> ignore;//Noe. We ignore all non-ws chars
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				ok = false;
			}
			else {
				ok = std::invoke(restrictf, v);
			}
			if (ok) return v;
		}
		std::exit(1);
	}
	/*struct parse_line_fn {
		template<class O, std::sentinel_for<O> S, class T = std::iter_value_t<O>> requires std::output_iterator<O, const T&>
		constexpr O operator()(O first, S last) const
		{

		}

		template<class R, class T = ranges::range_value_t<R>> requires ranges::output_range<R, const T&>
		constexpr ranges::borrowed_iterator_t<R> operator()(R&& r) const
		{
			return this->operator()(ranges::begin(r), ranges::end(r));
		}
	};
	inline parse_line_fn parse_line;

	template<class O, std::sentinel_for<O> S, class T = std::iter_value_t<O> >
	bool parse_line(std::string_view ln, O first, S last) {
		std::istringstream ss{ std::string(ln) };
		while (ss.good() && (ss >> std::ws).good()) { 
			std::iter_value_t<It> val;
			//ss >> val;
			//*dst++ = val;
		}
		return static_cast<bool>(ss);
	}*/

	std::optional<bool> enable_coloring() {
#if defined _WINDOWS_
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE) return false;
		DWORD dwMode = 0;
		if (!GetConsoleMode(hOut, &dwMode)) return false;
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;//ENABLE_PROCESSED_OUTPUT |
		if (!SetConsoleMode(hOut, dwMode)) return false; else return true;
#else
		return std::nullopt;//We have no information about coloring output support
#endif
	}

	template<std::size_t N>
	struct str_literal {
		std::array<char, N> stor;
		consteval std::string_view sv() const { return { stor.data(), N - 1 }; }
		consteval operator std::string_view() const { return sv(); }
		consteval str_literal(const char(&str)[N]) {
			std::copy_n(std::begin(str), N, std::begin(stor));
		}
	};

	template <str_literal Sep, str_literal Open, str_literal Close, std::ranges::input_range Rng>
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
	auto space_joiner(std::ranges::input_range auto&& rng) {return make_joiner<" ", Open, Close>(std::forward<decltype(rng)>(rng)); }

	template <str_literal Sep, str_literal Open, str_literal Close, std::ranges::input_range R>
	struct std::formatter<joiner<Sep, Open, Close, R>> : range_formatter<std::ranges::range_value_t<R>> {
		using joiner_t = utils::joiner<Sep, Open, Close, R>;
		using base_t = range_formatter<std::ranges::range_value_t<R>>;

		constexpr formatter() {
			base_t::set_separator(joiner_t::sep);
			base_t::set_brackets(joiner_t::open, joiner_t::close);
		}

		auto format(const joiner_t& joiner, std::format_context& ctx) const {
			return base_t::format(joiner.rng, ctx);
		}
	};


	template<std::uniform_random_bit_generator TEngine, class TDistrib>
	struct randomizer {
		using engine_t = TEngine;//Constructible by E(), E(result_type), E(SeedSequence), E(RandomNumberEngine)
		using distrib_t = TDistrib;//Constructible by D(), D(param_type), CopyConstructible, CopyAssignable 
		using eng_result_t = engine_t::result_type;//unsigned integer type
		using dist_result_t = distrib_t::result_type;//arithmetic type
		using param_t = distrib_t::param_type;//anything (CopyConstructible, CopyAssignable, EqualityComparable)

		engine_t engine;
		distrib_t distrib;

		static const param_t& make_param(const param_t& p) { return p; }
		static param_t make_param(auto&&... prms) { return param_t{ std::forward<decltype(prms)>(prms)... }; }

		template<class T = eng_result_t> requires (!std::same_as<std::decay_t<T>, randomizer>) //enable implicit defined copy constructor
			randomizer(T&& eng = 0, auto&&... prms)
			: engine(std::forward<T>(eng)), distrib(make_param(std::forward<decltype(prms)>(prms)...)) {
		}

		dist_result_t operator()() { return distrib(engine); }
		dist_result_t operator()(auto&&... prms) { return distrib(engine, make_param(std::forward<decltype(prms)>(prms)...)); }

		bool operator ==(const randomizer&) const = default;

		void seed() { engine.seed(); }
		void seed(eng_result_t v) { engine.seed(v); }
		void seed(auto& seq) { engine.seed(seq); }

		param_t param() { return distrib.param(); }
		void param(auto&&... prms) { param(make_param(std::forward<decltype(prms)>(prms)...)); }//TODO rec?
	};

	template<class T>
	concept uniform_int_input = std_ext::is_any_same<T,
		short, int, long, long long,
		unsigned short, unsigned int, unsigned long, unsigned long long>;

	template<class T>
	concept uniform_real_input = std_ext::is_any_same<T, float, double, long double>;

	template<std::uniform_random_bit_generator TEngine = std::default_random_engine>
	class random:private TEngine {
		using max_type = unsigned long long;//max type of support by uniform_int_distibution
		constexpr static auto max_type_digs = std::numeric_limits<max_type>::digits;
		
	public:
		using engine_t = TEngine;
	private:
		engine_t& engine() noexcept{
			return *this;
		}
		template<class TDistr>
		auto any_next(auto&&... prms) {
			thread_local TDistr distr{};
			return distr(engine(), typename TDistr::param_type{ std::forward<decltype(prms)>(prms)... });
		}

		max_type bits_val = 0;
		unsigned bits_avail = 0;
	public:
		using engine_t::engine_t;
		
		template<class TDistr>
		auto any_generator(auto&&... prms) {
			//Note: To avoid dangling reference we are capturing this by value(*this)
			//UPD|2024.12.27|Capturing by value causes the engine to be copied, the values will be the same.
			//So, we capture by reference(this), which may cause a dangling ref.
			/*std::array<int, 10> arr;
			std::ranges::generate(arr, utils::stable_random().generator(0, 1000));
			std::println("{}", arr);
			std::ranges::generate(arr, utils::stable_random().generator(0, 1000));
			std::println("{}", arr);*/
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


		template<unsigned N>
		std::bitset<N> get_bits() requires(N <= max_type_digs) {
			max_type ret = 0;
			if (bits_avail == 0) {
				ret = next<max_type>(0, std::numeric_limits<max_type>::max());
				bits_val = ret >> N;
				bits_avail = max_type_digs - N;
			}
			else if (bits_avail >= N) {
				ret = bits_val;
				bits_val >>= N;
				bits_avail -= N;
			}
			else {
				const auto tail = N - bits_avail;
				ret = bits_val;
				bits_val = next<max_type>(0, std::numeric_limits<max_type>::max());
				const max_type mask = (static_cast<max_type>(1) << tail) - 1U;
				ret = (ret << tail) | (bits_val & mask);
				bits_val >>= tail;
				bits_avail = max_type_digs - tail;
			}
			return ret;
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
		T next(T b = std::numeric_limits<T>::max())  {
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
			if constexpr(sizeof(T)<=sizeof(unsigned long)) return static_cast<T>(val.to_ulong());
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


		template<class It, class Sent>
		auto shuffle(It first, Sent last) {
			return std::ranges::shuffle(first, last, engine());
		}
		template<class Rng>
		auto shuffle(Rng&& rng) {
			return std::ranges::shuffle(std::forward<Rng>(rng), engine());
		}

		template<ranges::random_access_range Rng> requires std::permutable<ranges::iterator_t<Rng>>
		void shuffle_n(Rng&& rng, std::ranges::range_size_t<Rng> n) {
			const auto first = std::ranges::begin(rng);
			auto sz{ std::ranges::size(rng) };
			if (n > sz) return;
			for (--sz; n-->0; --sz)
				std::ranges::iter_swap(first + sz, first + next<decltype(n)>(0, sz));
		}

		//Note: В сравнение будут участвовать bits_val и bits_avail, что возможно не совсем ожидаемо
		bool operator ==(const random&) const = default;
	};

	template<class SeedFunc = std::random_device, std::uniform_random_bit_generator TEngine = std::default_random_engine>
	auto& thread_random() {
		thread_local random<TEngine> rnd{ std::invoke(SeedFunc{}) };
		return rnd;
	}

	template<std::size_t seed = 0, std::uniform_random_bit_generator TEngine = std::default_random_engine>
	auto& stable_random() {
		return thread_random<decltype([]() {return seed;}), TEngine>();
	}
	//std::random_device
	//default_random_engine RandomNumberEngine  https://en.cppreference.com/w/cpp/named_req/RandomNumberEngine
	//RandomNumberDistribution  https://en.cppreference.com/w/cpp/named_req/RandomNumberDistribution
	//SeedSequence https://en.cppreference.com/w/cpp/named_req/SeedSequence
}







namespace pretty_tree {
	enum class node_type :unsigned char { leaf = 0, left = 0b01, right = 0b10, both = left | right };

	constexpr node_type operator |(node_type l, node_type r) noexcept {
		return static_cast<node_type>(std::to_underlying(l) | std::to_underlying(r));
	}
	constexpr node_type& operator|=(node_type& l, node_type r) noexcept {
		return l = l | r;
	}

	struct text_span {
		using num_t = std::size_t;
		constexpr static num_t npos = static_cast<num_t>(-1);
		num_t start_pos = npos;
		num_t end_pos = npos;
		constexpr auto size() const noexcept {
			return end_pos - start_pos + 1;
		}
	};

	enum class ansi_colors :unsigned char { empty = 0, black=30, red = 31, green = 32, yellow = 33, blue = 34, meganta=35, cyan = 36, white=37 };
	struct ansi_color {
		ansi_colors val;
		bool is_underline;

		constexpr ansi_color(ansi_colors val = ansi_colors::empty, bool is_underline = false):val(val), is_underline(is_underline) {}

		auto write(auto it, auto&& data) const {
			const bool is_filled = val != ansi_colors::empty || is_underline;

			if (is_filled)
			{
				if (is_underline)
				{
					if (val != ansi_colors::empty) std::format_to(it, "\033[{};4m", std::to_underlying(val));
					else std::format_to(it, "\033[4m");
				} else 
					std::format_to(it, "\033[{}m", std::to_underlying(val));
			}

			std::invoke(std::forward<decltype(data)>(data));

			if (is_filled)
				std::ranges::copy(std::string_view{ "\033[0m" }, it);
			return it;
		}
	};

	template<class Node>
	struct tree_interface {
		using node_t = Node;

		template<class T>
		static consteval bool is_wrappered() {
			using D = std::decay_t<T>;
			if constexpr (std::is_pointer_v<D>) return true;
			else if constexpr (requires {std::enable_if_t<std::is_same_v<D, std::optional<typename D::value_type>>, int>{};}) return true;
			else if constexpr (requires { std::declval<D>().operator->(); }) return true;
			return false;
		}

		constexpr static auto r = std::conditional_t < is_wrappered<node_t>(), std::identity, decltype([](auto&& v) {return std::addressof(v);}) > {};
		static constexpr bool exists(node_t n) {
			//TODO подумать над переход к оператру operator!
			//static_assert(std::constructible_from<bool, node_t>);
			return static_cast<bool>(n);
		}

		static constexpr decltype(auto) left(node_t n) {
			if constexpr (requires { r(n)->left(); }) return r(n)->left(); else return r(n)->left;
		}

		static constexpr decltype(auto) right(node_t n) {
			if constexpr (requires { r(n)->right(); }) return r(n)->right(); else return r(n)->right;
		}

		static constexpr decltype(auto) value(node_t n) {
			if constexpr (requires { r(n)->value(); }) return r(n)->value(); else return r(n)->value;
		}

		static constexpr std::optional<unsigned> value_fsize(node_t n) {
			if constexpr (requires { r(n)->value_fsize(); })
				return r(n)->value_fsize();
			else if constexpr (requires { r(n)->value_fsize; })
				return r(n)->value_fsize;
			else
				return std::nullopt;
		}
	};

	struct node_info {
		bool is_right;
		unsigned depth;
		node_type type = node_type::leaf;
		text_span span;
	};

	enum class branches { left, right, empty };
	struct plain_segment {
		char open = '[', close = ']', lfill = '~', rfill = '~', empty = ' ';

		template<class OutputIt, class T>
		OutputIt write_data(OutputIt buf, T&& val, unsigned val_sz) const {
			return std::format_to(buf, "{}", std::forward<T>(val));
		}

		template<branches br, class OutputIt>
		OutputIt write_branch(OutputIt buf, unsigned cnt) const {
			if (cnt == 0) return buf;
			if constexpr (br == branches::left)
				std::fill_n(*buf++ = open, cnt - 1, lfill);
			else if constexpr (br == branches::right)
				*std::fill_n(buf, cnt - 1, rfill)++ = close;
			else std::fill_n(buf, cnt, empty);
			return buf;
		}
	};

	template<class TBase>
	struct colored_segment_wrapper :TBase {
		using base_t = TBase;

		ansi_color left_col, right_col, data_col;

		template<class OutputIt, class T>
		OutputIt write_data(OutputIt buf, T&& val, unsigned val_sz) const {
			return data_col.write(buf, [&]() {base_t::write_data(buf, std::forward<T>(val), val_sz);});
		}

		template<branches br, class OutputIt>
		OutputIt write_branch(OutputIt buf, unsigned cnt) const {
			const auto wr = [&]() {return base_t::write_branch<br>(buf, cnt);};
			if constexpr (br == branches::left)
				return left_col.write(buf, wr);
			else if constexpr (br == branches::right)
				return right_col.write(buf, wr);
			else return wr();
		}
	};

	using colored_segment = colored_segment_wrapper<plain_segment>;

	template <class TNode, class TSegment, class TOnFormat>
	struct tree_main_style {
		using node_t = TNode;
		using segment_t = TSegment;
		using on_format_t = TOnFormat;
		using tree = tree_interface<node_t>;

		segment_t segment;
		on_format_t on_format;

		struct tree_line {
			std::string line;
			std::size_t line_size = 0;
		};

		std::vector<tree_line> lines;

		//TODO add to the segment constexpr static bool allow_nonprint = true;

		constexpr static unsigned value_fsize(node_t node) {
			auto v = tree::value_fsize(node);
			return v ? (*v) : std::formatted_size("{}", tree::value(node));
		}

		template<class OutputIt>
		constexpr auto fill_buffer(OutputIt buf, node_t node, unsigned val_sz, node_info inf) {
			std::size_t pos = 0;
			const unsigned expected_sz = inf.span.size();
			auto val = [&]() {segment.write_data(buf, tree::value(node), val_sz); };
			auto left = [&](unsigned p) { segment.write_branch<branches::left>(buf, p); };
			auto right = [&](unsigned p) {segment.write_branch<branches::right>(buf, p);};
			switch (inf.type)
			{
			case node_type::left:
				pos = expected_sz - val_sz;
				left(pos), val();
				break;
			case node_type::right:
				val(), right(expected_sz - val_sz);
				break;
			case node_type::both:
				pos = (expected_sz - val_sz) / 2;
				left(pos), val(), right(expected_sz - val_sz - pos);
				break;
			case node_type::leaf:
				val();
				break;
			}
			return pos + (!inf.is_right ? 0 : val_sz - 1);
		}


		constexpr auto format_node(node_t node, node_info inf, unsigned val_sz) {
			const unsigned y = inf.depth;
			if (y >= lines.size()) {
				lines.insert(lines.end(), y - lines.size() + 1, {});
			}

			auto& ln = lines[y];
			assert(ln.line_size <= inf.span.start_pos);

			if constexpr (requires{std::invoke(on_format, *this, node, inf); }) 
				std::invoke(on_format, *this, node, inf);
			else  std::invoke(on_format, *this, node);

			auto buf = std::back_inserter(ln.line);
			segment.write_branch<branches::empty>(buf, inf.span.start_pos - ln.line_size);

			ln.line_size = inf.span.end_pos + 1;

			return fill_buffer(buf, node, val_sz, inf);
		}

		constexpr friend auto& operator<<(auto& os, const tree_main_style& st) {
			std_ext::first_foreach(st.lines, [&os](const auto& ln) {os << ln.line;}, [&os](const auto& ln) {os << '\n' << ln.line;});
			return os;
		}
	};

	template<class T1, class T2, class T3>
	struct std::formatter<tree_main_style<T1, T2, T3>> {
		constexpr auto parse(std::format_parse_context& ctx) {
			return ctx.begin();
		}
		auto format(const ::pretty_tree::tree_main_style<T1, T2, T3>& tr, std::format_context& ctx) const {
			return std::format_to(ctx.out(), "{:s}", 
				tr.lines | std::views::transform([](const auto& ln) -> const std::string& {return ln.line;}) | std::views::join_with('\n'));
		}
	};

	template<class TStyle>
	struct tree_printer {
		using style_t = TStyle;
		using node_t = typename std::decay_t<style_t>::node_t;
		using tree = tree_interface<node_t>;

		style_t style;
		std::size_t lnsz = 0;

		constexpr void print(node_t node, node_info c, node_info& par) {
			if (!tree::exists(node))
			{
				if (c.is_right) par.span.end_pos = lnsz - 1;
				return;
			}
			par.type |= !c.is_right ? node_type::left : node_type::right;

			print(tree::left(node), { .is_right = false, .depth = c.depth + 1 }, c);
			const auto val_sz = style.value_fsize(node);
			const auto oln = lnsz;
			lnsz += val_sz;

			if (c.span.start_pos == text_span::npos) c.span.start_pos = oln;

			if (!c.is_right) par.span.start_pos = oln;
			else             par.span.end_pos = lnsz - 1;

			print(tree::right(node), { .is_right = true, .depth = c.depth + 1 }, c);

			const auto new_pos = c.span.start_pos + style.format_node(node, c, val_sz);
			if (!c.is_right) par.span.start_pos = new_pos;
			else             par.span.end_pos = new_pos;
		}

		constexpr auto operator()(node_t node) {
			node_info ni{};
			print(node, {}, ni);
			return std::forward<TStyle>(style);
		}
	};

	template <class TNode>
	using tree_simple_style = tree_main_style<TNode, plain_segment, std_ext::discard>;

	template<class TNode, class Style = tree_simple_style<TNode>>
	constexpr auto print_tree(TNode node, Style&& style = {}) {
		static_assert(std::is_same_v<TNode, typename std::decay_t<Style>::node_t>);
		return tree_printer<Style>{std::forward<Style>(style)}(node);
	}

	namespace main_style {
		template<class TNode, class TSegment = plain_segment, class TOnFormat = std_ext::discard>
		constexpr auto print_tree(TNode node, TSegment&& segment = {}, TOnFormat&& on_format = {}) {
			auto style = tree_main_style<TNode, TSegment, TOnFormat>{ std::forward<TSegment>(segment), std::forward<TOnFormat>(on_format) };
			return ::pretty_tree::print_tree(node, std::move(style));
		}
	}

	namespace tests {

		void sscheck(auto&& data, std::string_view expected) {
			std::ostringstream ss{};
			ss << std::forward<decltype(data)>(data);
			const std::string str = std::move(ss).str();
			if (expected != str) {
				std::print("Test was NOT passed, expected:\n{}\nfound:\n{}\n", expected, str);
				throw false;
			}
		}


		const plain_segment tst_seg1{ .open = '[', .close = ']', .lfill = '<', .rfill = '>', .empty = ' ' };
		const plain_segment tst_seg2{ .open = '{', .close = '}', .lfill = ':', .rfill = ';', .empty = '@' };
		constexpr auto x = nullptr;

		namespace test1 {
			struct tree2i {
				std::vector<int>& stor;
				int ind = 1;
				operator bool() {return ind <= stor.size();}
				tree2i left() {ind *= 2;return *this;};
				tree2i right() { ind = ind * 2 + 1;return *this; };
				int value() {return stor[ind - 1];}
			};
			void run() {
				auto root = std::vector{ -10,-20,-3252340,41430,-5,-6,7124,-81,9,123456,3546753,-6213410,841530 };
				auto tree = tree2i{ root };
				//std::cout << pretty_tree::print_tree(tree2i{ root2 }) << '\n';
				constexpr auto correct = 1 + R"(
         [<<<<<<<<<<<<<<<<<<-10>>>>>>>>>>>>>>>>>>>]
  [<<<<<<-20>>>>>>>]                 [<<<<<-3252340>>>>>>]
[<41430>]   [<<<<<-5>>>>>>]   [<<<<<<-6>>>>>>]        7124
-81     9   123456  3546753   -6213410  841530)";
				sscheck(main_style::print_tree(tree, tst_seg1), correct);
			}
		}

		namespace test2 {
			struct node {
				int value;
				node* left = nullptr;
				node* right = nullptr;

				~node() {
					delete left;
					delete right;
				}
			};

			node* cr(int val, node* left = nullptr, node* right = nullptr) { return new node{ val, left, right }; }
			void check(node* rt, std::string_view ex, plain_segment seg = tst_seg1) { sscheck(main_style::print_tree(rt, seg), ex); if (rt != nullptr) delete rt; }

			void run() {

				check(static_cast<node*>(x), "");//empty
				check(cr(12345), "12345");//signle

				auto rt = cr(1, cr(12, cr(123, cr(12345)), cr(123454)));
				check(rt, 1 + R"(
         [<<<<<<1
     [<<<12>>>>]
[<<<<123  123454
12345)");

				rt = cr(10, cr(2453, cr(4144, cr(-8545, cr(16, cr(32)), cr(17, cr(33))), cr(9, cr(18, x, cr(34))))));
				check(rt, 1 + R"(
                      [<<<10
           [<<<<<<<<<<2453
     [<<<<<4144>>>>>>]
  [<<-8545>>]    [<<<9
[<16     [<17    18>]
32       33        34)");

				rt = cr(0, cr(2, x, cr(10, cr(20), cr(30))), cr(3));
				check(rt, 1 + R"(
[<<<0>>>]
2>>>]   3
 [<10>]
 20  30)");

				rt = cr(123, cr(435, cr(55225, cr(124), cr(1454)), cr(21012)), cr(3441, cr(235323), cr(125)));
				check(rt, 1 + R"(
@@@@@@@@@@{::::::::123;;;;;;;;}
@@@{::::::435;;;;;;}@@@{:::3441;;;;}
{::55225;;;}@@@21012@@@235323@@@@125
124@@@@@1454)", tst_seg2);
			}
		}

		namespace test3 {
			using test2::node;
			using test2::cr;

			struct colored_fmt {
				int i = 0;
				constexpr static std::array cols{ansi_colors::red, ansi_colors::green, ansi_colors::blue, ansi_colors::empty };
				std::array<int, 32> levs{};
				void operator()(auto& style, auto& node, const auto& inf) {
					auto& seg = style.segment;//if (i > colored_fmt::cols.size()) 
					auto next = [](int& i) { if (i >= colored_fmt::cols.size()) i = 0; return i++;};
					;

					seg.left_col = cols[next(levs[inf.depth])];
					seg.data_col = cols[next(levs[inf.depth])];
					seg.right_col = cols[next(levs[inf.depth])];
				}
			};

			void run() {
				colored_segment seg{tst_seg1};

				auto rt = cr(10, x, cr(3,x,cr(7, x, cr(13, cr(14, cr(24, cr(28), cr(29)), x), cr(15, cr(26, cr(30), cr(31)), cr(27))))));
				constexpr static auto correct =
					"\033[32m10\033[0m\033[34m]\033[0m\n"
					"  \033[32m3\033[0m\033[34m]\033[0m\n"
					"   \033[32m7\033[0m\033[34m>>>>>>>>>>>]\033[0m\n"
					"          \033[31m[<<<\033[0m\033[32m13\033[0m\033[34m>>>>]\033[0m\n"
					"      \033[31m[<<<\033[0m\033[32m14\033[0m    [<<\033[31m15\033[0m\033[32m>>]\033[0m\n"
					"    \033[31m[<\033[0m\033[32m24\033[0m\033[34m>]\033[0m    [<\033[31m26\033[0m\033[32m>]\033[0m  27\n"
					"    \033[32m28\033[0m  \033[31m29\033[0m    30  \033[34m31\033[0m";
				sscheck(main_style::print_tree(rt, seg, colored_fmt{}), correct);
			}
		}

		void run() {
			test1::run();
			test2::run();
			test3::run();
		}

	}
}

/*
*
10~~~~~~~~~~~~~]
		[~~~~~~3~~~~~~]
	[~~~6~~~]     [~~~7~~~]
  [~12~] [~13~] [~14~] [~15~]
  24  25 26  27 28  29 30  31


			[~~~~~~~~~~~~~1~~~~~~~~~~~~~]
	 [~~~~~~22~~~~~~~]     [~~~~~33333333~~~~~~]
[~~~~444444~~~~~]  [~5~] [~666777~]        [~888
1234134      2354  2 354 235      3        46




 COLORED TEST

			[~~~~~~1~~~~~~~]
		[~~~20~~~~]     [~~3~~]
   [~~~~4~~~~]   [5~] [~2~~] [3~]
[~~54675~~] [5]  2 34 25 346 4 67
5467     43 7 5
*/