#include <iostream>
#include <unordered_map>
#include <string>
#include <cstddef>
#include <algorithm>
#include <print>

//Лабораторная работа 3. Хеширование
//Составьте хеш-таблицу, содержащую буквы и количество их вхождений во введенной строке.
//Вывести таблицу на экран. 
//Осуществить поиск введенной буквы в хеш-таблице.

int main() {
    std::string input;
    std::println("Input string:");
    std::getline(std::cin, input);

    std::unordered_map<char, std::size_t> letters;

    for (char ch : input) {
        if (std::isalpha(ch)) letters[ch]++;
    }

    std::println("Letter table:");
    for (const auto& [ch, count] : letters) {
        std::println("{}: {}", ch, count);
    }

    char ch;
    std::println("Enter a letter for search: ");
    std::cin >> ch;

    if (auto search = letters.find(ch); search != letters.end())
        std::print("Letter: {}, occurrences: {}", search->first, search->second);
    else
        std::print("Not found");
}