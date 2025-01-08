use aoc::*;
use std::collections::{HashMap, HashSet};
use std::iter::Sum;
use std::ops::Add;

#[derive(Copy, Clone, Debug, Eq, Hash, PartialEq)]
struct Number(u64);

impl Number {
    fn new(s: &str) -> Number {
        Number(s.parse().unwrap())
    }

    fn mix(&mut self, op: u64) {
        self.0 ^= op;
    }

    fn prune(&mut self) {
        self.0 &= 0xFFFFFF;
    }

    fn next(&self) -> Number {
        let mut res = self.clone();
        res.mix(res.0 << 6);
        res.prune();

        let mut division: f64 = (res.0 >> 5) as f64;
        division = f64::floor(division);
        res.mix(division as u64);
        res.prune();

        res.mix(res.0 << 11);
        res.prune();

        res
    }

    fn price(&self) -> u64 {
        self.0 % 10
    }
}

impl Add for Number {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        Self {
            0: self.0 + other.0,
        }
    }
}

impl Sum<Self> for Number {
    fn sum<I>(iter: I) -> Self
    where
        I: Iterator<Item = Self>,
    {
        iter.fold(Self(0), |a, b| Add::add(a, b))
    }
}

pub fn main() {
    let input = input!();

    let numbers = std::fs::read_to_string(input)
        .unwrap()
        .lines()
        .map(|l| Number::new(l))
        .collect::<Vec<Number>>();

    let secrets_map: HashMap<Number, Vec<Number>> = numbers
        .iter()
        .map(|n| {
            let mut res = n.clone();
            (
                n.clone(),
                (0..2000)
                    .into_iter()
                    .map(|_| {
                        res = res.next();
                        res.clone()
                    })
                    .collect(),
            )
        })
        .collect();
    println!(
        "part1 : {:?}",
        secrets_map
            .values()
            .map(|v| *v.last().unwrap())
            .sum::<Number>()
    );

    let mut max_sequences: HashMap<(usize, usize, usize, usize), i64> = HashMap::new();
    for number in numbers {
        let secrets = secrets_map.get(&number).unwrap();

        let mut seen = HashSet::<(i64, i64, i64, i64)>::new();
        let prices = secrets.iter().map(|s| s.price()).collect::<Vec<u64>>();
        let mut price_diff: Vec<i64> = vec![];
        for i in 1..prices.len() {
            price_diff.push(prices[i] as i64 - prices[i - 1] as i64);
        }

        for (i, diff) in price_diff.iter().enumerate() {
            if i < 3 {
                continue;
            }

            let idx = (i - 3, i - 2, i - 1, i);
            let vals = (
                price_diff[i - 3],
                price_diff[i - 2],
                price_diff[i - 1],
                *diff,
            );

            if seen.insert(vals) {
                if let Some(m) = max_sequences.get_mut(&idx) {
                    *m += prices[i] as i64;
                } else {
                    max_sequences.insert(idx, *diff);
                }
            }
        }
    }

    println!("part2: {:?}", max_sequences.values().max());
}
