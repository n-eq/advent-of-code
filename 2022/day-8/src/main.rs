#[derive(Clone, Debug, PartialEq)]
enum Direction {
    Left,
    Top,
    Right,
    Bottom,
}

fn is_visible_from_anywhere(vec: &Vec<Vec<u32>>, i: usize, j: usize) -> bool {
    [
        Direction::Left,
        Direction::Top,
        Direction::Right,
        Direction::Bottom,
    ]
    .to_vec()
    .iter()
    .any(|d| is_visible(vec, d, i, j))
}

fn scenic_score(forest: &Vec<Vec<u32>>, i: usize, j: usize) -> u32 {
    let mut score = 1;
    let tree = forest[i][j]; // no bounds check
    for d in &[
        Direction::Left,
        Direction::Top,
        Direction::Right,
        Direction::Bottom,
    ] {
        let mut dir_tmp = 0u32;

        // TODO: fix borrow/ownership issues, there should be no need to declare it here
        let mut rev: Vec<u32> = Vec::new();

        let vec_to_check = match d {
            /*
             * We need to loop over the previous list and get their jth element, then
             * reverse before looping
             */
            Direction::Top => {
                for (k, val) in forest.iter().enumerate() {
                    if k >= i {
                        break;
                    }

                    rev.push(val[j]);
                }
                let rev: Vec<_> = rev.iter().rev().collect();
                rev.clone()
            }
            Direction::Bottom => {
                let mut vec_to_check: Vec<&u32> = Vec::new();
                for v in &forest[i + 1..] {
                    vec_to_check.push(&v[j]);
                }

                vec_to_check
            }
            Direction::Left => forest[i][..j].iter().rev().collect(),
            Direction::Right => {
                let mut vec_to_check: Vec<&u32> = Vec::new();
                for e in &forest[i][(j + 1)..] {
                    vec_to_check.push(e);
                }
                vec_to_check
            }
        };

        /* We loop over the vector of values and increment the score until we
         * get a tree higher or equal to our element */
        for e in vec_to_check {
            dir_tmp += 1;
            if e >= &tree {
                break;
            }
        }

        if dir_tmp != 0 {
            score *= dir_tmp;
        }
    }
    score
}

// element [i, j] is visible from the direction specified
fn is_visible(vec: &Vec<Vec<u32>>, dir: &Direction, i: usize, j: usize) -> bool {
    let tree = vec[i][j]; // no bound check!
    let vec_to_check: Vec<u32> = match dir {
        Direction::Top => vec[..i].iter().map(|v| v[j]).collect::<Vec<u32>>(),
        Direction::Bottom => vec[i + 1..].iter().map(|v| v[j]).collect::<Vec<u32>>(),
        Direction::Left => vec[i][..j].to_vec(),
        Direction::Right => vec[i][(j + 1)..].to_vec(),
    };

    vec_to_check.iter().any(|e| *e >= tree)
}

fn main() {
    let input = include_str!("input");
    // let input = include_str!("input_test");

    let lines: Vec<&str> = input.split("\n").collect();

    let lines = lines
        .iter()
        .map(|l| {
            l.chars()
                .map(|c| c.to_digit(10).unwrap())
                .collect::<Vec<u32>>()
        })
        .filter(|l| l != &[])
        .collect::<Vec<Vec<u32>>>();

    let mut part1 = 0;

    let number_of_lines = lines.len();
    let line_size = lines[0].len();

    // part2
    let mut scenic_scores: Vec<u32> = Vec::with_capacity(lines[0].len() * number_of_lines);

    let mut i = 1;
    while i < number_of_lines {
        if lines.get(i + 1).is_none() {
            break; // we reached the end
        }

        let mut j = 1;
        while j < line_size - 1 {
            if is_visible_from_anywhere(&lines, i, j) {
                part1 += 1;
            }

            let score = scenic_score(&lines, i, j);
            scenic_scores.push(score);
            j += 1
        }

        i += 1;
    }

    part1 += 4 * (lines[0].len() - 1); // 4 sides of the rectangle
    println!("part1: {}", part1);
    println!("part2: {}", scenic_scores.iter().max().unwrap());
}
