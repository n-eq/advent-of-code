#include <cassert>
#include <fstream>
#include <map>
#include <utility>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

int get_seat_id(const std::string str) {
    std::pair<int, int> row_range(0, 127);
    for (int i = 0; i < 7; i++) {
        if (str[i] == 'F') {
            row_range.second = (row_range.first + row_range.second) / 2;
        } else {
            // assume it can only be 'B'
            row_range.first = 1 + (row_range.first + row_range.second) / 2;
        }
    }

    assert(row_range.first == row_range.second);
    int row = row_range.first;

    std::pair<int, int> col_range(0, 7);
    for (int i = 7; i < 10; i++) {
        if (str[i] == 'L') {
            col_range.second = (col_range.first + col_range.second) / 2;
        } else {
            // assume it can only be 'R'
            col_range.first = 1 + (col_range.first + col_range.second) / 2;
        }
    }
    assert(col_range.first == col_range.second);
    int col = col_range.first;

    return row * 8 + col;
}

int main() {
    std::ifstream input("input");
    std::string str;

    std::vector<int> seat_ids;
    std::map<std::string, int> seats;
    while (std::getline(input, str)) {
        // int tmp = get_seat_id(str);
        // std::cout << "tmp = " << tmp << std::endl;
        seat_ids.push_back(get_seat_id(str));
        seats[str] = get_seat_id(str);
    }

    std::sort(seat_ids.begin(), seat_ids.end());
    int part1 = seat_ids[seat_ids.size() - 1];
    std::cout << "Part 1: " << part1 << std::endl;

    int part2 = 0;
    for (int i = 1; i < seat_ids.size(); i++) {
        if (seat_ids[i] - seat_ids[i - 1] == 2) {
            part2 = seat_ids[i] - 1;
            break;
        }
    }
    std::cout << "Part 2: " << part2 << std::endl;
}
