#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <SmartString.h>

struct Number {
    int value{};
    std::vector<std::pair<int, int>> coordinates{};
};

struct Symbol {
    char sym{};
    std::pair<int, int> coordinates{};

public:
    [[nodiscard]] bool isAdjacent(const Number& other) const {
        for(const auto& coord : other.coordinates) {
            if(std::abs(coordinates.first - coord.first) <= 1 &&
               std::abs(coordinates.second - coord.second) <= 1) {
                return true;
            }
        }
        return false;
    }
};

int main() {
    std::ifstream input_file("/mnt/c/Users/Matt/CLionProjects/advent_of_code_2023/Day_03/input_data/input.txt");
    std::string   l;
    std::vector<Number> numbers;
    std::vector<Symbol> symbols;
    int lineNumber = 0;

    while(std::getline(input_file, l))
    {
        Utilities::SmartString line(l);
        line.strip();
        Utilities::SmartString currentNumberStr;
        Number currentNumber;
        for(int i = 0; i < line.length(); i++) {
            if(std::isdigit(line[i])) {
                currentNumberStr += line[i];
                currentNumber.coordinates.emplace_back(i, lineNumber);
            } else {
                if(!currentNumberStr.isEmpty()) {
                    currentNumber.value = currentNumberStr.convert<int>();
                    numbers.push_back(currentNumber);
                    currentNumber = Number();
                    currentNumberStr.clear();
                }
                if(line[i] == '*') {
                    symbols.push_back({
                        .sym=line[i],
                        .coordinates={i, lineNumber}
                    });
                }
            }
        }
        if(!currentNumberStr.isEmpty()) {
            currentNumber.value = currentNumberStr.convert<int>();
            numbers.push_back(currentNumber);
        }
        lineNumber++;
    }
    input_file.close();

    int gearRatioSum = 0;

    for(const auto& symbol : symbols) {
        std::vector<Number> adjacentNumbers;
        for(const auto& number : numbers) {
            if(symbol.isAdjacent(number)) {
                adjacentNumbers.push_back(number);
            }
        }
        if(adjacentNumbers.size() == 2) {
            gearRatioSum += adjacentNumbers.front().value * adjacentNumbers.back().value;
        }
    }

    std::cout << gearRatioSum << std::endl;

    return 0;
}
