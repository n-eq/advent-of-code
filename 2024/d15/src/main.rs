type Point = (isize, isize);

#[derive(Debug, PartialEq, Copy, Clone)]
enum Position {
    WarehouseBox,
    Wall,
    Empty,
    Robot,
    // Part2
    WarehouseBoxLeft,
    WarehouseBoxRight,
    DoubleWall,
    DoubleEmpty,
}

impl Position {
    // when moving complex (non-simple) Position types,
    // we must move 2 elements in the map (the rhs and lhs)
    fn is_simple(&self) -> bool {
        !matches!(
            self,
            Position::WarehouseBoxRight | Position::WarehouseBoxLeft
        )
    }

    fn is_empty(&self) -> bool {
        matches!(self, Position::Empty | Position::DoubleEmpty)
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
    data: Vec<Vec<Position>>,
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
                    //                     "{: >2}",
                    match c {
                        Position::WarehouseBox => "O",
                        Position::Wall => "#",
                        Position::Robot => "@",
                        Position::DoubleEmpty => ".",
                        Position::DoubleWall => "#",
                        Position::WarehouseBoxLeft => "[",
                        Position::WarehouseBoxRight => "]",
                        _ => ".",
                    }
                );
            }
            println!();
        }
    }

    fn new(input: String) -> Self {
        let mut data: Vec<Vec<Position>> = vec![];
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
                            '#' => Position::Wall,
                            'O' => Position::WarehouseBox,
                            '.' => Position::Empty,
                            '@' => Position::Robot,
                            _ => panic!(),
                        })
                        .collect::<Vec<Position>>(),
                );
            }
        }

        // duplicated
        let mut robot: Point = (0, 0);
        for (i, line) in data.iter().enumerate() {
            for (j, c) in line.iter().enumerate() {
                if c == &Position::Robot {
                    robot = (i as isize, j as isize);
                    break;
                }
            }
        }

        Self { data, moves, robot }
    }

    fn can_move(&self, p: Point, m: &Move) -> bool {
        let next = match m {
            Move::Up => (p.0 - 1, p.1),
            Move::Down => (p.0 + 1, p.1),
            Move::Left => (p.0, p.1 - 1),
            Move::Right => (p.0, p.1 + 1),
        };
        if let Some(pos) = self.get(next) {
            let current_type = self.get(p).unwrap();
            let up_or_down = matches!(m, Move::Left | Move::Right);
            if pos.is_empty() {
                if up_or_down {
                    true
                } else {
                    if current_type.is_simple() {
                        true
                    } else {
                        match current_type {
                            Position::WarehouseBoxRight => self
                                .get((next.0, next.1 + 1))
                                .map_or(false, |p| p.is_empty()),
                            Position::WarehouseBoxLeft => self
                                .get((next.0, next.1 - 1))
                                .map_or(false, |p| p.is_empty()),
                            _ => todo!(),
                        }
                    }
                }
            } else if matches!(pos, Position::Wall | Position::DoubleWall) {
                false
            } else {
                let mut res = self.can_move(next, m);
                if up_or_down {
                    if !current_type.is_simple() {
                        match current_type {
                            Position::WarehouseBoxRight => res &= self.can_move((p.0, p.1 - 1), m),
                            Position::WarehouseBoxLeft => res &= self.can_move((p.0, p.1 + 1), m),
                            _ => todo!(),
                        }
                    }
                }
                res
            }
        } else {
            // shouldn't happen (because the map is surrounded by walls) but position out of bounds
            println!("SHOULDN4T HAPP");
            false
        }
    }

    fn extend(&mut self) {
        let prevdata = self.data.clone();
        for (row, _) in prevdata.iter().enumerate() {
            let mut newdata: Vec<Position> = vec![];
            for col in 0..prevdata[row].len() {
                match self.data[row][col] {
                    Position::WarehouseBox => {
                        newdata.push(Position::WarehouseBoxLeft);
                        newdata.push(Position::WarehouseBoxRight);
                    }
                    Position::Empty => {
                        newdata.push(Position::DoubleEmpty);
                        newdata.push(Position::DoubleEmpty);
                    }
                    Position::Wall => {
                        newdata.push(Position::DoubleWall);
                        newdata.push(Position::DoubleWall);
                    }
                    Position::Robot => {
                        newdata.push(Position::Robot);
                        newdata.push(Position::Empty);
                    }
                    _ => (),
                }
            }
            self.data[row] = newdata;
        }
        println!("MAP AFTER EXTENSION");

        for (i, line) in self.data.iter().enumerate() {
            for (j, c) in line.iter().enumerate() {
                if c == &Position::Robot {
                    self.robot = (i as isize, j as isize);
                    break;
                }
            }
        }

        self.draw_map();
    }

    fn get(&self, p: Point) -> Option<Position> {
        self.data
            .get(p.0 as usize)
            .and_then(|l| l.get(p.1 as usize))
            .map(|p| *p)
    }

    fn get_next_positions(&self, m: &Move) -> Vec<Point> {
        let mut next = self.robot;
        let mut next_positions: Vec<Point> = vec![];
        match m {
            Move::Down => {
                next.0 += 1;
                for (i, _) in self
                    .data
                    .iter()
                    .enumerate()
                    .filter(|(i, _)| *i > self.robot.0 as usize)
                {
                    next_positions.push((i as isize, self.robot.1));
                }
            }
            Move::Up => {
                next.0 -= 1;
                for (i, _) in self
                    .data
                    .iter()
                    .enumerate()
                    .filter(|(i, _)| *i < self.robot.0 as usize)
                {
                    next_positions.push((i as isize, self.robot.1));
                }
                next_positions.reverse();
            }

            Move::Left => {
                next.0 -= 1;
                for (_, l) in self
                    .data
                    .iter()
                    .enumerate()
                    .filter(|(i, _)| *i == self.robot.0 as usize)
                {
                    for (col, _) in l
                        .iter()
                        .enumerate()
                        .filter(|(c, _)| *c < self.robot.1 as usize)
                    {
                        next_positions.push((self.robot.0, col as isize));
                    }
                }
                next_positions.reverse();
            }
            Move::Right => {
                next.0 += 1;
                for (_, l) in self
                    .data
                    .iter()
                    .enumerate()
                    .filter(|(i, _)| *i == self.robot.0 as usize)
                {
                    for (col, _) in l
                        .iter()
                        .enumerate()
                        .filter(|(c, _)| *c > self.robot.1 as usize)
                    {
                        next_positions.push((self.robot.0, col as isize));
                    }
                }
            }
        }

        next_positions
    }

    fn solve(&mut self) -> usize {
        for (i, m) in self.moves.iter().enumerate() {
            let can_move = self.can_move(self.robot, m);
            if !can_move {
                continue;
            }

            println!("\nMove {i} {m:?}");
            self.draw_map();

            let up_or_down = matches!(m, Move::Up | Move::Down);
            if up_or_down {
                println!("todo");
            } else {
                if m == &Move::Left {
                    //                     self.data[self.robot.0].rotate_left(
                    let first_wall = self.data[self.robot.0 as usize][..self.robot.1 as usize]
                        .iter()
                        .enumerate()
                        .filter(|(_, c)| *c == &Position::Wall || *c == &Position::DoubleWall)
                        .map(|(col, _)| col)
                        .last()
                        .unwrap();

                    /*
                    self.data[self.robot.0 as usize]
                        .copy_within(first_wall + 1..=self.robot.1 as usize, first_wall);
                    self.data[self.robot.0 as usize][self.robot.1 as usize] = Position::Empty;
                    self.robot.1 -= 1;
                    */
                } else {
                    let first_wall = self.data[self.robot.0 as usize][self.robot.1 as usize..]
                        .iter()
                        .position(|c| c == &Position::Wall || c == &Position::DoubleWall)
                        .unwrap();

                    /*
                    self.data[self.robot.0 as usize]
                        .copy_within(self.robot.1 as usize..first_wall, self.robot.1 as usize + 1);

                    self.data[self.robot.0 as usize][self.robot.1 as usize] = Position::Empty;
                    self.robot.1 += 1;
                    */
                }
                // TODO put empty at prev position of robot
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
                        .filter(|(_, p)| {
                            matches!(*p, Position::WarehouseBox | Position::WarehouseBoxLeft)
                        })
                        .map(|(col, p)| {
                            if p == &Position::WarehouseBoxLeft {
                                std::cmp::min(col, l.len() - 1 - (col + 1))
                            } else {
                                col
                            }
                        })
                        .collect::<Vec<_>>(),
                )
            })
            //             .map(|(i, columns)| (std::cmp::min(i, self.data.len() - 1 - (i + 1)), columns))
            .map(|(i, columns)| columns.iter().map(|c| c + i * 100).sum::<usize>())
            .collect()
    }
}

fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = args.get(1).map_or("input_test", |v| v);

    let mut map = Problem::new(std::fs::read_to_string(input).unwrap());
    //     println!("part1: {:?}", map.clone().solve());

    map.extend();
    map.solve();
}

#[cfg(test)]
mod tests {
    use crate::*;

    #[test]
    fn test_example_input() {
        let input = "########
#..O.O.#
##@.O..#
#...O..#
#.#.O..#
#...O..#
#......#
########

<^^>>>vv<v>>v<<"
            .to_string();
        let mut map = Problem::new(input.clone());
        let s = map.solve();
        assert_eq!(s, 2028);
    }

    #[test]
    fn test_example_p2() {
        let input = "#######
#...#.#
#.....#
#..OO@#
#..O..#
#.....#
#######

<vv<<^^<<^^"
            .to_string();
        let mut map = Problem::new(input);
        map.extend();
        let s = map.solve();
        assert_eq!(s, 10000);
    }

    #[test]
    fn test_large_example() {
        //         let mut map = Problem::new(std::fs::read_to_string("input_test").unwrap());
        //         assert_eq!(map.solve(), 10092);
        let mut map = Problem::new(std::fs::read_to_string("input_test").unwrap());
        map.extend();
        assert_eq!(map.solve(), 9021);
    }

    #[test]
    fn test_ext() {
        let mut map = Problem::new(std::fs::read_to_string("input_test").unwrap());
        map.draw_map();
        map.extend();

        let map_end = Problem::new(
            "####################
##[].......[].[][]##
##[]...........[].##
##[]........[][][]##
##[]......[]....[]##
##..##......[]....##
##..[]............##
##..@......[].[][]##
##......[][]..[]..##
####################"
                .to_string(),
        );
        map.solve();
        assert_eq!(map, map_end);
    }
}
