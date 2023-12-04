#include <iostream>
#include <fstream>
#include <vector>
#include <SmartString.h>

struct Game {
    int id{};
    std::vector<std::map<std::string, int>> distributions{};
};

int main() {
    std::ifstream input_file("/mnt/c/Users/Matt/CLionProjects/advent_of_code_2023/Day_02/input_data/input.txt");
    std::string   l;

    std::vector<Game> games;
    while(std::getline(input_file, l))
    {
        Utilities::SmartString line(l);
        auto splitLine = line.split<Utilities::SmartString>(":");

        int gameid = splitLine[0].split<Utilities::SmartString>()[1].convert<int>();

        Game currentGame;
        currentGame.id = gameid;

        auto distributions = splitLine[1].split<Utilities::SmartString>(";");

        for(auto distribution : distributions) {
            distribution.strip();
            std::map<std::string, int> distributionMap {{"red", 0}, {"blue", 0}, {"green", 0}};

            auto colorDistribution = distribution.split<Utilities::SmartString>(", ");

            for(auto color_number_pair : colorDistribution) {
                auto cnp = color_number_pair.split<Utilities::SmartString>();
                distributionMap[cnp[1].str()] = cnp[0].convert<int>();
            }
            currentGame.distributions.push_back(distributionMap);
        }
        games.push_back(currentGame);
    }
    input_file.close();

    int idsum = 0;
    std::map<std::string, int> max_values {
        {"red", 12}, {"blue", 14}, {"green", 13}
    };
    for(auto& game : games) {
        bool max_value_exceeded = false;
        for(auto& distribution : game.distributions) {
            for(const auto& max_value_pair : max_values) {
                if(distribution[max_value_pair.first] > max_value_pair.second) {
                    max_value_exceeded = true;
                }
            }
        }
        if(!max_value_exceeded) {
            idsum += game.id;
        }
    }

    std::cout << idsum << std::endl;

    return 0;
}
