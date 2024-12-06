fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = args.get(1).map_or("input_test", |v| v);

    let part1 = std::fs::read_to_string(input)
        .unwrap()
        .lines()
        .map(|l| l.parse::<usize>().unwrap())
        .collect::<Vec<usize>>()
        .windows(2)
        .filter_map(|w| if w[1] > w[0] { Some(w[1] - w[0]) } else { None })
        .count();

    let part2 = std::fs::read_to_string(input)
        .unwrap()
        .lines()
        .map(|l| l.parse::<usize>().unwrap())
        .collect::<Vec<usize>>()
        .windows(3)
        .map(|w| w[0] + w[1] + w[2])
        .collect::<Vec<usize>>()
        .windows(2)
        .filter_map(|w| if w[1] > w[0] { Some(w[1] - w[0]) } else { None })
        .count();

    println!("{part1:?}");
    println!("{part2:?}");
}
