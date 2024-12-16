mod test;

type Point = (isize, isize);

#[derive(Debug, PartialEq, Copy, Clone)]
enum Tile {
    WarehouseBox,
    Wall,
    Empty,
    Robot,
    // Part2
    WarehouseBoxLeft,
    WarehouseBoxRight,
}
use Tile::*;

impl Tile {
    // when moving complex (non-simple) Tile types,
    // we must move 2 elements in the map (the rhs and lhs)
    fn is_simple(&self) -> bool {
        !matches!(self, WarehouseBoxRight | WarehouseBoxLeft)
    }

    fn is_empty(&self) -> bool {
        self == &Empty
    }

    fn is_wall(&self) -> bool {
        self == &Wall
    }
}

#[derive(Debug, Copy, Clone, PartialEq)]
enum Move {
    Left,
    Up,
    Down,
    Right,
}

#[derive(Debug, Default, Clone, PartialEq)]
struct Problem {
    data: Vec<Vec<Tile>>,
    moves: Vec<Move>,
    robot: Point,
}

impl Problem {
    #[allow(unused)]
    fn draw_map(&self) {
        for l in &self.data {
            for c in l {
                print!(
                    "{}",
                    match c {
                        WarehouseBox => "O",
                        Wall => "#",
                        Robot => "@",
                        WarehouseBoxLeft => "[",
                        WarehouseBoxRight => "]",
                        _ => ".",
                    }
                );
            }
            println!();
        }
    }

    fn new(input: String) -> Self {
        let mut data: Vec<Vec<Tile>> = vec![];
        let mut moves: Vec<Move> = vec![];
        for line in input.lines() {
            if line == "" {
                continue;
            }

            if ['<', '>', 'v', '^'].iter().any(|c| line.contains(*c)) {
                moves.extend(
                    line.chars()
                        .filter(|c| c != &'\n')
                        .map(|c| match c {
                            '<' => Move::Left,
                            '>' => Move::Right,
                            '^' => Move::Up,
                            'v' => Move::Down,
                            _ => panic!(),
                        })
                        .collect::<Vec<Move>>(),
                );
            } else {
                data.push(
                    line.chars()
                        .map(|c| match c {
                            '#' => Wall,
                            'O' => WarehouseBox,
                            '.' => Empty,
                            '@' => Robot,
                            '[' => WarehouseBoxLeft,
                            ']' => WarehouseBoxRight,
                            _ => {
                                println!("ENCOUNTERED {c}");
                                panic!();
                            }
                        })
                        .collect::<Vec<Tile>>(),
                );
            }
        }

        // duplicated
        let mut robot: Point = (0, 0);
        for (i, line) in data.iter().enumerate() {
            for (j, c) in line.iter().enumerate() {
                if c == &Robot {
                    robot = (i as isize, j as isize);
                    break;
                }
            }
        }

        Self { data, moves, robot }
    }

    fn try_move(&mut self, p: Point, m: &Move, apply: bool) -> bool {
        let next = match m {
            Move::Up => (p.0 - 1, p.1),
            Move::Down => (p.0 + 1, p.1),
            Move::Left => (p.0, p.1 - 1),
            Move::Right => (p.0, p.1 + 1),
        };
        self.get(next).map_or_else(
            || false,
            |next_tile| {
                let current_tile = self.get(p).unwrap();
                let up_or_down = matches!(m, Move::Up | Move::Down);

                if !apply {
                    println!(
                        "checking if {p:?} ({current_tile:?}) can move {m:?} to {next_tile:?}"
                    );
                }

                // Case closed
                match next_tile {
                    Wall => false,
                    Empty => {
                        if apply {
                            self.data[next.0 as usize][next.1 as usize] = current_tile;
                            self.data[p.0 as usize][p.1 as usize] = Empty;
                            println!("Applied to empty {:?} {:?}", p, next);
                        }
                        true
                    }
                    WarehouseBoxLeft => {
                        if up_or_down {
                            if self.try_move(next, m, apply)
                                && self.try_move((next.0, next.1 + 1), m, apply)
                            {
                                if apply {
                                    self.data[next.0 as usize][next.1 as usize] = current_tile;
                                    self.data[p.0 as usize][p.1 as usize] = Empty;
                                }
                                true
                            } else {
                                false
                            }
                        } else {
                            if self.try_move(next, m, apply) {
                                if apply {
                                    self.data[next.0 as usize][next.1 as usize] = current_tile;
                                    self.data[p.0 as usize][p.1 as usize] = Empty;
                                }
                                true
                            } else {
                                false
                            }
                        }
                    }
                    WarehouseBoxRight => {
                        if up_or_down {
                            if self.try_move(next, m, apply)
                                && self.try_move((next.0, next.1 - 1), m, apply)
                            {
                                if apply {
                                    self.data[next.0 as usize][next.1 as usize] = current_tile;
                                    self.data[p.0 as usize][p.1 as usize] = Empty;
                                }
                                true
                            } else {
                                false
                            }
                        } else {
                            if self.try_move(next, m, apply) {
                                if apply {
                                    self.data[next.0 as usize][next.1 as usize] = current_tile;
                                    self.data[p.0 as usize][p.1 as usize] = Empty;
                                }
                                true
                            } else {
                                false
                            }
                        }
                    }
                    WarehouseBox => {
                        if self.try_move(next, m, apply) {
                            if apply {
                                self.data[next.0 as usize][next.1 as usize] = current_tile;
                                self.data[p.0 as usize][p.1 as usize] = Empty;
                            }
                            true
                        } else {
                            false
                        }
                    }
                    _ => {
                        println!("encountered weird tile {next_tile:?}");
                        panic!();
                    }
                }
            },
        )
    }

    fn extend(&mut self) {
        let prevdata = self.data.clone();
        for (row, _) in prevdata.iter().enumerate() {
            let mut newdata: Vec<Tile> = vec![];
            for col in 0..prevdata[row].len() {
                match self.data[row][col] {
                    WarehouseBox => {
                        newdata.push(WarehouseBoxLeft);
                        newdata.push(WarehouseBoxRight);
                    }
                    Empty => {
                        newdata.push(Empty);
                        newdata.push(Empty);
                    }
                    Wall => {
                        newdata.push(Wall);
                        newdata.push(Wall);
                    }
                    Robot => {
                        newdata.push(Robot);
                        newdata.push(Empty);
                    }
                    _ => (),
                }
            }
            self.data[row] = newdata;
        }
        println!("MAP AFTER EXTENSION");

        for (i, line) in self.data.iter().enumerate() {
            for (j, c) in line.iter().enumerate() {
                if c == &Robot {
                    self.robot = (i as isize, j as isize);
                    break;
                }
            }
        }

        self.draw_map();
    }

    fn get(&self, p: Point) -> Option<Tile> {
        self.data
            .get(p.0 as usize)
            .and_then(|l| l.get(p.1 as usize))
            .map(|p| *p)
    }

    fn solve(&mut self) -> usize {
        for (i, m) in self.moves.clone().iter().enumerate() {
            println!("\nMove {i} {m:?}");

            let can_move = self.try_move(self.robot, m, false);
            if can_move {
                self.try_move(self.robot, m, true);
                self.draw_map();
                self.robot = match m {
                    Move::Up => (self.robot.0 - 1, self.robot.1),
                    Move::Down => (self.robot.0 + 1, self.robot.1),
                    Move::Left => (self.robot.0, self.robot.1 - 1),
                    Move::Right => (self.robot.0, self.robot.1 + 1),
                };
                //                 println!("new robot: {:?}", self.robot);
            }
        }
        self.gps_coordinates().iter().sum::<usize>()
    }

    fn gps_coordinates(&self) -> Vec<usize> {
        self.data
            .iter()
            .enumerate()
            .map(|(i, l)| {
                (
                    i,
                    l.iter()
                        .enumerate()
                        .filter(|(_, p)| matches!(*p, WarehouseBox | WarehouseBoxLeft))
                        .map(|(col, _)| col)
                        .collect::<Vec<_>>(),
                )
            })
            .map(|(i, columns)| columns.iter().map(|c| c + i * 100).sum::<usize>())
            .collect()
    }
}

fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = args.get(1).map_or("input_test", |v| v);

    let mut map = Problem::new(std::fs::read_to_string(input).unwrap());
    let part1 = map.clone().solve();

    map.extend();
    let part2 = map.solve();
    println!("{part1}, {part2}");
}
