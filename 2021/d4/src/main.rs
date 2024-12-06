type Board = Vec<Vec<usize>>;

fn parse_input(lines: &Vec<&str>) -> (Vec<usize> /* draws */, Vec<Board>) {
    let draws = lines[0]
        .split(",")
        .map(|s| s.parse::<usize>().unwrap())
        .collect::<Vec<usize>>();

    let mut boards: Vec<Board> = vec![];
    let mut board: Board = vec![];
    for line in &lines[1..] {
        if *line == "\n" || *line == "" {
            if board.len() > 0 {
                boards.push(board);
            }
            board = vec![];
            continue;
        }

        board.push(
            line.split_whitespace()
                .map(|n| n.parse::<usize>().unwrap())
                .collect(),
        );
    }

    (draws, boards)
}

fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = args.get(1).map_or("input_test", |v| v);

    let binding = std::fs::read_to_string(input).unwrap();
    let lines = binding.split("\n").collect::<Vec<&str>>();

    let (draws, boards) = parse_input(&lines);

    let mut results = Vec::<usize>::new();
    'outer: for i in 5..draws.len() {
        let slice = draws[..i].to_vec();

        for board in &boards {
            for line in board {
                if line.iter().all(|e| slice.contains(e)) {
                    // found horizontal
                    let sum = board
                        .into_iter()
                        .map(|l| l.iter().filter(|n| !slice.contains(n)).sum())
                        .collect::<Vec<usize>>()
                        .into_iter()
                        .sum::<usize>();
                    results.push(draws[i - 1] * sum);
                    continue 'outer;
                }
            }

            for col in 0..board[0].len() {
                let mut vec: Vec<usize> = vec![];
                for l in 0..board.len() {
                    vec.push(board[l][col]);
                }

                if vec.iter().all(|e| slice.contains(e)) {
                    // found horizontal
                    let sum = board
                        .into_iter()
                        .map(|l| l.iter().filter(|n| !slice.contains(n)).sum())
                        .collect::<Vec<usize>>()
                        .into_iter()
                        .sum::<usize>();
                    results.push(draws[i - 1] * sum);
                    continue 'outer;
                }
            }
        }
    }

    println!("part1 {:?}", results[0]);
    println!("part2 {:?}", results[results.len() - 1]);
}
