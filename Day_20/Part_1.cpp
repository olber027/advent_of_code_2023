#include <iostream>
#include <fstream>
#include <vector>
#include <SmartString.h>

int main() {
    std::ifstream input_file("/mnt/c/Users/Matt/CLionProjects/advent_of_code_2023/Day_20/input_data/input.txt");
    std::string   l;
    while(std::getline(input_file, l))
    {
        Utilities::SmartString line(l);
        auto splitLine = line.split<Utilities::SmartString>();
    }
    input_file.close();

    return 0;
}
