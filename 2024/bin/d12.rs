use aoc::*;
use std::collections::{HashMap, HashSet, VecDeque};

type Point = (usize, usize);

#[derive(Hash, Debug, Clone, Copy, PartialEq, Eq)]
enum Direction {
    BottomUp,
    TopDown,
    RightToLeft,
    LeftToRight,
}

#[derive(Hash, Debug, Clone, Copy, PartialEq, Eq)]
struct Side {
    coord: isize,
    dir: Direction,
}

#[derive(Default, Debug, Clone, Copy, PartialEq)]
struct GardenPlot {
    name: char,
    pos: Point,
}

#[derive(Default, Debug, Clone, PartialEq)]
struct Region {
    name: char,
    positions: HashSet<Point>,
}

fn is_adjacent(source: Point, p: Point) -> bool {
    [(0, 1), (0, -1), (1, 0), (-1, 0)]
        .iter()
        .map(|d| (d.0 as isize, d.1 as isize))
        .map(|d| (d.0 + source.0 as isize, d.1 + source.1 as isize))
        .any(|d| d.0 == p.0 as isize && d.1 == p.1 as isize)
}

impl Region {
    fn get_sides(&self) -> usize {
        let mut edges: HashSet<(Point, Point)> = HashSet::new();
        for p in self.positions.clone() {
            for nbr in [
                (p.0 as isize - 1, p.1 as isize),
                (p.0 as isize + 1, p.1 as isize),
                (p.0 as isize, p.1 as isize - 1),
                (p.0 as isize, p.1 as isize + 1),
            ]
            .iter()
            .filter(|(x, y)| *x >= 0 && *y >= 0)
            .map(|(x, y)| (*x as usize, *y as usize))
            {
                if !self.positions.contains(&nbr) {
                    edges.insert((p, nbr));
                }
            }
        }

        let mut sides = 0;
        for (p1, p2) in &edges {
            if p1.1 == p2.1 {
                if let Some(p22) = p2.1.checked_sub(1) {
                    if let Some(p12) = p1.1.checked_sub(1) {
                        if !edges.contains(&((p1.0, p12), (p2.0, p22))) {
                            sides += 1;
                        }
                    }
                }
            } else {
                if let Some(p21) = p2.0.checked_sub(1) {
                    if let Some(p11) = p1.0.checked_sub(1) {
                        if !edges.contains(&((p11, p1.1), (p21, p2.1))) {
                            sides += 1;
                        }
                    }
                }
            }
        }

        sides
    }

    // To calculate the number of sides, we'll use this formula
    // sum of internal angles = (number of sides - 2) / 180
    // for that we need to get only points of the region that are on the edge (points inside
    // shouldn't be considered), i.e.: points that have at most 3 neighbors from the same region
    // for each of those points, compute its internal angle, sum and apply the formula
    fn sides(&self) -> usize {
        if self
            .positions
            .iter()
            .all(|p| p.0 == self.positions.iter().next().unwrap().0)
            || self
                .positions
                .iter()
                .all(|p| p.1 == self.positions.iter().next().unwrap().1)
        {
            return 4;
        }

        // Could have been a hashmap...
        let point_and_neighbors_in_region: HashMap<Point, Vec<Point>> = self
            .positions
            .iter()
            .map(|p| {
                (
                    *p,
                    self.positions
                        .iter()
                        .filter(|point| p != *point)
                        .filter(|point| is_adjacent(**point, *p))
                        .map(|point| *point)
                        .collect::<Vec<Point>>(),
                )
            })
            .collect();

        let sides: Vec<Side> = self
            .positions
            .iter()
            .filter(|position| {
                let mut c = false;
                for (p, n) in &point_and_neighbors_in_region {
                    if p == *position {
                        c = n.len() < 4;
                        break;
                    }
                }
                c
            })
            .map(|position| {
                let res = vec![
                    Side {
                        coord: position.0 as isize - 1,
                        dir: Direction::BottomUp,
                    },
                    Side {
                        coord: position.0 as isize + 1,
                        dir: Direction::TopDown,
                    },
                    Side {
                        coord: position.1 as isize + 1,
                        dir: Direction::LeftToRight,
                    },
                    Side {
                        coord: position.1 as isize - 1,
                        dir: Direction::RightToLeft,
                    },
                ];
                println!("{position:?} res {res:?} before filter ");
                let res = res
                    .iter()
                    .filter(|side| {
                        let mut neighbors: Vec<Point> = vec![];
                        for (p, n) in point_and_neighbors_in_region.iter() {
                            if p == position {
                                neighbors = n.clone();
                                break;
                            }
                        }

                        match side.dir {
                            // line
                            Direction::BottomUp | Direction::TopDown => {
                                neighbors.iter().all(|p| p.0 != side.coord as usize)
                            }
                            // column
                            Direction::LeftToRight | Direction::RightToLeft => {
                                neighbors.iter().all(|p| p.1 != side.coord as usize)
                            }
                        }
                    })
                    .map(|side| *side)
                    .collect::<Vec<Side>>();
                println!("res {res:?} after filter ");
                res
            })
            .flatten()
            .collect::<Vec<Side>>();
        let set: HashSet<Side> = HashSet::from_iter(sides.iter().cloned());
        println!("sides for region {}: {set:?}", self.name);

        sides.len()
    }

    fn perimeter(&self) -> usize {
        self.positions
            .iter()
            .map(|pos| {
                // The perimeter of a region is the number of sides of garden plots
                // in the region that do not touch another garden plot in the same region. The
                let sides = [
                    (-1, 0), /* north */
                    (1, 0),  /* south */
                    (0, 1),  /* west */
                    (0, -1), /* east */
                ];

                let mut res = 4usize;
                for side in sides {
                    if self.positions.iter().any(|p| {
                        (p.0 as isize) == side.0 + pos.0 as isize
                            && (p.1 as isize) == side.1 + pos.1 as isize
                    }) {
                        res -= 1;
                    }
                }
                res
            })
            .sum::<usize>()
    }
}

fn get_map(input: String) -> Vec<GardenPlot> {
    input
        .lines()
        .enumerate()
        .map(|(i, l)| {
            l.chars()
                .enumerate()
                .map(|(j, c)| GardenPlot {
                    name: c,
                    pos: (i, j),
                })
                .collect::<Vec<GardenPlot>>()
        })
        .flatten()
        .collect::<Vec<GardenPlot>>()
}

fn find_regions(garden_plots: &Vec<GardenPlot>) -> Vec<Region> {
    let mut regions = Vec::new();
    let mut visited = HashSet::new(); // Tracks visited positions

    let mut plot_map: HashMap<char, Vec<Point>> = HashMap::new();
    for plot in garden_plots {
        plot_map
            .entry(plot.name)
            .or_insert_with(Vec::new)
            .push(plot.pos);
    }

    let directions = [(-1, 0), (1, 0), (0, -1), (0, 1)];

    for plot in garden_plots {
        if visited.contains(&plot.pos) {
            continue;
        }

        // find all connected plots of the same name
        let mut region_positions = Vec::new();
        let mut queue = VecDeque::new();
        queue.push_back(plot.pos);
        visited.insert(plot.pos);

        while let Some(current) = queue.pop_front() {
            region_positions.push(current);

            // Check neighbors
            for dir in &directions {
                let neighbor = (
                    (current.0 as isize + dir.0) as usize,
                    (current.1 as isize + dir.1) as usize,
                );

                if plot_map
                    .get(&plot.name)
                    .map_or(false, |points| points.contains(&neighbor))
                    && !visited.contains(&neighbor)
                {
                    queue.push_back(neighbor);
                    visited.insert(neighbor);
                }
            }
        }

        // Add the region to the list of regions
        regions.push(Region {
            name: plot.name,
            positions: HashSet::from_iter(region_positions.into_iter()),
        });
    }

    regions
}

fn solve(input: String) -> (usize, usize) {
    let map: Vec<GardenPlot> = get_map(input);
    let mut regions = find_regions(&map);

    let mut part1 = 0usize;
    let mut part2 = 0usize;

    regions.sort_by(|r1, r2| {
        r1.positions
            .iter()
            .min()
            .unwrap()
            .cmp(r2.positions.iter().min().unwrap())
    });

    for region in &regions {
        let nb_sides = region.sides();
        let perimeter = region.perimeter();
        println!(
            "/////// gtREGION {}: len {}, Sides: {} perim {} ",
            region.name,
            region.positions.len(),
            nb_sides,
            region.perimeter(),
        );
        part1 += region.positions.len() * perimeter;
        part2 += region.positions.len() * nb_sides;
    }

    (part1, part2)
}

pub fn main() {
    let input = input!();

    let (part1, part2) = solve(std::fs::read_to_string(input).unwrap());
    println!("{part1} {part2}");
}

#[cfg(test)]
mod tests {
    use crate::d12::{solve, Region};
    use std::collections::HashSet;

    #[test]
    fn example_input() {
        assert_eq!(
            solve(std::fs::read_to_string("input_test").unwrap()),
            (140, 80)
        );
    }

    #[test]
    fn test_sides() {
        let positions = HashSet::from_iter([(0, 0), (0, 1), (0, 2), (0, 3)].into_iter());
        let region = Region {
            name: 'A',
            positions,
        };
        assert_eq!(region.get_sides(), 4);
    }

    #[test]
    fn test_sides_cross_sign() {
        let region = Region {
            name: 'A',
            positions: HashSet::from_iter(
                [
                    (0, 2),
                    (1, 2),
                    (2, 0),
                    (2, 1),
                    (2, 2),
                    (2, 3),
                    (2, 4),
                    (3, 2),
                    (4, 2),
                ]
                .into_iter(),
            ),
        };
        //  ..A..
        //  ..A..
        //  AAAAA
        //  ..A..
        //  ..A..
        assert_eq!(region.get_sides(), 12);
    }

    #[test]
    fn test_sides_small_cross_sign() {
        let region = Region {
            name: 'A',
            positions: HashSet::from_iter(
                [(1, 2), (2, 0), (2, 1), (2, 2), (2, 3), (3, 2)].into_iter(),
            ),
        };
        //  .....
        //  ..A..
        //  .AAA.
        //  ..A..
        //  .....
        assert_eq!(region.get_sides(), 12);
    }

    #[test]
    fn test_sides_square() {
        let region = Region {
            name: 'A',
            positions: HashSet::from_iter([(1, 1), (1, 2), (2, 1), (2, 2)].into_iter()),
            // ....
            // .AA.
            // .AA.
            // ....
        };
        assert_eq!(region.get_sides(), 4);
    }

    #[test]
    fn test_sides_tee() {
        let region = Region {
            name: 'A',
            positions: HashSet::from_iter(
                [
                    (0, 0),
                    (0, 1),
                    (0, 2),
                    (0, 3),
                    (1, 1),
                    (1, 2),
                    (2, 1),
                    (2, 2),
                    (3, 1),
                    (3, 2),
                ]
                .into_iter(),
            ),
            // AAAA
            // .AA.
            // .AA.
            // .AA.
        };
        assert_eq!(region.get_sides(), 8);
    }

    #[test]
    fn test_sides_weird_form() {
        let region = Region {
            name: 'A',
            positions: HashSet::from_iter([
                (1, 1),
                (2, 1),
                (2, 2),
                (3, 2), // ....
                        // .A..
                        // .AA.
                        // ..A.
            ]),
        };
        //         assert_eq!(region.sides(), 8);
        assert_eq!(region.get_sides(), 8);
    }

    #[test]
    fn large_input() {
        assert_eq!(
            solve(std::fs::read_to_string("large").unwrap()),
            (1930, 1206)
        );
    }
}
