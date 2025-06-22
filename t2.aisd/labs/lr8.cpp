//https://en.cppreference.com/w/cpp/memory/construct_at
//https://stackoverflow.com/questions/61272763/why-cant-stdpriority-queuetop-return-a-non-const-reference
//https://stackoverflow.com/questions/25035691/why-doesnt-stdqueuepop-return-value
//https://thephd.dev/output-ranges
//https://stackoverflow.com/a/76140307
//https://www.cppstories.com/2023/fun-print-tables-format/
//https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
//https://medium.com/@guilhermeprogrammer/c-string-formatting-part-2-4440bb7d04cc
//https://stackoverflow.com/questions/67716780/is-it-possible-advisable-to-return-a-range
//https://vittorioromeo.com/index/blog/capturing_perfectly_forwarded_objects_in_lambdas.html
//https://learn.microsoft.com/en-us/cpp/cpp/constructors-cpp?view=msvc-170#extended_aggregate наследование конструкторов, using
//https://stackoverflow.com/questions/23389676/avoid-copying-string-from-ostringstream
//https://stackoverflow.com/questions/24879417/avoiding-the-first-newline-in-a-c11-raw-string-literal
//https://stackoverflow.com/questions/8016880/c-less-operator-overload-which-way-to-use
//https://stackoverflow.com/questions/8016880/c-less-operator-overload-which-way-to-use
//https://it.underhood.club/Nekrolm#0d919877586243839e64e782ee6bc5f9
//https://gist.github.com/Nekrolm/27ef2c1cb284c47e875115f90a5d5c21
//https://superuser.com/questions/413073/windows-console-with-ansi-colors-handling
//https://stackoverflow.com/questions/13416418/define-nominmax-using-stdmin-max
//https://stackoverflow.com/a/15913187

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

//Лабораторная работа 8. Задачи сжатия и кодирования информации. 
//Написать программу, используя алгоритм сжатия Хаффмана, для кодирования своих фамилии и имени. 

namespace outsys {
	using pretty_tree::ansi_color;
	using pretty_tree::ansi_colors;
}

namespace huffman {
	struct code {
		std::size_t value;
		unsigned depth;//количество бит отводимых на код
		outsys::ansi_color color{ outsys::ansi_colors::empty };
		
		constexpr void for_each(auto&& func) const {
			for (code c = *this; c.depth--; c.value >>= 1) {
				std::invoke(func, (c.value & 1) == 1);
			}
		}
		constexpr code add_bit(bool v) const {
			return code{ value | (static_cast<std::size_t>(v) << depth), depth + 1, color };
		}
	};


	template<class Symbol>
	struct node_value {
		using symbol_t = Symbol;
		std::size_t freq;
		std::optional<symbol_t> sym;
		constexpr static auto freq_great = [](auto* const l, auto* const r) {return l->freq > r->freq;};
	};

	template<class Symbol>
	struct node:node_value<Symbol> {
		node* left = nullptr;
		node* right = nullptr;
		auto value() const { return node_value{ this->freq, this->sym }; }
		constexpr bool is_leaf() const noexcept { return this->sym.has_value(); }
		static void free(node*& n) {
			if (!n) return;
			free(n->left);
			free(n->right);
			delete n;//Note: We expect the node to have been created via new
			n = nullptr;
		}
	};

	template<class Symbol>
	struct helper_types {
		using symbol_t = node<Symbol>::symbol_t;
		using usymbol_t = std::make_unsigned_t<symbol_t>;
		constexpr static auto bits_per_code = std::numeric_limits<usymbol_t>::digits;
	};

	template<class Symbol>
	struct tree_builder {
		using node_t = node<Symbol>;

		using symbol_t = node_t::symbol_t;
		using freq_map_t = std::unordered_map<symbol_t, std::size_t>;
		using code_map_t = std::unordered_map<symbol_t, code>;

		static freq_map_t build_freq_map(ranges::input_range auto&& data) {
			freq_map_t map{};
			ranges::for_each(data, [&map](const symbol_t ch) {++map[ch];});
			return map;
		}

		static node_t* build_huffman_tree(const freq_map_t& freq) {
			struct unpop_pqueue : std::priority_queue <node_t*, std::vector<node_t*>, decltype(node_t::freq_great) > {
				auto unpop() {
					auto t = std::move(this->c.front());
					this->pop();
					return t;
				}
			};

			unpop_pqueue queue;
			queue.push_range(freq | std::views::transform([](const auto& m) { return new node_t{ m.second, m.first }; }));
			
			while (queue.size() > 1) {
				node_t* l{ queue.unpop() };
				node_t* r{ queue.unpop() };
				queue.push(new node_t{ l->freq + r->freq, std::nullopt, l, r });
			}
			return queue.empty()?nullptr:queue.unpop();
		}

		template<class OnCode = std::identity>
		static code_map_t build_code_map(const node_t& node, OnCode on_ucode = {}) {
			code_map_t map{};
			const auto on_code = [&on_ucode, &map](symbol_t sym, const code& cd) {map[sym] = cd; std::invoke(on_ucode, sym, map[sym]);};
			struct visitor {
				decltype(on_code) on_code;
				void operator ()(node_t const* node, const code& cd) {
					if (node->is_leaf())
						std::invoke(on_code, *node->sym, cd);
					else {
						this->operator()(node->left, cd.add_bit(0));
						this->operator()(node->right, cd.add_bit(1));
					}
				}
			};
			visitor{ std::move(on_code) }(&node, {});
			return map;
		}

		template<class OnWrited = std::identity>
		static void encode(ranges::input_range auto&& src, std::weakly_incrementable auto dst, const code_map_t& codes, OnWrited on_wr = {}) {
			for (const symbol_t sym : src) {
				code cd = codes.at(sym);
				cd.for_each([&dst](bool b) {*dst++ = b;});
				std::invoke(on_wr, cd);
			}
		}

		static void decode(const node_t& root, ranges::input_range auto&& src, std::weakly_incrementable auto dst) {
			node_t const* cur = &root;
			for (const bool b : src) {
				cur = !b ? cur->left : cur->right;
				if (cur->is_leaf()) {
					*dst++ = *cur->sym;
					cur = &root;
				}
			}
		}
	};
}


namespace outsys {

	template<class Symbol>
	struct printable_symbol { huffman::helper_types<Symbol>::symbol_t sym; };

	template<class Symbol>
	struct printer {
		using symbol_t = huffman::helper_types<Symbol>::symbol_t;
		using usymbol_t = huffman::helper_types<Symbol>::usymbol_t;
		using printable_symbol = printable_symbol<Symbol>;

		constexpr static auto bits_per_code = std::numeric_limits<usymbol_t>::digits;

		constexpr static std::string_view breaker = "---------------------------\n";

		struct entry {
			symbol_t sym;
			std::size_t freq;
			huffman::code huffman;
			constexpr static auto greator_freq = [](const auto& l, const auto& r) {return l.freq >= r.freq;};
		};

		struct code_table_t {
			printer const& owner;
			std::size_t max_symbol_len = 0;
			void update(const entry& ent) {
				max_symbol_len = std::max(max_symbol_len, std::formatted_size("{}", printable_symbol{ ent.sym }));
			}
			friend auto& operator<< (auto& wr, const code_table_t& v) {
				constexpr static std::string_view SYMBOL = "SYMBOL";
				constexpr static std::string_view FREQ = "FREQ";
				std::print(wr, "|{:^{}}|{:^{}}|{}|{}\n", SYMBOL, v.max_symbol_len, "CODE", bits_per_code, FREQ, "HUFFMAN");
				for (const entry& o : v.owner.entries) {
					std::print(wr, "|{: <{}}|{:0>{}b}|{: <{}}|{}\n", 
						printable_symbol{ o.sym }, std::max(v.max_symbol_len, SYMBOL.size()), 
						static_cast<usymbol_t>(o.sym), 
						bits_per_code, 
						o.freq, FREQ.size(),
						o.huffman);
				}
				return wr;
			}
		};

		struct effectivity_t {
			printer const& owner;
			std::size_t raw_sz;
			std::size_t new_sz;
			std::size_t codes_count;
			std::size_t max_freq;

			void update(const entry& ent) {
				new_sz += ent.freq * ent.huffman.depth;
				max_freq = std::max(max_freq, ent.freq);
			}

			friend auto& operator<< (auto& wr, const effectivity_t& e) {
				const auto codes_count = e.owner.entries.size();
				const auto bits_per_freq = static_cast<unsigned>(std::bit_width(e.max_freq));
				const auto raw_sz = e.raw_sz * bits_per_code;
				const auto table_size = codes_count * (bits_per_code + bits_per_freq);
				const auto compressed = e.new_sz + table_size;
				std::print(wr, 1 + R"(
Raw size: {}bi
new size: {}bi
table size = codes count: {} * (bits per symbol: {}bi + bits per freq: {}bi) = {}bi
compressed size = {}bi
Effectivity: {:.2f}%)", raw_sz, e.new_sz, codes_count, bits_per_code, bits_per_freq, table_size, compressed, (1 - static_cast<double>(compressed) / raw_sz) * 100);
				return wr;
			}
		};
	private:
		code_table_t code_table{ *this };
		effectivity_t effectivity{ *this };
	public:
		std::vector<entry> entries;

		void update(const entry& ent) {
			code_table.update(ent);
			effectivity.update(ent);
			entries.push_back(ent);
		}
		code_table_t const& get_code_table() {return code_table;}
		effectivity_t const& get_effectivity(std::size_t raw_sz) { effectivity.raw_sz = raw_sz;  return effectivity; }
		void sort() {ranges::stable_sort(entries, entry::greator_freq);}
	};
}



template<>
struct std::formatter<huffman::code> : std::formatter<std::string_view> {
	using base_t = std::formatter<std::string_view>;
	auto format(const huffman::code& c, std::format_context& ctx) const {
		/*std::string tmp;
		c.for_each([&tmp](bool b) {tmp.push_back(b ? '1' : '0');});
		return base_t::format(tmp, ctx);*/

		std::string tmp;
		auto it = std::back_inserter(tmp);
		c.color.write(it, [&]() { c.for_each([&](bool b) { *it++ = b ? '1' : '0'; });  });
		return base_t::format(tmp, ctx);
	}
};

template<class Symbol>
struct std::formatter<huffman::node_value<Symbol>> {
	constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
	auto format(const huffman::node_value<Symbol>& v, std::format_context& ctx) const {
		return (v.sym) ? std::format_to(ctx.out(), "{}", outsys::printable_symbol<Symbol>{ *v.sym }) : std::format_to(ctx.out(), "{}", v.freq);
	}
};

//TODO improve this specialization to support multibyte characters
template<class Symbol>
struct std::formatter<outsys::printable_symbol<Symbol>> :
	std::formatter<std::conditional_t<std::is_same_v<typename huffman::helper_types<Symbol>::symbol_t, char>, std::string_view, Symbol>> {

	using symbol_t = huffman::helper_types<Symbol>::symbol_t;
	using usymbol_t = huffman::helper_types<Symbol>::usymbol_t;

	auto format(const outsys::printable_symbol<symbol_t> o, std::format_context& ctx) const {
		if constexpr (std::is_same_v<symbol_t, char>) {
			const auto ch = o.sym;
			const auto uc = static_cast<usymbol_t>(o.sym);
			std::string tmp;
			if (ch >= '!' && ch <= '~')
				tmp = ch;
			else {
				switch (ch)
				{
				case '\t':tmp = "\\t";break;
				case '\n':tmp = "\\n";break;
				case '\r':tmp = "\\r";break;
				default: std::format_to(std::back_inserter(tmp), "{:x>2X}", uc);
				}
			}
			return std::formatter<string_view>::format(tmp, ctx);
		}
		else {
			return std::formatter<symbol_t>::format(o.sym, ctx);
		}
	}
};

int main(int argc, char* argv[])
{
	constexpr std::string_view no_cols = "-nc";

	 std::string_view input = 1+R"(
====================
Student: PavelPI
Subject: Algorithms and data structures
Teacher: Angelika Dmitrievna
07.12.2024
====================)";
	 pretty_tree::tests::run();

	if (argc > 1 && *argv[1]!='\0') input = argv[1];
	//TODO Print a message if unsupported arguments were detected
	bool colors = !(argc > 2 && std::string_view{ argv[2] } == no_cols);
	if (colors) colors=utils::enable_coloring().value_or(true);

	using symbol_t = char;
	using builder = huffman::tree_builder<symbol_t>;
	outsys::printer<symbol_t> pp{};
	constexpr auto br = pp.breaker;

	using outsys::ansi_colors;

	constexpr static std::array palette{ansi_colors::empty,  ansi_colors::red, ansi_colors::green, ansi_colors::yellow, ansi_colors::blue, ansi_colors::meganta, ansi_colors::cyan };

	const auto freq_map = builder::build_freq_map(input);
	auto* root = builder::build_huffman_tree(freq_map);
	const auto code_map = builder::build_code_map(*root, [&, ind = 0](symbol_t sym, huffman::code& c) mutable { if (colors) c.color = palette[(ind = ind % palette.size())++]; pp.update({ sym, freq_map.at(sym), c }); });

	std::print("Welcome to Huffman Code Visualizer 1.0(2024.12.10) by PavelPI. Just watch the steps\n");
	std::print("{}Step 1. Let's build a tree using a priority queue:\n{}\n",br, pretty_tree::print_tree(root));
	std::print("{}Step 2. Now we are visit the tree, assuming that the left subtree will give 0, and the right 1, so result is:\n", br);
	std::cout << pp.get_code_table();

	std::print("{}Step 3. From now on, we are ready to encode the input data:\n", br);
	std::vector<bool> enc;
	builder::encode(input, std::back_inserter(enc), code_map, [colors, even=true](huffman::code cd) mutable {cd.color.is_underline = colors && (even = !even);std::print("{}", cd);});

	std::print(".\n{}Step 4. For the test, we will also decode. This should be equivalent to the input:\n", br);
	builder::decode(*root, enc, std::ostream_iterator<symbol_t>(std::cout));

	std::print("\n{}Step 5. Conclusion\n", br);
	std::cout << pp.get_effectivity(root->freq);

	std::print("\n{}HINT. Command line arguments: first - input string, second \"{}\" - disable colors", br, no_cols);
	std::print("\nHINT2. In the table, non-printable characters are encoded in the C style or by using their code in the hex");
	builder::node_t::free(root);
	return 0;
}