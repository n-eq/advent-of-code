use aoc::*;
use std::collections::HashSet;

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
enum Direction {
    North = 0,
    South = 1,
    East = 2,
    West = 3,
}
use Direction::*;

impl Direction {
    fn direction_change(&self, d: Direction) -> bool {
        (matches!(self, South | North) && matches!(d, East | West))
            || (matches!(self, East | West) && matches!(d, South | North))
    }

    fn opposite(&self) -> Direction {
        match self {
            East => West,
            West => East,
            South => North,
            North => South,
        }
    }
}

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

    fn get_successors(&self, p: Point, prev_dir: Direction) -> Vec<((Point, Direction), usize)> {
        [(-1, 0, North), (1, 0, South), (0, 1, East), (0, -1, West)]
            .iter()
            .map(|(dx, dy, d)| ((p.0 + dx, p.1 + dy), d))
            .filter(|(_, d)| *d != &prev_dir.opposite())
            .filter(|((x, y), _)| {
                *x >= 0 && *x < self.data.len() as isize && *y >= 0 && *y < self.data.len() as isize
            })
            .filter(|((x, y), _)| self.data[*x as usize][*y as usize] != '#')
            .map(|(p, d)| (p, *d))
            .map(|(p, dir)| {
                let cost = if dir.direction_change(prev_dir) {
                    1001
                } else {
                    1
                };
                ((p, dir), cost)
            })
            .collect()
    }
}

pub fn main() {
    let input = input!();

    let map = Map::new(std::fs::read_to_string(input).unwrap());

    let part1 = pathfinding::prelude::dijkstra(
        &(map.start, East),
        |&((x, y), d)| map.get_successors((x, y), d),
        |&(p, _)| p == map.end,
    )
    .unwrap()
    .1;

    // pathfinding crate doesn't have a method to return all shortest using Dijkstra
    let all_shortest_paths = pathfinding::directed::astar::astar_bag(
        &(map.start, East),
        |&((x, y), d)| map.get_successors((x, y), d),
        // For the heuristic, we just consider the manhattan distance to the end tile
        |&(p, _)| p.0.abs_diff(map.end.0) + p.1.abs_diff(map.end.1),
        |&(p, _)| p == map.end,
    )
    .unwrap()
    .0;

    let part2 = HashSet::<Point>::from_iter(
        all_shortest_paths
            .into_iter()
            .flat_map(|p| p.iter().map(|(tile, _)| *tile).collect::<Vec<Point>>()),
    )
    .len();
    println!("{part1} {part2}");
}
