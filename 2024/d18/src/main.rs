use std::collections::{HashMap, VecDeque};
type Pos = (isize, isize);

fn bfs(start: Pos, end: Pos, obstacles: Vec<Pos>) -> Option<usize> {
    let mut queue = VecDeque::<Pos>::new();
    queue.push_back(start);

    let mut dist: HashMap<Pos, usize> = HashMap::new();
    dist.insert(start, 0);

    while let Some(current) = queue.pop_front() {
        if obstacles.iter().any(|ob| ob == &current) {
            continue;
        }

        for n in [
            (current.0 + 1, current.1),
            (current.0 - 1, current.1),
            (current.0, current.1 - 1),
            (current.0, current.1 + 1),
        ]
        .iter()
        .filter(|(x, y)| *x >= 0 && *x <= end.0 && *y >= 0 && *y <= end.1)
        {
            // if n is already in dist, (some), we continue
            // otherwise insert the value of current + 1
            if dist.insert(*n, dist.get(&current).unwrap() + 1).is_some() {
                continue;
            }

            if *n == end {
                break;
            }

            queue.push_back(*n);
        }
    }

    dist.get(&end).map(|n| *n)
}

fn parse_falling_bytes(input: String) -> Vec<Pos> {
    input
        .lines()
        .map(|l| {
            let mut s = l.split(',');
            (
                s.next().unwrap().parse::<isize>().unwrap(),
                s.next().unwrap().parse::<isize>().unwrap(),
            )
        })
        .collect::<Vec<Pos>>()
}

fn main() {
    let bytes = parse_falling_bytes(std::fs::read_to_string("input").unwrap());

    let start: Pos = (0, 0);
    let end: Pos = (70, 70);

    let mut part1 = 0usize;
    let mut part2: Pos = (0, 0);
    let mut i = 1024usize;
    while i < bytes.len() {
        if let Some(res) = bfs(start, end, bytes[..i].to_vec()) {
            if i == 1024 {
                part1 = res;
            }
        } else {
            part2 = bytes[i - 1];
            break;
        }

        i += 1;
    }

    println!("{part1:?} {part2:?}");
}
