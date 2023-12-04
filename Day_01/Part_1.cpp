#include <iostream>
#include <fstream>
#include <vector>
#include <SmartString.h>

class ProgressiveWordFinder {
private:
    int value;
    std::string repr;
    int current_index;
public:
    ProgressiveWordFinder(int val, std::string rep) : value(val), repr(rep), current_index(-1) {}
    int checkNext(char c) {
        if(repr[++current_index] != c) {
            current_index = -1;
            if(repr[0] == c) {
                current_index = 0;
            }
            return -1;
        }
        if(current_index == (repr.length() - 1)) {
            current_index = -1;
            return value;
        }
        return -1;
    }
};

std::vector<ProgressiveWordFinder> createWords() {
    return std::vector<ProgressiveWordFinder> {
            {1, "1"},
            {2, "2"},
            {3, "3"},
            {4, "4"},
            {5, "5"},
            {6, "6"},
            {7, "7"},
            {8, "8"},
            {9, "9"}
    };
}

int main() {
    std::ifstream input_file("/mnt/c/Users/Matt/CLionProjects/advent_of_code_2023/Day_01/input_data/input.txt");
    std::string   l;
    int sum = 0;
    while(std::getline(input_file, l))
    {
        int first_digit = -1; int second_digit = -1;
        Utilities::SmartString line(l);
        line.strip();
        std::vector<ProgressiveWordFinder> words = createWords();
        for(char c : line) {
            for(auto& word : words) {
                auto result = word.checkNext(c);
                if(result >= 0) {
                    if(first_digit < 0) {
                        first_digit = result;
                    }
                    second_digit = result;
                }
            }
        }
        auto val = std::max(first_digit, 0) * 10 + std::max(second_digit, 0);
        sum += val;
    }
    input_file.close();
    std::cout << std::endl << sum << std::endl;
    // Correct Answer: 53386
    return 0;
}