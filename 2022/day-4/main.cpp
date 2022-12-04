// compile && run: g++ main.cpp && ./a.out
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

int main() {

    std::ifstream infile("input");

    int part1 = 0, part2 = 0;
    std::string buf;
    while (std::getline(infile, buf)) {
        int a, b, c, d;

        /* This line is basically a cheat (not really idiomatic C++ one could expect) */
        sscanf(buf.c_str(), "%d-%d,%d-%d", &a, &b, &c, &d);

        if ((a >= c && b <= d) || (c >= a && d <= b)) {
            part1++;
        }

        bool no_overlap = ((a < c && b < c) || (c < a && d < a));
        if (!no_overlap) {
            part2++;
        }
    }

    std::cout << "part1 = " << part1 << "\n";
    std::cout << "part2 = " << part2 << "\n";

    return 0;
}
