#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

int main() {
    std::ifstream input("input");
    std::string str;

    int valid_p1 = 0;
    int valid_p2 = 0;
    while (std::getline(input, str)) {
        std::istringstream iss(str);
        int x, y;
        char c;
        std::string d;
        std::string s;
        iss >> x >> c >> y >> c >> d >> s;

        auto first = s[x - 1];
        auto second = s[y - 1];
        if ((first == c || second == c) && first != second) {
            valid_p2++;
        }

        int occurrences = static_cast<int>(std::count(s.begin(), s.end(), c));
        if (occurrences >= x && occurrences <= y) {
            valid_p1++;
        }

    }
    std::cout << "Part1: " << valid_p1 << std::endl;
    std::cout << "Part2: " << valid_p2 << std::endl;

    input.close();
}
