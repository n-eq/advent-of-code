#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main() {
    std::ifstream fileIn("input");
    
    std::string str;
    std::vector<int> data;

    while (std::getline(fileIn, str)) {
        data.push_back(std::stoi(str));
    }

    for (auto first = data.begin(); first != data.end(); ++first) {
        for (auto second = first + 1; second != data.end(); ++second) {
            if (*first + *second == 2020) {
                std::cout << "Part1: " << *first * *second << std::endl;
            }
        }
    }

    for (auto first = data.begin(); first != data.end(); ++first) {
        for (auto second = first + 1; second != data.end(); ++second) {
            for (auto third = second + 1; third != data.end(); ++third) {
                if (*first + *second + *third == 2020) {
                    std::cout << "Part2: " << *first * *second * *third << std::endl;
                }
            }
        }
    }

    fileIn.close();
}
