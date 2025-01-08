use aoc::*;
use std::collections::HashMap;

// recursion + memoization using memo hashmap
fn is_possible(towel: &String, patterns: &Vec<String>, memo: &mut HashMap<String, usize>) -> usize {
    if let Some(res) = memo.get(towel) {
        return *res;
    }

    let mut res = 0usize;
    for p in patterns {
        if *p == *towel {
            res += 1;
        }

        if towel.starts_with(&*p) {
            let right = towel.replacen(&*p, "", 1);
            res += is_possible(&right.to_string(), patterns, memo);
        }
    }

    memo.insert(towel.to_string(), res);
    res
}

pub fn main() {
    let mut patterns: Vec<String> = Vec::new();
    let mut towels: Vec<String> = Vec::new();

    for line in std::fs::read_to_string(input!()).unwrap().lines() {
        if line == "" {
            continue;
        }

        if line.contains(",") {
            patterns = line.split(", ").map(|s| s.to_string()).collect();
        } else {
            towels.push(line.to_string());
        }
    }
    //     let patterns = HashSet::from_iter(patterns.into_iter());

    let mut patterns_ext = patterns.clone();

    let mut cache: HashMap<String, usize> = HashMap::new();
    let possible = towels
        .iter()
        .map(|ipt| is_possible(ipt, &mut patterns_ext, &mut cache))
        .filter(|v| *v > 0)
        .collect::<Vec<usize>>();
    println!("part1: {:?}", possible.len());
    println!("part2: {:?}", possible.iter().sum::<usize>());
}
