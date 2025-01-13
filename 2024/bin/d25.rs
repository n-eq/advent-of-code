use aoc::*;

type Key = [usize; 5];
type Lock = [usize; 5];

fn parse_lock(block: &str) -> Lock {
    let lines = block.split("\n").collect::<Vec<&str>>();
    let mut res: Lock = [0; 5];
    for (i, item) in res.iter_mut().enumerate() {
        let mut j = 1;
        while lines[j].chars().nth(i).unwrap() == '#' {
            j += 1;
        }
        *item = j - 1;
    }
    res
}

fn parse_key(block: &str) -> Key {
    let lines = block.split("\n").collect::<Vec<&str>>();
    let mut res: Lock = [0; 5];
    for (i, item) in res.iter_mut().enumerate() {
        let mut j = 5;
        while lines[j].chars().nth(i).unwrap() == '#' {
            j -= 1;
        }
        *item = 5 - j;
    }
    res
}

fn main() {
    let mut keys: Vec<Key> = vec![];
    let mut locks: Vec<Lock> = vec![];
    for block in std::fs::read_to_string(input!()).unwrap().split("\n\n") {
        if &block[0..5] == "#####" {
            locks.push(parse_lock(block));
        } else {
            keys.push(parse_key(block));
        }
    }

    let mut res = 0usize;
    for lock in locks {
        for key in &keys {
            if std::iter::zip(key, lock).all(|(k, l)| k + l <= 5) {
                res += 1;
            }
        }
    }
    println!("{res:?}");
}
