#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <optional>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <memory>
#include <queue>
#include <format>
#include <stack>
#include <unordered_map>
#include <list>
#include <print>

#include "utils.hpp"

//Лабораторная работа 6. Динамическое программирование
//Каждой вершине дерева присвоено некоторое число (возможно отрицательное). 
//Нужно найти в дереве путь с максимальной суммой вершин (начальные и конечные вершины могут быть произвольными).  

//https://www.desmos.com/calculator/dr2d7kfukx
using data_t = int;
using optdata_t = std::optional<data_t>;

struct node {
    data_t value;
    node* left = nullptr;
    node* right = nullptr;

    static void free(node*& n) {
        if (!n) return;
        free(n->left);
        free(n->right);
        delete n;//Note: We expect the node to have been created via new
        n = nullptr;
    }
};

struct path_data {
    std::vector<node const*> data;
    long long sum;

    path_data& operator+=(node const* val) {
        sum += val->value;
        data.push_back(val);
        return *this;
    }

    constexpr bool is_present(node const* node) const {
        //Note: O(n) search
        return std::ranges::find(data, node) != data.end();
    }

    auto format_path() const {
        return utils::make_joiner<"->">(data | std::views::transform([](const node* node) {return node->value;}));
    }

    friend path_data operator+ (path_data l, const path_data& r) {
        l.sum += r.sum;
        l.data.append_range(r.data | std::views::reverse);
        return l;
    }
    friend bool operator<(const path_data& l, const path_data& r) {
        return l.sum < r.sum;
    }
};

struct find_max_path_fn {
    path_data max_path;
    const static inline path_data zero{};
    constexpr path_data find(node const* root) {
        if (!root) return {};

        auto l = std_ext::rvmax(find(root->left), zero);
        auto r = std_ext::rvmax(find(root->right), zero);

        l += root;
        if (max_path.data.empty() || (l.sum + r.sum > max_path.sum))
            max_path = l + r;
        
        return std_ext::rvmax(l, r += root);
    }
    
    constexpr auto operator()(node const* root) {
        max_path = {};
        find(root);
        return std::move(max_path);
    }
};

constexpr auto find_max_path(node const* root) {
    return find_max_path_fn{}(root);
}


struct stream_number:optdata_t {
    using base_t = optdata_t;
    constexpr static std::string_view null_str = "x";

    using base_t::base_t;

    friend auto& operator<<(auto& os, const stream_number& num) {
        std::print(os, "{}", num);
        return os;
    }

    friend auto& operator>>(auto& is, stream_number& num) {
        using sentry_t = typename std::decay_t<decltype(is)>::sentry;
        num.reset();
        if (!sentry_t{ is }) return is;

        const int is_peek = is.peek();

        if (is_peek == std::decay_t<decltype(is)>::traits_type::eof())
            return is;

        if (is_peek == null_str[0]) {
            is.ignore();
            for (char ch : null_str | std::views::drop(1)) {
                if (ch != is.peek()) {
                    is.setstate(std::ios_base::failbit);
                    break;
                }
                is.ignore();
            }
        }
        else {
            num.emplace();
            is >> *num;
        }

        return is;
    }
};

template<>
struct std::formatter<stream_number>:std::formatter<std::string_view> {
    using base_t = std::formatter<std::string_view>;
    auto format(const stream_number& o, std::format_context& ctx) const {
        if (o) {
            std::string tmp;
            std::format_to(std::back_inserter(tmp), "{}", *o);
            return base_t::format(tmp, ctx);
        }
        else
            return base_t::format(stream_number::null_str, ctx);
    }
};

struct bad_tree : std::exception {
    optdata_t first;
    explicit bad_tree(optdata_t fst) :first(fst) {}
    const char* what() const override {
        return "Invalid attempt to add a node whose parent does not exist";
    }
};

node* bfs_short_build(std::ranges::input_range auto&& rng) {
    node* root{};
    node* cur{};
    std::queue<node*> que;
    const auto new_node = [](const data_t& v) { return new node(v);};
    for (int mode = -1; const optdata_t& val : rng) {
        if (mode == -1) {
            if (!val) throw bad_tree{val};
            que.push(root = new_node(*val));
            mode = 0;
        }
        else if (mode == 0) {
            if (que.empty()) throw bad_tree{val};
            cur = que.front();
            que.pop();
            if (val) que.push(cur->left = new_node(*val));
            mode = 1;
        }
        else {
            if (val) que.push(cur->right = new_node(*val));
            mode = 0;
        }
    }
    return root;
}

node* random_tree(std::pair<data_t, data_t> values = {-9,40}, std::pair<unsigned, unsigned> depths= { 4,6 }, unsigned null_prob=9) {
    node* root{};
    auto& rnd = utils::thread_random();
    const auto new_node = [&]() { return new node(rnd.next<data_t>(values.first, values.second)); };
    const auto max_depth = rnd.next(depths.first, depths.second);
    auto creator = [&](node*& n, int d) -> node* {
        return n = (d < max_depth && rnd.next(null_prob)) ? new_node() : nullptr;
    };

    struct visitor {
        decltype(creator) creator;
        void operator()(node* n, int d=0) {
            if (!n) return;
            ++d;
            this->operator()(creator(n->left,d), d);
            this->operator()(creator(n->right,d), d);
        }
    };

    visitor{std::move(creator)}(root = new_node());
    return root;
}

struct color_path_fmt {
    const path_data path;
    std::optional<pretty_tree::colored_segment> src;
    void operator()(auto& style, node const* node) {
        if (!src) src = style.segment;
        auto& seg = style.segment;
        seg = *src;
        if (path.is_present(node))
        {
            seg.data_col = pretty_tree::ansi_colors::red;
            if (path.is_present(node->left)) seg.left_col = pretty_tree::ansi_colors::red;
            if (path.is_present(node->right)) seg.right_col = pretty_tree::ansi_colors::red;
        }
    }
};

struct char_path_fmt {
    const path_data path;
    std::optional<pretty_tree::plain_segment> src;
    void operator()(auto& style, node const* node) {
        if (!src) src = style.segment;
        auto& seg = style.segment;
        seg = *src;
        if (path.is_present(node))
        {
            if (path.is_present(node->left)) seg.lfill = '<', seg.open='|';
            if (path.is_present(node->right)) seg.rfill = '>', seg.close='|';
        }
    }
};


void test() {
    const auto check = [](std::initializer_list<optdata_t> nodes, std::initializer_list<data_t> exp_path) {
        auto root = bfs_short_build(nodes);
        auto path = find_max_path(root);
        if (!std::ranges::equal(path.data, exp_path, {}, [](node const* q) {return q->value;})) {
            std::print("Test was NOT passed, tree:\n{}\nexpected:\n{}\nfound:\n{}\n", 
                pretty_tree::print_tree(root), path.format_path(), utils::make_joiner<"->">(exp_path));
            throw false;
        }
    };
    constexpr auto x = std::nullopt;

    check({}, {});
    check({-10}, {-10});
    check({-10,-20,-30}, {-10});
    check({-10,30,20}, {30,-10,20});
    check({10,x,3,x,7,x,13,14,15,24,x,26,27,28,29,30,31}, {29,24,14,13,15,26,31});
    check({1,2,3,4,5,6,7,8,9}, {9,4,2,1,3,7});
    check({1,9,2,3,x,5,6,7,8,9}, { 8,3,9,1,2,5,9 });
    check({ 1,-2,-3 }, {1});
    check({ -4,-3,20,-10, -2, -10, -1 }, {20});
    check({ 4,3,2,1,2,-10,1 }, {2,3,4,2,1});
    check({ -10, 90, 20, x, x, 15, 7 }, { 90,-10,20,15 });
    check({7,6,5,-9,8,4,3,6}, {8,6,7,5,4});
    //bfs_short_build(std::vector<optdata_t>{1,x,x,3}); MUST THROW
}

int main(int argc, char* argv[]) {
#ifndef NDEFBUG
    test();
    pretty_tree::tests::run();
#endif
    constexpr std::string_view no_cols = "-nc";

    std::print("Welcome to Path Finder 1.0(2024.11.12) by PavelPI\n");
    const bool is_random = !(argc > 1 && *argv[1] != '\0');
    bool colors = !(argc > 2 && std::string_view{ argv[2] } == no_cols);
    if (colors) colors = utils::enable_coloring().value_or(true);

    bool ok = true;
    node* root{};
    if (!is_random) {
        std::istringstream ss{ argv[1] };
        std::vector<stream_number> nums;
        //Why "ss >> std::ws"? - if the input ends with spaces, then std::ws set eofbit
        //Why "(ss >> std::ws).good()" - unlike operator bool, which is true even if eofbit is set, the good() excludes this bit
        while (ss.good() && (ss >> std::ws).good()) { ss >> nums.emplace_back(); }
        ok = static_cast<bool>(ss);
        const std::string input_str = std::move(ss).str();
        if (ok) {
            try
            {
                root = bfs_short_build(nums);
            }
            catch (const bad_tree& err) {
                std::print("Incorrect tree: {}, input: {:?}", err.what(), input_str);
                ok = false;
            }
        } else
            std::print("Unparsable input: {:?}", input_str);
    } else
        root = random_tree();

    if (ok) {
        const auto path = find_max_path(root);
        constexpr static std::string_view tree_msg = "Tree with max sum of path:\n{}";
        if (colors)
            std::print(tree_msg, pretty_tree::main_style::print_tree(root, pretty_tree::colored_segment{}, color_path_fmt{ path }));
        else
            std::print(tree_msg, pretty_tree::main_style::print_tree(root, pretty_tree::plain_segment{}, char_path_fmt{ path }));
        
        std::print("\nSum={}, path={}", path.sum, path.format_path());
    }
    std::print(R"(
---------------------------
HINT. Command line arguments: first - input tree, second "{}" - disable colors
HINT2. Input tree - it's a binary tree that determine by listing of its nodes in the order of traversal by levels.
For non-existent nodes, use "{}" (without quotes)
)", no_cols,  stream_number::null_str);

    node::free(root);
    return 0;
}
