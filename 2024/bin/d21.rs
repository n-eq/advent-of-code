use aoc::*;
use std::collections::HashMap;

#[path = "d21_helper.rs"]
mod helper;

use crate::helper::*;

fn generate_combinations(v: impl Iterator<Item = Directions>) -> Directions {
    v.fold(vec![Vec::new()], |acc, group| {
        let mut new_result = Vec::with_capacity(acc.len() * group.len());
        for (mut prefix, suffix) in itertools::iproduct!(acc, group) {
            prefix.extend(suffix);
            new_result.push(prefix);
        }
        new_result
    })
}

#[inline]
fn get_directions(buttons: Vec<char>) -> Directions {
    let mut state = 'A';
    let map = DIRECTIONS.lock().unwrap();
    let res = buttons.iter().copied().map(|b| {
        let r = map.get(&(state, b)).unwrap();
        state = b;
        r.clone()
    });

    generate_combinations(res)
}

pub type DirectionCache = String;

fn find_shortest_path_length(
    keys: String,
    depth: usize, /* number of robots (part1: 3, part2: 25) */
    cache: &mut HashMap<(String, usize), usize>,
) -> usize {
    if depth == 0 {
        keys.len()
    } else if let Some(val) = cache.get(&(keys.clone(), depth)) {
        *val
    } else {
        let mut total: usize = 0;
        for subkey in keys.split_inclusive("A") {
            total += get_directions(subkey.chars().collect())
                .iter()
                .map(|dir| find_shortest_path_length(String::from_iter(dir), depth - 1, cache))
                .min()
                .unwrap();
        }
        cache.insert((keys, depth), total);
        total
    }
}

pub fn main() {
    let mut cache = HashMap::with_capacity(1000000);
    println!(
        "{:?}",
        std::fs::read_to_string(input!())
            .unwrap()
            .lines()
            .map(|code| {
                let chars = code.chars();
                let num = chars
                    .clone()
                    .filter(|c| c.is_ascii_digit())
                    .collect::<Vec<_>>()
                    .iter()
                    .fold(0, |acc, x| x.to_digit(10).unwrap() + 10 * acc)
                    as usize;

                (
                    num * find_shortest_path_length(code.to_string(), 3, &mut cache),
                    (num * find_shortest_path_length(code.to_string(), 25, &mut cache)),
                )
            })
            .fold((0, 0), |(acc1, acc2), (p1, p2)| (acc1 + p1, acc2 + p2))
    );
}
