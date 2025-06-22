#include <ostream>
#include <iterator>
#include <print>
#include <filesystem>
#include <fstream>
#include <cstddef>
#include <algorithm>
#include <string>
#include <limits>
#include "utils.hpp"

//Лабораторная работа 10. Вариант 10. Задание 10
//Создать программу, переписывающую в текстовый файл g содержимое
//файла f, исключая пустые строки, а остальные дополнить справа пробелами или ограничить до n символов

auto& write_test_data(std::ostream& os) {
	auto& rnd = utils::thread_random();
	std::ostreambuf_iterator<char> it{ os };
	for (int lines = rnd.next(26, 32); os && lines--;) {
		//We use short, which can lead to the generation of unnecessary bits for char type
		ranges::generate_n(it, rnd.next(0, 16), rnd.generator<short>('a', 'z'));
		*it++ = '\n';
	}
	return os;
}

int main(int argc, char* argv[]) {
	using std::println, std::print;
	const auto args = utils::get_u8args(argc, argv);

	if (args.size() < 1) {
		println("Error. The program was started without specifying a startup path");
		return 1;
	}

	const fs::path base_path = fs::path{ args[0] }.remove_filename();
	const fs::path input_path = base_path / "lr10_10_input.txt";
	const fs::path output_path = base_path / "lr10_10_output.txt";

	std::print(1 + R"(
Welcome to Laboratory work 10, task 10 (2025.01.09) by PavelPI
Goal: Copy non-empty lines from input file to the output file,
with limiting the line length to N characters, otherwise perform the right padding with spaces.
Input file: {}
Output file: {}
================================================================
)", input_path, output_path);

	std::fstream in{ input_path };
	if (!in) {
		println("Error. Input file {} is not available", input_path);
		if (!utils::request_type<utils::yesbool>("Would you like to create a test file? [Y/n]: ")) return 0;
		in = std::fstream(input_path, std::fstream::in | std::fstream::out | std::fstream::trunc);
		if (!write_test_data(in)) {
			println("Error. Test file {} is not available", input_path);
			return 2;
		}
		println("So test file has been created");
		in.seekg(0);
		in.seekp(0);
	}

	const size_t n = utils::request_type<size_t>("Enter the maximum string length(N): ", [](auto v) {return v > 0;});
	std::string str(n, ' ');

	std::ofstream out{ output_path };
	if (!out) {
		println("Error. Output file {} could not be created", output_path);
		return 3;
	}

	println("The following lines have been written:");
	size_t total{};

	for (bool isf = true; out && in.good() && (!in.getline(str.data(), n + 1).bad());) {
		const auto readed = in.gcount() - (in.gcount() > 0 && in.good());
		if (in.fail() && !in.eof()) {
			in.clear();
			in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		if (readed > 0) {
			ranges::fill(str | std::views::drop(readed), ' ');
			if (!std::exchange(isf, false)) out.put('\n');
			out.write(str.data(), str.size());
			println("{:?}", str);
			++total;
		}
	}

	if (out)
		println("OK. Total lines were written: {}", total);
	else
		println("Error. The output stream is corrupted");
	return 0;
}

//https://stackoverflow.com/questions/76535858/why-is-the-utf-8-flag-in-msvc-not-allowing-my-program-to-display-unicode-charac
//https://stackoverflow.com/questions/47690822/possible-to-force-cmake-msvc-to-use-utf-8-encoding-for-source-files-without-a-bo
//https://stackoverflow.com/questions/5408730/what-is-the-encoding-of-argv
//https://stackoverflow.com/questions/54004000/why-is-stdfilesystemu8path-deprecated-in-c20
//https://stackoverflow.com/questions/57603013/how-to-safely-convert-const-char-to-const-char8-t-in-c20