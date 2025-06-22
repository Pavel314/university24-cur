#include <stdexcept>
#include <cstddef>
#include <format>
#include <utility>
#include <string>
#include <array>
#include <istream>
#include <fstream>
#include <ostream>
#include <iterator>
#include <vector>
#include <ranges>
#include <algorithm>
#include <print>
#include <filesystem>
#include <functional>
#include "utils.hpp"

//Лабораторная работа 10. Вариант 10. Задание 11
//В файле, содержащем фамилии студентов и их оценки, изменить на
//прописные буквы фамилии тех студентов, которые имеют средний балл за национальной шкалой более «4»
struct bad_student : std::runtime_error {
	using std::runtime_error::runtime_error;
};
struct bad_surname: bad_student {
	explicit bad_surname():bad_student("Failed to parse the surname") {}
};
struct bad_grade: bad_student {
	size_t ind;
	explicit bad_grade(size_t ind):bad_student(std::format("Failed to parse the grade, grade index is {}", ind)), ind{ind} {}
};

struct student {
	constexpr static size_t grades_count = 3;
	using grade_type = unsigned short;
	constexpr static auto grades_range = std::pair<grade_type, grade_type>{ 0,5 };

	std::string surname;
	std::array<grade_type, grades_count> grades{};

	friend auto& operator>>(std::istream& is, student& stud) {
		 if (!std::istream::sentry{is}) return is;
		 if (!(is >> stud.surname) || stud.surname.empty()) throw bad_surname{};
		 for (size_t ind{1}; auto& grade : stud.grades) {
			if (!(is >> grade) || grade < grades_range.first || grade > grades_range.second) throw bad_grade(ind);
			++ind;
		}
		return is;
	}

	friend std::ostream& operator<<(std::ostream& os, student& stud) {
		os << stud.surname << ' ';
		ranges::copy_n(ranges::begin(stud.grades), stud.grades.size() - 1, std::ostream_iterator<student::grade_type>(os, " "));
		os << stud.grades.back();
		return os;
	}
};

auto& write_test_data(std::ostream& os) {
	constexpr static std::array surnames{
		"Ivanov", "Petrov", "Sidorov",  "Smirnov", "Kuznetsov", 
		"Popov", "Lebedev", "Novikov", "Morozov", "Fedorov"};
	
	auto& rnd = utils::thread_random();
	for (int n = rnd.next(20, 25); os && n--;) {
		student stud{ surnames[rnd.next<size_t>(0, surnames.size() - 1)] };
		ranges::generate(stud.grades, rnd.generator<student::grade_type>(2, student::grades_range.second));
		os << stud << '\n';
	}
	return os;
}

struct printer {
	struct entry:student {
		double average;
		bool updated;
	};
	std::vector<entry> entries;
	
	template<class T>
	struct table_layout:std::array<T, student::grades_count + 2> {
		T& surname = *this->begin();
		constexpr inline auto grades() noexcept { return ranges::subrange(this->begin() + 1, this->begin() + 1 + student::grades_count); }
		T& average = *(this->end() - 1);
	};

	static auto get_rows() {
		table_layout<std::string> rows;
		rows.surname = "Surname";
		ranges::generate(rows.grades(), [ind = 0]() mutable {return std::format("Subject{}", ++ind); });
		rows.average = "Average";
		return rows;
	}

	friend auto& operator<< (std::ostream& os, const printer& v) {
		constexpr static int after_point = 2;
		const static table_layout<std::string> rows{get_rows()};
		table_layout<size_t> sizes;

		ranges::transform(rows, sizes.begin(), ranges::size);
		const auto mark = [](bool b) {return b ? "*" : "";};
		for (const entry& ent : v.entries) {
			sizes.surname = std::max(sizes.surname, ent.surname.size());
			sizes.average = std::max(sizes.average, std::formatted_size("{:.{}f}{}", ent.average, after_point, mark(ent.updated)));
		}

		using std::get;
		const auto print = [&os]<class... Args>(std::format_string<Args...> fmt, Args&&... args) {std::print(os, fmt, std::forward<decltype(args)>(args)...); };

		ranges::for_each(views::zip(rows, sizes), [&print](const auto& tup) {print("|{:^{}}|", get<0>(tup), get<1>(tup));});
		print("\n");
		for (const entry& ent : v.entries) {
			print("|{:<{}}|", ent.surname, sizes.surname);
			ranges::for_each(views::zip(ent.grades, sizes.grades()), [&print](const auto& tup) {print("|{:^{}}|", get<0>(tup), get<1>(tup));});
			std::string aver;
			std::format_to(std::back_inserter(aver), "{:.{}f}{}", ent.average, after_point, mark(ent.updated));
			print("|{:^{}}|\n", aver, sizes.average);
		}
		return os;
	}

};

int main(int argc, char* argv[]) {
	using std::println, std::print;

	const auto args = utils::get_u8args(argc, argv);

	if (args.size() < 1) {
		println("Error. The program was started without specifying a startup path");
		return 1;
	}
	const fs::path path = fs::path{ args[0] }.replace_filename("lr10_11_file.txt");

	std::print(1 + R"(
Welcome to Laboratory work 10, task 11 (2025.01.10) by PavelPI
A text file containing the names of the students and their grades is given, the count of grades is {}.
Each grade is a number in the range {}
Goal: Change the surnames of those students who have a average rating above 4 to capital letters.
Input&Output file: {}
================================================================
)", student::grades_count, student::grades_range, path);

	std::fstream fs{ path };
	if (!fs) {
		println("Error. Input file {} is not available", path);
		if (!utils::request_type<utils::yesbool>("Would you like to create a test file? [Y/n]: ")) return 0;
		fs = std::fstream(path, std::fstream::in | std::fstream::out | std::fstream::trunc);
		if (!write_test_data(fs)) {
			println("Error. Test file {} is not available", path);
			return 2;
		}
		println("So test file has been created");
		fs.seekg(0);
		fs.seekp(0);
	}

	printer printer{};
	student stud{};
	size_t ind{ 1 };
	try {
		//We construct sentry to skip all spaces(std::ws) and thus get the correct position of the beginning of the surname.
		for (;std::istream::sentry(fs);++ind) {
			const auto stud_pos = fs.tellg();
			fs >> stud;
			const auto back_pos = fs.tellg();

			const auto average = ranges::fold_left(stud.grades, 0.0, std::plus<>{}) / stud.grades.size();
			const bool upd = average > 4;
			if (upd) {
				ranges::transform(stud.surname, stud.surname.begin(), [](unsigned char c) { return std::toupper(c); });
				fs.seekp(stud_pos);
				fs.write(stud.surname.data(), stud.surname.size());
				fs.seekp(back_pos);
			}
			printer.entries.emplace_back(stud, average, upd);
		}
	} catch (const bad_surname& surname) {
		println("Student #{}: {}", ind, surname.what());
		return 100;
	} catch (const bad_grade& grade) {
		println("Student #{}({}): {}",ind, stud.surname, grade.what());
		return 101;
	}
	std::cout << printer;
	return 0;
}

//https://stackoverflow.com/questions/14522122/difference-between-basic-istreamtellg-and-basic-ostreamtellp