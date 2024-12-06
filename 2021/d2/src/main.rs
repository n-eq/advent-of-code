fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = args.get(1).map_or("input_test", |v| v);

    let binding = std::fs::read_to_string(input).unwrap();
    let instructions = binding.lines().collect::<Vec<&str>>();

    let (mut hor, mut depth) = (0isize, 0isize);
    let (mut hor2, mut depth2, mut aim) = (0isize, 0isize, 0isize);

    for i in instructions {
        let mut splits = i.split(" ");
        let instruction = splits.next().unwrap();
        let step = splits.next().unwrap().parse::<isize>().unwrap();

        match instruction {
            "forward" => {
                hor += step;
                hor2 += step;
                depth2 += aim * step;
            }
            "down" => {
                depth += step;
                aim += step;
            }
            "up" => {
                depth -= step;
                aim -= step;
            }
            _ => unreachable!(),
        }
    }

    println!("part1 {}", hor * depth);
    println!("part2 {}", hor2 * depth2);
}
