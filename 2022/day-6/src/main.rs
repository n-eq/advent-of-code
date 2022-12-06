// run: cargo r
use std::collections::HashSet;

fn main() {
    let input = std::fs::read_to_string("input").unwrap();
    let string = input.strip_suffix("\n").unwrap();

    let mut i: usize = 0;
    let part1: usize = 4;
    let part2: usize = 14;

    for window in string.chars().collect::<Vec<char>>().windows(part1) {
        if window.iter().collect::<HashSet<&char>>().len() == part1 {
            println!("part1: {}", i + part1);
            break;
        }
        i = i + 1;
    }

    i = 0;
    for window in string.chars().collect::<Vec<char>>().windows(part2) {
        if window.iter().collect::<HashSet<&char>>().len() == part2 {
            println!("part2: {}", i + part2);
            break;
        }
        i = i + 1;
    }
}
