#include <iostream>
#include <vector>
#include <ranges>
#include <cassert>
#include <cstddef>
#include <array>
#include <algorithm>
#include <concepts>
#include <tuple>
#include <string_view>
#include "utils.hpp"

//https://stackoverflow.com/questions/2539545/fastest-gap-sequence-for-shell-sort
//https://stackoverflow.com/questions/21508595/shellsort-2-48k-1-vs-tokudas-sequence
//https://stackoverflow.com/questions/25295265/how-do-you-test-speed-of-sorting-algorithm

namespace shell_sorts {
	//Based on "Hai He (hai(AT)mathteach.net) and Gilbert Traub, Dec 28 2004", see https://oeis.org/A003586
	template<std::ranges::forward_range Rng, class int_t = std::ranges::range_value_t<Rng>>
	constexpr auto smooth3_seq(Rng&& rng, int_t a, int_t b) {
		auto beg = std::ranges::begin(rng);
		const auto end = std::ranges::end(rng);
		if (beg == end) return rng;

		auto it = beg;
		*beg++ = 1;
		int_t pow = 1;

		std::ranges::generate(beg, end, [&]() {return (*it * a) < (pow * b) ? (*it++ * a) : pow *= b;});
		return rng;
	}

	using std::size_t;

	template<class Ratio = std::ratio<9, 4>>
	struct ciura01_iterator {
		constexpr static auto ratio = std_ext::ratio_t<Ratio::num, Ratio::den, false>{};

		using value_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		static_assert(Ratio::num > Ratio::den && Ratio::den > 0);

		constexpr static std::array<std::size_t, 9> arr{ 1, 4, 10, 23, 57, 132, 301, 701, 1750 };
		value_type elem{ arr[0] }, ind{};
		constexpr bool operator==(const ciura01_iterator& o) const noexcept { return ind == o.ind; }

		constexpr auto operator*() const noexcept { return elem; }
		constexpr auto& operator++() noexcept {
			return elem = (++ind < arr.size()) ? arr[ind] : elem * std_ext::ratio<Ratio{}> , *this;
		}
		constexpr auto operator++(int) noexcept {
			auto copy = *this;
			return ++ * this, copy;
		}
		constexpr auto& operator--() noexcept {
			return elem = (--ind < arr.size()) ? arr[ind] : elem/std_ext::up_ratio<Ratio{}>, *this;
		}
		constexpr auto operator--(int) noexcept {
			auto copy = *this;
			return -- * this, copy;
		}
	};

	//2.25 or 2.243609061420001L

	template<auto gamma>
	struct mulplusone_iterator {
		using value_type = size_t;
		using difference_type = ptrdiff_t;
		decltype(gamma) elem{ 1 };

		constexpr value_type operator*() const noexcept { 
			//return static_cast<value_type>(elem) +(elem > static_cast<value_type>(elem));
			return static_cast<value_type>(elem) + (elem - static_cast<value_type>(elem)>1e-10);
		}
		constexpr bool operator==(const mulplusone_iterator& o) const noexcept {return **this == *o;}

		constexpr auto& operator++() noexcept {
			return elem = (elem * gamma + 1), *this;
		}
		constexpr auto operator++(int) noexcept {
			auto copy = *this;
			return ++ * this, copy;
		}
		constexpr auto& operator--() noexcept {
			return elem = ((elem - 1) / gamma), *this;
		}
		constexpr auto operator--(int) noexcept {
			auto copy = *this;
			return -- * this, copy;
		}
	};

	template<size_t a, size_t b, size_t size>
	constexpr auto cached3smooth = smooth3_seq(std::array<size_t, size>{}, a, b);

	namespace views = std::views;
	namespace ranges = std::ranges;
	using ranges::subrange, views::take;

	template<class TSeq> requires std::invocable<TSeq>
	constexpr bool eql(std::initializer_list<std::ranges::range_value_t<decltype(TSeq{}())>> lst) {
		return std::ranges::equal(TSeq{}() | std::views::take(lst.size()), lst);
	}

	template<class TSeq> requires std::invocable<TSeq, size_t>
	constexpr bool eql(size_t n, std::initializer_list<size_t> lst) {
		return std::ranges::equal(TSeq{}(n), lst);
	}

	template<class Cmp = std::less<>>
	constexpr auto tail(size_t n, Cmp cmp = {}) { return views::take_while([=](size_t i) { return cmp(i, n); }) | views::reverse; };

	template<class S>
	constexpr auto seq = [](size_t n) {
		if constexpr (std::invocable<S, size_t>)
			return S{}(n);
		else
			return S{}() | tail(n);
	};

	template<class TSeq>
	constexpr bool eql2(size_t n, std::initializer_list<size_t> lst) {
		return std::ranges::equal(seq<TSeq>(n), lst);
	}

	constexpr auto pow2(auto x) { return static_cast<decltype(x)>(1) << x; }
	constexpr auto take_to1 = views::take_while([](size_t i) constexpr { return i >= 1; });

	constexpr auto inf_seq(size_t start, auto gen) { return views::iota(start) | views::transform(gen); }
	//Note. Самостоятельно проверять выполнение инварианта, согласно которому, конечные последовательностей при n=1|0 должны возвращать Ø

	using shell59 = decltype([](size_t n) { return inf_seq(1, [n](size_t i) {return n >> i;}) | take_to1; });
	static_assert(eql<shell59>(42, { 21, 10, 5 ,2, 1 }));

	using hibard63 = decltype([]() consteval {return inf_seq(1, [](size_t i) {return pow2(i) - 1;}); });
	static_assert(eql<hibard63>({1,3,7,15,31,63,127 }));

	using papernov65 = decltype([]() consteval {return inf_seq(0, [](size_t i) {return i ? pow2(i) + 1 : 1;});});
	static_assert(eql<papernov65>({ 1, 3, 5, 9, 17, 33, 65, 129, 257, 513 }));

	using pratt71 = decltype([](size_t n) {return cached3smooth<2, 3, 1344> | tail<std::less_equal<>>(n >> 1);});
	static_assert(eql<pratt71>(256, { 128,108,96,81,72,64,54,48,36,32,27,24,18,16,12,9,8,6,4,3,2,1 }));

	using sedgewick82 = decltype([]() consteval {return inf_seq(0, [](size_t i) {return i ? pow2(2 * i) + 3 * pow2(i - 1) + 1 : 1;});});
	static_assert(eql<sedgewick82>({ 1, 8, 23, 77, 281, 1073, 4193, 16577, 65921, 262913 }));

	using sedgewick86 = decltype([]() consteval {return inf_seq(0, [](size_t i) {return ((i & 1) ? 8 * pow2(i) - 6 * pow2((i + 1) / 2) : 9 * (pow2(i) - pow2(i / 2))) + 1;});});
	static_assert(eql<sedgewick86>({ 1, 5, 19, 41, 109, 209, 505, 929, 2161, 3905 }));

	using tokuda92 = decltype([]() {return subrange(mulplusone_iterator<2.25L>{}, std::unreachable_sentinel);});
	static_assert(std::ranges::bidirectional_range<decltype(tokuda92{}()) > );
	static_assert(eql2<tokuda92>(153401, { 68178, 30301, 13467, 5985, 2660, 1182, 525, 233, 103, 46, 20, 9, 4, 1 }));
	
	using lee21 = decltype([]() {return subrange(mulplusone_iterator<2.243609061420001L>{}, std::unreachable_sentinel);});
	static_assert(eql2<lee21>(516, { 230, 102, 45, 20,9,4,1 }));

	template<class Ratio>
	using ciura01 = decltype([]() {return subrange(ciura01_iterator<Ratio>{}, std::unreachable_sentinel);});

	using ciura01_225 = ciura01<std::ratio<9,4>>;
	static_assert(std::ranges::bidirectional_range<decltype(ciura01_225{}()) > );
	static_assert(eql2<ciura01_225>(1149241, { 510774, 227011, 100894, 44842, 19930, 8858, 3937, 1750, 701, 301, 132, 57, 23, 10, 4, 1 }));

	using knuth73 = decltype([](size_t n) {return subrange(mulplusone_iterator<3>{}, std::unreachable_sentinel) | tail<std::less_equal<>>(n * std_ext::up_ratio<1,3>);});

	template<class S>
	auto cache(size_t n) {
		return [vec = seq<S>(n) | ranges::to<std::vector<size_t>>()](size_t n) {
			return subrange(ranges::upper_bound(vec, n, ranges::greater{}), vec.cend());};
	}

	template<class S, size_t n>
	struct cx_cache {
		constexpr static auto data = [](){
			auto sq = seq<S>(n);
			constexpr auto size = []() consteval {return static_cast<size_t>(std::ranges::distance(seq<S>(n)));};
			std::array<size_t, size()> ret{};
			std::ranges::copy(sq, ret.begin());
			return ret;
		}();

		constexpr auto operator()(size_t n) const noexcept {
			return subrange(ranges::upper_bound(data, n, ranges::greater{}), data.end());
		}
	};
}

struct insertion_sort_fn {
	template<std::ranges::bidirectional_range Rng, class Cmp = std::ranges::less>
	requires std::sortable<std::ranges::iterator_t<Rng>, Cmp>
	constexpr void operator()(Rng&& rng, Cmp cmp = {}, std::ranges::range_size_t<Rng> gap = 1) const {
		auto move_back = [gap](const auto first, auto it, auto&& pred) {
			for (; ;)
			{
				auto prev = std::ranges::prev(it, gap, first);
				if (auto elem = *prev; it != first && std::invoke(pred, elem)) {
					*it = std::move(elem);
					it = std::move(prev);
				}
				else break;
			}
			return std::move(it);
			};

		auto first = std::ranges::begin(rng);
		auto last = std::ranges::end(rng);

		for (auto it = std::ranges::next(first, gap, last); it != last; ++it) {
			auto elem = std::move(*it);
			*move_back(first, it, [&elem, &cmp](const auto& v) {return std::invoke(cmp, elem, v);}) = std::move(elem);
		}
	}
};
inline constexpr insertion_sort_fn insertion_sort{};

struct shell_sort_fn {
	using default_seq = decltype(shell_sorts::seq<shell_sorts::sedgewick82>);
	template<std::ranges::bidirectional_range Rng, class Cmp = std::ranges::less, class TSeq = default_seq>
		requires std::sortable<std::ranges::iterator_t<Rng>, Cmp>
	constexpr void operator()(Rng&& rng, Cmp cmp = {}, TSeq&& seq = {}) const {
		const auto size = std::ranges::size(rng);
		auto gaps = std::invoke(std::forward<TSeq>(seq), size);

		assert([&]() {
			auto vec = gaps | std::ranges::to<std::vector>();
			if (vec.empty()) return size <= 1;
			return std::ranges::is_sorted(vec, std::greater_equal<>{}) && vec.front() < size && vec.back() == 1;
			}());

		std::ranges::for_each(gaps, [&](const auto gap) {insertion_sort(rng, cmp, gap);});
	}
};
inline constexpr shell_sort_fn shell_sort{};



namespace sort_tester {
	template<class Cmp = std::ranges::less>
	struct count_comparer {
		Cmp base{};
		std::size_t count{};
		constexpr auto operator()(auto&& l, auto&& r) {
			++count;
			return base(std::forward<decltype(l)>(l), std::forward<decltype(r)>(r));
		}
	};

	template<class Cmp = std::ranges::less>
	struct inv_comparer {
		Cmp base{};
		constexpr auto operator()(auto&& l, auto&& r) {
			return base(std::forward<decltype(r)>(r), std::forward<decltype(l)>(l));
		}
	};

	auto make_sort(auto&& user, auto&& func, auto&&... args) {
		return std::make_tuple(std::forward<decltype(user)>(user), std::forward<decltype(func)>(func), std::forward<decltype(args)>(args)...);
	}

	//Каждый провайдер - кортеж, первый параметр назначен для пользовательских нужд, второй - функция с одним аргумент, заполняющая диапазон
	auto make_prov(auto&& user, auto&& func) {
		return std::make_tuple(std::forward<decltype(user)>(user), std::forward<decltype(func)>(func));
	}

	auto make_sorts(auto&&... sorts) {
		return std::make_tuple(std::forward<decltype(sorts)>(sorts)...);
	}

	auto make_providers(auto&&... funcs) {
		return std::make_tuple(std::forward<decltype(funcs)>(funcs)...);
	}

	namespace providers {
		//Note. Представленные лямбды захватывают парметр seq по значению, что неожидаемо для lvalue ссылок.
		auto just_fill(auto&& seq) {
			return [=](std::ranges::forward_range auto&& rng) {std::ranges::generate(rng, std::move(seq));};
		}

		auto unique_fill(auto& rnd, int pr) {
			return [=](std::ranges::sized_range auto&& rng) mutable {std::ranges::generate(rng, rnd.generator<std::size_t>(0, std::ranges::size(rng) * pr / 100));};
		}

		template<class Cmp = std::ranges::less>
		auto sorted_fill(auto&& seq, Cmp cmp = {}) {
			return [=](std::ranges::forward_range auto&& rng) {
				just_fill(std::move(seq))(rng);
				std::ranges::sort(rng, std::move(cmp));
			};
		}

		template<class Cmp = std::ranges::less>
		auto reverse_fill(auto&& seq, Cmp cmp = {}) {
			return sorted_fill(std::forward<decltype(seq)>(seq), inv_comparer<Cmp>{std::move(cmp)});
		}

		template<class Cmp = std::ranges::less>
		auto invert_fill(auto& rnd, auto&& seq, int pr, Cmp cmp = {}) {
			return [=, &rnd](std::ranges::random_access_range auto&& rng) mutable {
				sorted_fill(std::move(seq), std::move(cmp))(rng);
				rnd.shuffle_n(rng, std::ranges::size(rng) * pr / 100);
			};
		}
	}

	struct sort_report {
		double secs;
		std::size_t cmps;
		auto& operator+=(const sort_report& o) {
			secs += o.secs;
			cmps += o.cmps;
			return *this;
		}
	};


	struct printer {
		using named_report = std::pair<std::string, sort_report>;
		std::unordered_map<std::string, sort_report> reps;

		friend auto& operator<< (auto& wr, const printer& p) {
			constexpr static int after_point = 6;
			auto vec = p.reps | std::ranges::to<std::vector<named_report>>();
			std::ranges::sort(vec, [](const named_report& l, const named_report& r) {return l.second.cmps < r.second.cmps;});

			constexpr std::string_view NAME = "NAME";
			constexpr std::string_view CMPS = "CMPS";
			constexpr std::string_view SECS = "SECS";

			size_t name_sz{ NAME.size() }, cmps_sz{ CMPS.size() }, secs_sz{ SECS.size() };

			for (const auto& [name, rep] : vec) {
				name_sz = std::max(name_sz, name.size());
				cmps_sz = std::max(cmps_sz, std::formatted_size("{}", rep.cmps));
				secs_sz = std::max(secs_sz, std::formatted_size("{:.{}f}", rep.secs, after_point));
			}

			std::println(wr, "|{:^{}}|{:^{}}|{:^{}}|", NAME, name_sz, CMPS, cmps_sz, SECS, secs_sz);
			for (const auto& [name, rep] : vec) {
				std::println(wr, "|{:<{}}|{: >{}}|{:.{}f}|", name, name_sz, rep.cmps, cmps_sz, rep.secs, after_point);
			}
			return wr;
		}

		void new_provider(std::string_view name) {
			std::cout << name << '\n';
		}
		void end_provider(std::string_view name) {
			std::cout << *this<<'\n';
			reps.clear();
		}

		void sort_done(std::string_view prov, std::string_view sort, const sort_report& rep) {
			reps[std::string{ sort }] += rep;
		}
	};

	template<class Callback, class Cont = std::vector<int>, class Cmp = std::ranges::less>
	struct tester {
		Callback callback;
		Cmp cmp;

		void run_sort(Cont container, auto& prov_user, auto& sort_user, auto& sort_f, auto&... sort_args) {
			count_comparer<Cmp> count_cmp{ cmp };
			auto tim = utils::speed_counter::start_new();
			std::invoke(sort_f, container, std::ref(count_cmp), sort_args...);
			const auto ms = tim.stop().count();
			callback.sort_done(prov_user, sort_user, sort_report{ ms, count_cmp.count });
			if (!std::ranges::is_sorted(container, cmp)) std::println("An internal error occurred");
		}

		void run_provider(std::ranges::forward_range auto& sizes, auto& prov_user, auto& prov, auto& sorts) {
			Cont container{};
			callback.new_provider(prov_user);
			for (std::size_t size : sizes) {
				container.resize(size);
				std::invoke(prov, container);
				std::apply([&](auto&... sort) { (std::apply([&](auto&... args) {run_sort(container, prov_user, args...);}, sort), ...); }, sorts);
			}
			callback.end_provider(prov_user);
		}

		template<std::ranges::forward_range TSizes = std::initializer_list<std::size_t>>
		void run(TSizes&& sizes, auto&& provs, auto&& sorts) {
			std::apply([&](auto&... prov) { (std::apply([&](auto&... args) {run_provider(sizes, args..., sorts);}, prov), ...); }, provs);
		}
	};
}

bool test() {
	using namespace shell_sorts;

	struct error {};

	const auto check0 = [](auto&& data, auto cmp, auto seq) {
		shell_sort(data, cmp, seq);
		if (!std::ranges::is_sorted(data, cmp)) {
			std::println("Data was NOT sorted, input: {}", utils::space_joiner(data));
			throw error{};
		}
		};

	const auto check1 = [&](auto&& data, auto seq) {
		check0(data, std::less<>{}, seq);
		check0(data, std::greater<>{}, seq);
		};

	const auto check2 = [&](auto&& data) {
		check1(data, seq<shell59>);
		check1(data, seq<hibard63>);
		check1(data, seq<papernov65>);
		check1(data, seq<sedgewick82>);
		check1(data, seq<sedgewick86>);
		check1(data, seq<pratt71>);
		check1(data, seq<ciura01_225>);
		check1(data, seq<tokuda92>);
		};


	const auto check = [&](std::size_t n, std::size_t rep) {
		for (std::vector<int> data(n); rep--;) {
			std::ranges::generate(data, utils::stable_random().generator(-static_cast<int>(n), static_cast<int>(n)));
			check2(data);
		};
		};

	try
	{
		check(0, 1); for (int i = 1; i <= 100;i *= 10) check(i, 10);
	}
	catch (error)
	{
		return false;
	}
	return true;
}

struct merge_sort_fn {

	template <typename LeftIt, typename RightIt, typename BufferIt>
	void merge(LeftIt left, LeftIt left_last, RightIt right, RightIt right_last, BufferIt buffer, auto cmp)
	{
		for (; left != left_last && right != right_last; ++buffer) {
			const auto l_val = *left;
			const auto r_val = *right;
			if (std::invoke(cmp, r_val, l_val)) {
				*buffer = std::move(r_val);
				++right;
				//if constexpr (count_inversions)
				//	inversions<true>::count += std::distance(left, left_last);
			}
			else {
				*buffer = std::move(l_val);
				++left;
			}
		}
		std::copy(left, left_last, buffer);
		std::copy(right, right_last, buffer);
	}

	template <typename DataIt, typename BufIt>
	void merge_step(
		DataIt data_it, DataIt data_end, 
		BufIt buf_it, BufIt buf_end,
		const std::size_t length, auto cmp) // length should be power of two
	{
		const std::size_t half_length = length / 2;
		for (; buf_end - buf_it >= length; buf_it += length, data_it += length) {
			merge(data_it, data_it + half_length, data_it + half_length, data_it + length, buf_it, cmp);
		}
		if (buf_it != buf_end) {
			buf_it -= length;
			DataIt merging_it = data_it - length;
			merge(buf_it, buf_it + length, data_it, data_end, merging_it, cmp);
			std::copy(merging_it, data_end, buf_it);
		}
	}

	template <typename InputIt, class Comp = std::ranges::less>
	void merge_sort(InputIt first, InputIt last, Comp cmp = {})
	{
//		if constexpr (count_inversions)
//			inversions<true>::count = 0;
		std::vector<typename std::iterator_traits<InputIt>::value_type> buffer(std::distance(first, last));
		const std::size_t size = buffer.size();
		const auto buf_start = buffer.begin();
		const auto buf_end = buffer.end();
		std::size_t step = 1;

		while (step * 4 <= size) {
			merge_step(first, last, buf_start, buf_end, step * 2, std::ref(cmp));
			merge_step(buf_start, buf_end, first, last, step * 4, std::ref(cmp));
			step *= 4;
		}
		if (step * 2 <= size) {
			merge_step(first, last, buf_start, buf_end, step * 2, std::ref(cmp));
			if (step * 2 == size)
				std::copy(buffer.cbegin(), buffer.cend(), first);
		}
	}

};



using merge = decltype([](std::ranges::random_access_range auto&& rng, auto cmp) {
	merge_sort_fn{}.merge_sort(std::ranges::begin(rng), std::ranges::end(rng), std::move(cmp));
});

int main() {;

std::println("{}", shell_sorts::seq<shell_sorts::knuth73>(119));


	assert(test());
	std::println(1+R"(
Welcome to Sorter 1.0(2024.12.24) by PavelPI.
It's just a benchmark that compares the performance of shell sorting with different gaps sequences.  
In addition, other popular sorting algorithms will be used.
)");

	{
		using namespace shell_sorts;
		using namespace sort_tester;
		using sort_tester::make_sort, sort_tester::make_providers;
		constexpr auto shell = [](auto&& user, auto&& seq) {return make_sort(std::forward<decltype(user)>(user), shell_sort, std::forward<decltype(seq)>(seq));};

		using data_t = int;
		auto& rnd = utils::thread_random();
		const auto gen = rnd.generator<data_t>();

		sort_tester::tester<printer, std::vector<data_t>> tst{};

		tst.run({ 1,10,100,1000,10000, 1000000, 5000000,10000000 },
			make_providers(
				make_prov("===Step1. Fill data by random===", providers::just_fill(gen)),
				make_prov("===Step2. Fill data with sorted array with few permutation===", providers::invert_fill(rnd, gen, 10)),
				make_prov("===Step3. Fill data with reverse sorted array===", providers::reverse_fill(gen)),
				make_prov("===Step4. Fill data by few unique===", providers::unique_fill(rnd, 10))
			),
			make_sorts(
				//shell("hibard63", seq<hibard63>),
				//shell("papernov65", seq<papernov65>),
				//shell("pratt71", seq<pratt71>),
				//shell("knuth73", seq<knuth73>),
				shell("sedgewick82", seq<sedgewick82>),
				shell("sedgewick86", seq<sedgewick86>),
				shell("tokuda92", seq<tokuda92>),
				shell("ciura01_225", seq<ciura01_225>),
				shell("lee21", seq<lee21>),
				make_sort("std::ranges::sort", std::ranges::sort),
				make_sort("merge sort", merge{})
			)
		);
	}
	return 0;
}
