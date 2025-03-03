#include <fstream>
#include <vector>
#include <regex>
#include <iostream>

using namespace std;

bool valid_passport(const vector<string> data, bool part2 = false) {
    vector<string> tokens;
    for (auto d : data) {
        size_t pos = 0;
        string token;
        while ((pos = d.find(' ')) != string::npos) {
            token = d.substr(0, pos);
            tokens.push_back(token);
            d.erase(0, pos + 1);
        }
        tokens.push_back(d);
    }

    string byr, iyr, eyr, hgt, hcl, ecl, pid, cid;
    for (auto token : tokens) {
        string lhs = token.substr(0, token.find(':'));
        string rhs = token.substr(token.find(':') + 1, token.length() - 1);
        if (lhs == "byr") {
            byr = rhs;
        } else if (lhs == "iyr") {
            iyr = rhs;
        } else if (lhs == "eyr") {
            eyr = rhs;
        } else if (lhs == "hgt") {
            hgt = rhs;
        } else if (lhs == "hcl") {
            hcl = rhs;
        } else if (lhs == "ecl") {
            ecl = rhs;
        } else if (lhs == "pid") {
            pid = rhs;
        } else if (lhs == "cid") {
            cid = rhs;
        }
    }

    if (!part2)
        return !(byr.empty() || iyr.empty() || eyr.empty() || hgt.empty() || hcl.empty() || ecl.empty() || pid.empty());
    else {
        return (
                (byr.length() == 4 && stoi(byr) >= 1920 && stoi(byr) <= 2002)
                && (iyr.length() == 4 && stoi(iyr) >= 2010 && stoi(iyr) <= 2020)
                && (eyr.length() == 4 && stoi(eyr) >= 2020 && stoi(eyr) <= 2030)
                && (regex_match(hgt, regex(R"((\d+)(cm|in))")) &&
                    ((hgt.find("cm") != string::npos && stoi(hgt.substr(0, hgt.size() - 2)) >= 150 && stoi(hgt.substr(0, hgt.size() - 2)) <= 193) ||
                     (hgt.find("in") != string::npos && stoi(hgt.substr(0, hgt.size() - 2)) >= 59 && stoi(hgt.substr(0, hgt.size() - 2)) <= 76)))
                && (regex_match(hcl, regex(R"(#[0-9a-f]{6})")))
                && (regex_match(ecl, regex(R"(amb|blu|brn|gry|grn|hzl|oth)")))
                && (regex_match(pid, regex(R"(\d{9})"))));
    }
}

int main() {
    ifstream input("input");
    string str;

    vector<string> passport_data;
    int part1 = 0, part2 = 0;
    while (getline(input, str)) {
        if (str.empty()) {
            if (valid_passport(passport_data))
                part1++;
            if (valid_passport(passport_data, true))
                part2++;

            passport_data.clear();
        } else {
            passport_data.push_back(str);
        }
    }
    // for the last passport, there is an empty line after the last line in the input file,
    // so it's not being considered in the loop above
    if (valid_passport(passport_data))
        part1++;
    if (valid_passport(passport_data, true))
        part2++;

    cout << "Part 1: " << part1 << endl;
    cout << "Part 2: " << part2 << endl;
}
