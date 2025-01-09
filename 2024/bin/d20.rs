use aoc::*;
use itertools::Itertools;
use std::collections::{HashMap, HashSet};

type Point = (isize, isize);

#[derive(Debug)]
struct Map {
    data: Vec<Vec<char>>,
    start: Point,
    end: Point,
}

impl Map {
    fn new(input: String) -> Self {
        let mut data: Vec<Vec<char>> = vec![];
        let mut start: Point = (0, 0);
        let mut end: Point = (0, 0);

        for (i, line) in input.lines().enumerate() {
            let mut chars = line.chars();
            data.push(chars.clone().collect());
            if line.contains('S') {
                start = (
                    i as isize,
                    chars.clone().position(|c| c == 'S').unwrap() as isize,
                );
            }
            if line.contains('E') {
                end = (i as isize, chars.position(|c| c == 'E').unwrap() as isize);
            }
        }

        Self { data, start, end }
    }

    fn get_neighbors_with_cheat(&self, p: Point, cheat: usize) -> HashSet<Point> {
        let mut res = Vec::new();
        for dx in 0..=cheat {
            for dy in 0..=cheat - dx {
                if dx == 0 && dy == 0 {
                    continue;
                }
                res.push((p.0 + dx as isize, p.1 + dy as isize));
                res.push((p.0 + dx as isize, p.1 - dy as isize));
                res.push((p.0 - dx as isize, p.1 + dy as isize));
                res.push((p.0 - dx as isize, p.1 - dy as isize));
            }
        }
        res.into_iter().collect()
    }

    fn count_shorter_paths_with_cheat(&self, max_cheat: usize, threshold: usize) -> usize {
        let path = pathfinding::prelude::dijkstra(
            &self.start,
            |&p| self.get_successors_no_cheat(p),
            |&p| p == self.end,
        )
        .unwrap()
        .0;

        let path_values = path
            .iter()
            .enumerate()
            .map(|(i, p)| (p, i))
            .collect::<HashMap<_, _>>();

        let cheats = path
            .iter()
            .flat_map(|pt| {
                self.get_neighbors_with_cheat(*pt, max_cheat)
                    .into_iter()
                    .filter_map(|n| {
                        let d = pt.0.abs_diff(n.0) + pt.1.abs_diff(n.1);
                        let nv = path_values.get(&n).unwrap_or(&0);
                        let pv = path_values[pt];
                        (*nv > (pv + d)).then(|| *nv - pv - d)
                    })
            })
            .filter(|&c| c >= threshold)
            .collect_vec();

        cheats.len()
    }

    fn get_successors_no_cheat(&self, p: Point) -> Vec<(Point, usize)> {
        [(-1, 0), (1, 0), (0, 1), (0, -1)]
            .iter()
            .map(|(dx, dy)| (p.0 + dx, p.1 + dy))
            .filter(|(x, y)| {
                *x >= 0 && *x < self.data.len() as isize && *y >= 0 && *y < self.data.len() as isize
            })
            .filter(|(x, y)| self.data[*x as usize][*y as usize] != '#')
            .map(|p| (p, 1))
            .collect()
    }
}

pub fn main() {
    let input = input!();

    let map = Map::new(std::fs::read_to_string(input).unwrap());

    println!("part1: {:?}", map.count_shorter_paths_with_cheat(2, 100));
    println!("part2: {:?}", map.count_shorter_paths_with_cheat(20, 100));
}
