use itertools::Itertools;

fn main() {
    let input = std::fs::read_to_string("src/input.txt").unwrap();
    let lines = input.split("\n\n");

    let top3: Vec<u64> = lines
        .map(|block| {
            block
                .lines()
                .map(|l| l.parse::<u64>().unwrap())
                .collect::<Vec<u64>>()
        })
        .collect::<Vec<Vec<u64>>>()
        .iter()
        .map(|vec| vec.iter().sum())
        .sorted()
        .rev()
        .take(3)
        .collect();

    println!("{} {}", top3.get(0).unwrap(), top3.iter().sum::<u64>());
}
