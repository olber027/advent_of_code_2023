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
            std::map<std::string, int> distributionMap;
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

    int total_power = 0;

    for(auto& game : games) {
        std::map<std::string, int> min_required_values {
                {"red", 0},
                {"blue", 0},
                {"green", 0}
        };
        for(auto& distribution : game.distributions) {
            for(const auto& value_pair : distribution) {
                min_required_values[value_pair.first] = std::max(value_pair.second, min_required_values[value_pair.first]);
            }
        }
        auto game_power = 1;
        for(const auto& min_value_pair : min_required_values) {
            game_power *= min_value_pair.second;
        }
        total_power += game_power;
    }

    std::cout << total_power << std::endl;

    return 0;
}
