use std::collections::HashSet;
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

    let mut bingos: Vec<(
        usize, /* board index */
        usize, /* draw */
        usize, /* result */
    )> = vec![];

    // we need at least 5 draws to make a bingo
    for i in 5..draws.len() {
        let slice = draws[..i].to_vec();

        for (b, board) in boards.iter().enumerate() {
            let mut found = board.iter().any(|l| l.iter().all(|e| slice.contains(e)));

            if !found {
                for col in 0..board[0].len() {
                    let mut vec: Vec<usize> = vec![];
                    for l in 0..board.len() {
                        vec.push(board[l][col]);
                    }

                    if vec.iter().all(|e| slice.contains(e)) {
                        found = true;
                        break;
                    }
                }
            }

            if found {
                let sum = board
                    .iter()
                    .flatten()
                    .filter(|e| !slice.contains(*e))
                    .sum::<usize>();
                bingos.push((b, draws[i - 1], draws[i - 1] * sum));
            }
        }
    }

    println!("part1 {:?}", bingos[0].2);

    let mut part2 = 0usize;
    let mut s: HashSet<usize> = HashSet::new();
    for (b, _, res) in bingos {
        if s.insert(b) && s.len() == boards.len() {
            part2 = res;
            break;
        }
    }
    println!("part2: {part2:?}");
}
