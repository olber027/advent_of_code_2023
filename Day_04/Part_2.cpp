#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <SmartString.h>

struct Ticket {
    int cardNumber{};
    std::vector<int> winningNumbers{};
    std::vector<int> playerNumbers{};

    [[nodiscard]] int getNumberOfWinningNumbers() const {
        int result = 0;
        for(auto number : winningNumbers) {
            if(std::find(playerNumbers.begin(), playerNumbers.end(), number) != playerNumbers.end())
            {
                result ++;
            }
        }
        return result;
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
        auto cardNumber = splitLine[0].strip().split<Utilities::SmartString>().back().strip().convert<int>();
        auto numbers = splitLine[1].strip().split<Utilities::SmartString>("|");
        auto winningNumbers = numbers[0].strip().split<Utilities::SmartString>();
        auto playerNumbers = numbers[1].strip().split<Utilities::SmartString>();

        Ticket currentTicket;
        currentTicket.cardNumber = cardNumber;

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

    std::map<int, int> ticketMap;
    for(const auto& ticket : tickets) {
        int score = ticket.getNumberOfWinningNumbers();
        ticketMap[ticket.cardNumber] += 1;
        for(int j = 0; j < ticketMap[ticket.cardNumber]; j++) {
            for (int i = score; i > 0; i--) {
                ticketMap[ticket.cardNumber + i] += 1;
            }
        }
    }

    int totalTickets = 0;
    for(const auto& ticket : tickets) {
        totalTickets += ticketMap[ticket.cardNumber];
    }

    std::cout << totalTickets << std::endl;

    return 0;
}
