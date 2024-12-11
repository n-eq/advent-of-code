use std::collections::HashMap;

fn blink(stone: usize) -> Vec<usize> {
    if stone == 0 {
        vec![1]
    } else {
        let mut digit_count = 0usize;
        let mut stone_copy = stone;
        while stone_copy != 0 {
            digit_count += 1;
            stone_copy /= 10;
        }

        if digit_count % 2 == 0 {
            let stone_as_string = stone.to_string();
            let splits = stone_as_string.split_at(digit_count / 2);
            let (left, right) = (
                splits.0.parse::<usize>().unwrap(),
                splits.1.parse::<usize>().unwrap(),
            );
            vec![left, right]
        } else {
            vec![stone * 2024]
        }
    }
}

fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = args.get(1).map_or("input_test", |v| v);

    let mut stones: HashMap<usize, usize> = std::fs::read_to_string(input)
        .unwrap()
        .trim_end()
        .split(" ")
        .map(|s| (s.parse::<usize>().unwrap(), 1))
        .collect::<HashMap<usize, usize>>();

    let mut part1 = 0;
    for i in 0..75 {
        if i == 25 {
            part1 = stones.values().sum::<usize>();
        }

        let mut new_stones: HashMap<usize, usize> = HashMap::new();
        for (k, v) in &stones {
            let new_vals = blink(*k);

            for val in new_vals {
                if let Some(stone) = new_stones.get_mut(&val) {
                    *stone += v;
                } else {
                    new_stones.insert(val, *v);
                }
            }
        }
        stones = new_stones;
    }
    let part2 = stones.values().sum::<usize>();

    println!("{part1} {part2}");
}
