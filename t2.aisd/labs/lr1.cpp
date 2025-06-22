#include <algorithm>

#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <unordered_map>
#include <limits>
#include <ranges>
#include <queue>
#include <variant>
#include <utility>
#include <iomanip>
#include <string_view>
#include "utils.hpp"

template<class T, class Container = std::vector<T>, class Comparer = std::less<typename Container::value_type>>
struct heap {
	using container_type = Container;
	using value_type = container_type::value_type;
	using size_type = container_type::size_type;
	using reference = container_type::reference;
	using const_reference = container_type::const_reference;
	using comparer_type = Comparer;
private:
	using ind_t = size_type;
	using init_list = std::initializer_list<value_type>;

	container_type cont;
	comparer_type cmp;

	template<class ptr_t>
	struct tree_base {
		ind_t ind;//Indexing from 1
		ptr_t stor;
		explicit operator bool() { return ind >= 1 && ind <= stor->size(); }
		tree_base left() { return {ind*2, stor}; };
		tree_base right() { return {ind*2+1, stor }; };
		tree_base parent() { return {ind/2, stor }; };
		decltype(auto) value() { return stor->operator[](ind - 1); }
		//C++23: bfs based on co_yield
	};
	using tree = tree_base<container_type*>;
	using const_tree = tree_base<container_type const*>;
private:
	void sift_down(ind_t ind) {
		using std::swap;
		tree cur{ ind + 1, &cont };
		for (tree l, child; (l = cur.left()); cur = child)
		{
			child = (cur.right() && cmp(cur.right().value(), l.value()))?cur.right():l;
			if (cmp(child.value(), cur.value())) swap(cur.value(), child.value()); else break;
		}
	}

	void sift_up(ind_t ind) {
		using std::swap;
		tree cur{ ind + 1, &cont };
		for (tree par; (par = cur.parent()) && cmp(cur.value(), par.value()); cur = par) {
			swap(cur.value(), par.value());
		}
	}

	void fix_heap() {
		for (ind_t ind = cont.size() / 2; ind;) {
			sift_down(--ind);
		}
	}

public:
	const_tree get_tree() const {
		return { 1, &cont };
	}

	template<class TCont = init_list, class TCmp = comparer_type>
		requires (std::constructible_from<container_type, TCont&&> && std::constructible_from<comparer_type, TCmp&&>)
	heap(TCont&& c = {}, TCmp&& cmp = {}) : cont(std::forward<TCont>(c)), cmp(std::forward<TCmp>(cmp)) {
		fix_heap();
	}

	template<std::ranges::input_range TRng = init_list>
	void push_range(TRng&& rng) {
		cont.append_range(std::forward<TRng>(rng));
		fix_heap();
	}
	
	template<class TV> requires std::convertible_to<std::remove_reference_t<TV>, value_type>
	void push(TV&& val) {
		cont.push_back(std::forward<TV>(val));
		sift_up(cont.size()-1);
	}

	const_reference top() noexcept {return cont.front();}
	constexpr bool empty() const noexcept { return cont.empty(); }
	constexpr size_type size() const noexcept { return cont.size(); }

	void pop() {
		cont.front() = cont.back();
		cont.pop_back();
		sift_down(0);
	}

	value_type get_top() {
		//TODO гарантия исключений, включить метод по requires
		value_type ret = std::move(cont.front());
		pop();
		return ret;
	}
};

void test() {
	{
		heap<int> h1;//check default constructible
		heap<int> h2{ h1 };//check copy constructor
		h2 = h1;
		heap<int> h{ {1,2,3} };
	}

	const auto comp_check = [](std::initializer_list<int> data, auto comp) {
		heap<int, std::vector<int>, decltype(comp)> heap{ data, comp };
		if (heap.size() != data.size()) throw false;
		//We check not only the order, but also the presence of all the necessary elements
		std::vector<int> sorted{ data }; std::ranges::stable_sort(sorted, comp);
		for(const auto val:sorted) {
			if (heap.get_top() != val) throw false;
		}
	};

	const auto check = [&](std::initializer_list<int> data) {
		comp_check(data, std::less<int>{});
		comp_check(data, std::greater<int>{});
	};

	check({});
	check({1});
	check({ -10, 10 });
	check({ 1,2,3,4,5,6,7,0,8,0,2,5,5,1,693,3 });
	check({ 99, 2, 3,4,5,6,7,8,9,10,11,12,-10, 5,10,1,13,14,15,16,17,18,19,0,1,2,23,24,25,26,7,28,29,30,31 });
}

int main()
{
#ifndef NDEFBUG
	test();
#endif

	constexpr static std::string_view br = "\n---------------------------";
	using data_t = int;
	using comp_t = std::less<data_t>;
	comp_t comp{};

	std::vector<data_t> input;
	input.reserve(utils::thread_random().next(10, 25));
	std::ranges::generate_n(std::back_inserter(input), input.capacity(), utils::thread_random().generator<data_t>(0,50));

	std::print("Input data: {}\n", utils::make_joiner<" ">(input));

	heap<data_t> heap(std::move(input));
	std::print("Step 1. Constructed min heap:{0}\n{1}{0}\n", br, pretty_tree::print_tree(heap.get_tree()));
	std::print("Step 2. Min element: {}\n", heap.get_top());
	std::print("Step 3. After extracting:{0}\n{1}{0}", br, pretty_tree::print_tree(heap.get_tree()), br);
	return 0;
}