use itertools::Itertools; // combinations
use std::collections::{HashMap, HashSet};

type Point = (usize, usize);

#[derive(Clone, Debug, PartialEq)]
enum LocationType {
    Empty,
    Frequency(char),
    AntiNode(char),
}

fn symmetricals(p1: Point, p2: Point, bounds: isize) -> Vec<Point> {
    vec![
        (
            2 * p1.0 as isize - p2.0 as isize,
            2 * p1.1 as isize - p2.1 as isize,
        ),
        (
            2 * p2.0 as isize - p1.0 as isize,
            2 * p2.1 as isize - p1.1 as isize,
        ),
    ]
    .iter()
    .filter(|p| !(p.0 < 0 || p.0 >= bounds || p.1 < 0 || p.1 >= bounds))
    .map(|p| (p.0 as usize, p.1 as usize))
    .collect::<Vec<Point>>()
}

fn points_in_line(p1: Point, p2: Point, bounds: isize) -> Vec<Point> {
    let dx: Vec<isize> = vec![p1.0 as isize - p2.0 as isize, p2.0 as isize - p1.0 as isize];
    let dy: Vec<isize> = vec![p1.1 as isize - p2.1 as isize, p2.1 as isize - p1.1 as isize];

    (0..bounds)
        .into_iter()
        .map(|i| {
            let x: Vec<isize> = vec![
                p1.0 as isize + (i as isize) * dx[0],
                p2.0 as isize + (i as isize) * dx[1],
            ];
            let y: Vec<isize> = vec![
                p1.1 as isize + (i as isize) * dy[0],
                p2.1 as isize + (i as isize) * dx[1],
            ];

            x.into_iter()
                .cartesian_product(y)
                .collect::<Vec<(isize, isize)>>()
        })
        .flatten()
        .filter(|(x, y)| !(*x < 0 || *x >= bounds || *y < 0 || *y >= bounds))
        .map(|(x, y)| (x as usize, y as usize))
        .collect()
}

pub fn main() {
    let input = crate::utils::input!();

    let binding = std::fs::read_to_string(input).unwrap();
    let lines: Vec<Vec<char>> = binding
        .lines()
        .map(|l| l.chars().collect::<Vec<char>>())
        .collect();

    let mut map: HashMap<Point, Vec<LocationType>> = HashMap::new();
    for (i, l) in lines.clone().into_iter().enumerate() {
        for (j, c) in l.iter().enumerate() {
            map.insert(
                (i, j),
                vec![match c {
                    '.' => LocationType::Empty,
                    _ => LocationType::Frequency(*c),
                }],
            );
        }
    }

    // { 'A': vec![(0, 1), (4, 5)], 'a': vec![(2, 3,), (1, 1)], ... }
    let mut freq_map: HashMap<char, Vec<Point>> = HashMap::new();
    for (k, vals) in &map {
        for v in vals {
            match v {
                LocationType::Frequency(c) => {
                    if let Some(v) = freq_map.get_mut(c) {
                        v.push(*k);
                    } else {
                        freq_map.insert(*c, vec![*k]);
                    }
                }
                _ => (),
            }
        }
    }

    let mut part1: HashSet<Point> = HashSet::new();
    let mut part2: HashSet<Point> = HashSet::new();
    for (k, v) in freq_map {
        for comb in v.iter().combinations(2).collect::<Vec<_>>() {
            for s in symmetricals(*comb[0], *comb[1], lines.len() as isize) {
                let current = map.get_mut(&(s.0 as usize, s.1 as usize)).unwrap();
                current.push(LocationType::AntiNode(k));

                // remove Empty if there's any (just in case)
                current.retain(|loc| *loc != LocationType::Empty);

                part1.insert((s.0, s.1));
            }

            for p in points_in_line(*comb[0], *comb[1], lines.len() as isize) {
                part2.insert((p.0, p.1));
            }
        }
    }

    println!("part1: {:?} ", part1.len());
    println!("part2: {:?} ", part2.len());
}
