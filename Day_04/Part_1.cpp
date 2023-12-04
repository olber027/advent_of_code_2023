#include <iostream>
#include <fstream>
#include <vector>
#include <SmartString.h>

struct Ticket {
    std::vector<int> winningNumbers{};
    std::vector<int> playerNumbers{};

    [[nodiscard]] int getScore() const {
        int score = 0;
        for(auto number : winningNumbers) {
            if(std::find(playerNumbers.begin(), playerNumbers.end(), number) != playerNumbers.end())
            {
                if(score < 1) {
                    score = 1;
                } else {
                    score *= 2;
                }
            }
        }
        return score;
    }
};

int main() {
    std::ifstream input_file("/mnt/c/Users/Matt/CLionProjects/advent_of_code_2023/Day_04/input_data/input.txt");
    std::string   l;
    std::vector<Ticket> tickets;

    while(std::getline(input_file, l))
    {
        Utilities::SmartString line(l);
        auto splitLine = line.split<Utilities::SmartString>(":");
        auto numbers = splitLine[1].strip().split<Utilities::SmartString>("|");
        auto winningNumbers = numbers[0].strip().split<Utilities::SmartString>();
        auto playerNumbers = numbers[1].strip().split<Utilities::SmartString>();

        Ticket currentTicket;

        for(auto& number : winningNumbers) {
            if(!number.strip().isEmpty()) {
                currentTicket.winningNumbers.push_back(number.convert<int>());
            }
        }

        for(auto& number : playerNumbers) {
            if(!number.strip().isEmpty()) {
                currentTicket.playerNumbers.push_back(number.convert<int>());
            }
        }

        tickets.push_back(currentTicket);
    }
    input_file.close();

    int totalScore = 0;
    for(const auto& ticket : tickets) {
        totalScore += ticket.getScore();
    }

    std::cout << totalScore << std::endl;

    return 0;
}
