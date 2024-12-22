const XMAS: &str = "XMAS";

fn part1(lines: &Vec<Vec<char>>) -> usize {
    let mut count: usize = 0;
    for line in 0..lines.len() {
        for col in 0..lines[line].len() {
            if lines[line][col] != 'X' {
                continue;
            }

            // check ->
            if lines[line]
                .get(col..col + 4)
                .unwrap_or_default()
                .iter()
                .collect::<String>()
                == XMAS
            {
                count += 1
            }

            // check <-
            // check col >= 4 to avoid unchecked sub panic
            if col >= 3
                && lines[line]
                    .get(col - 3..=col)
                    .unwrap_or_default()
                    .into_iter()
                    .rev()
                    .collect::<String>()
                    == XMAS
            {
                count += 1;
            }

            // check v
            if [
                lines[line][col],
                lines.get(line + 1).map_or(' ', |l| l[col]),
                lines.get(line + 2).map_or(' ', |l| l[col]),
                lines.get(line + 3).map_or(' ', |l| l[col]),
            ]
            .into_iter()
            .collect::<String>()
                == XMAS
            {
                count += 1;
            }

            // check ^
            if line >= 3
                && [
                    lines[line][col],
                    lines.get(line - 1).map_or(' ', |l| l[col]),
                    lines.get(line - 2).map_or(' ', |l| l[col]),
                    lines.get(line - 3).map_or(' ', |l| l[col]),
                ]
                .into_iter()
                .collect::<String>()
                    == XMAS
            {
                count += 1;
            }

            // check up right
            if line >= 3
                && [
                    lines[line][col],
                    lines
                        .get(line - 1)
                        .map_or(' ', |l| *l.get(col + 1).unwrap_or(&' ')),
                    lines
                        .get(line - 2)
                        .map_or(' ', |l| *l.get(col + 2).unwrap_or(&' ')),
                    lines
                        .get(line - 3)
                        .map_or(' ', |l| *l.get(col + 3).unwrap_or(&' ')),
                ]
                .into_iter()
                .collect::<String>()
                    == XMAS
            {
                count += 1;
            }

            // check up left
            if line >= 3
                && col >= 3
                && [
                    lines[line][col],
                    lines
                        .get(line - 1)
                        .map_or(' ', |l| *l.get(col - 1).unwrap_or(&' ')),
                    lines
                        .get(line - 2)
                        .map_or(' ', |l| *l.get(col - 2).unwrap_or(&' ')),
                    lines
                        .get(line - 3)
                        .map_or(' ', |l| *l.get(col - 3).unwrap_or(&' ')),
                ]
                .into_iter()
                .collect::<String>()
                    == XMAS
            {
                count += 1;
            }

            // check down right
            if [
                lines[line][col],
                lines
                    .get(line + 1)
                    .map_or(' ', |l| *l.get(col + 1).unwrap_or(&' ')),
                lines
                    .get(line + 2)
                    .map_or(' ', |l| *l.get(col + 2).unwrap_or(&' ')),
                lines
                    .get(line + 3)
                    .map_or(' ', |l| *l.get(col + 3).unwrap_or(&' ')),
            ]
            .into_iter()
            .collect::<String>()
                == XMAS
            {
                count += 1;
            }

            // check down left
            if col >= 3
                && [
                    lines[line][col],
                    lines
                        .get(line + 1)
                        .map_or(' ', |l| *l.get(col - 1).unwrap_or(&' ')),
                    lines
                        .get(line + 2)
                        .map_or(' ', |l| *l.get(col - 2).unwrap_or(&' ')),
                    lines
                        .get(line + 3)
                        .map_or(' ', |l| *l.get(col - 3).unwrap_or(&' ')),
                ]
                .into_iter()
                .collect::<String>()
                    == XMAS
            {
                count += 1;
            }
        }
    }

    count
}

fn part2(lines: &Vec<Vec<char>>) -> usize {
    let mut count = 0usize;
    for line in 0..lines.len() {
        for col in 0..lines[line].len() {
            if lines[line][col] != 'A' {
                continue;
            }

            if col == 0 || line == 0 {
                continue;
            }

            let prev_line = lines.get(line - 1);
            let next_line = lines.get(line + 1);
            // c1 c2
            //   A
            // c3 c4
            let (c1, c2, c3, c4) = (
                prev_line.map_or(' ', |l| *l.get(col - 1).unwrap_or(&' ')),
                prev_line.map_or(' ', |l| *l.get(col + 1).unwrap_or(&' ')),
                next_line.map_or(' ', |l| *l.get(col - 1).unwrap_or(&' ')),
                next_line.map_or(' ', |l| *l.get(col + 1).unwrap_or(&' ')),
            );

            if (c1 == c2 && c1 == 'M' && c3 == c4 && c3 == 'S')
                || (c1 == c3 && c1 == 'M' && c2 == c4 && c2 == 'S')
                || (c3 == c4 && c3 == 'M' && c1 == c2 && c1 == 'S')
                || (c2 == c4 && c2 == 'M' && c1 == c3 && c1 == 'S')
            {
                count += 1;
            }
        }
    }
    count
}

pub fn main() {
    let input = crate::utils::input!();

    let lines = std::fs::read_to_string(input)
        .unwrap()
        .lines()
        .map(|l| l.chars().collect())
        .collect::<Vec<Vec<char>>>();

    println!("part1: {:?}", part1(&lines));
    println!("part2: {:?}", part2(&lines));
}
