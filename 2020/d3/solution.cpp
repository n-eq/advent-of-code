#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    ifstream input("input");

    vector<string> vec;
    string str;
    while (getline(input, str)) {
        vec.push_back(str);
    }
    int width = vec[0].length();
    int max_depth = vec.size();

    vector<pair<int, int>> slopes = {{1, 1}, {3, 1}, {5, 1}, {7, 1}, {1,2}};
    vector<long> res;
    for (auto slope: slopes) {
        tuple<int, int> current(0, 0);

        int tmp = 0;
        while (get<1>(current) < max_depth) {

            if (vec[get<1>(current)][get<0>(current) % width] == '#') {
                tmp++;
            }

            get<0>(current) += slope.first;
            get<1>(current) += slope.second;
        }
        res.push_back(tmp);
    }

    cout << "Part 1: " << res[1] << endl;
    long p2 = 1;
    for (auto i : res) 
        p2 *= i;
    cout << "Part 2: " << p2 << endl;
}
