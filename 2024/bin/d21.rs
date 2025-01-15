use aoc::*;
use itertools::*;
use pathfinding::directed::astar::*;

type Directions = Vec<Vec<char>>;

fn generate_combinations(v: Vec<Directions>) -> Directions {
    v.into_iter().fold(vec![Vec::new()], |acc, group| {
        let mut new_result = Vec::with_capacity(acc.len() * group.len());
        for (mut prefix, suffix) in iproduct!(acc, group) {
            prefix.extend(suffix);
            new_result.push(prefix);
        }
        new_result
    })
}

#[derive(Hash, Copy, Debug, Default, Clone, PartialEq, Eq)]
#[repr(u8)]
enum NumericKeypad {
    /*
    +---+---+---+
    | 7 | 8 | 9 |
    +---+---+---+
    | 4 | 5 | 6 |
    +---+---+---+
    | 1 | 2 | 3 |
    +---+---+---+
        | 0 | A |
        +---+---+
    */
    #[default]
    A = b'A',
    Zero = b'0',
    One = b'1',
    Two = b'2',
    Three = b'3',
    Four = b'4',
    Five = b'5',
    Six = b'6',
    Seven = b'7',
    Eight = b'8',
    Nine = b'9',
    // Virtual button (leftmost in the last row)
    #[allow(dead_code)]
    X = b'X',
}

#[derive(Hash, Copy, Debug, Default, Clone, PartialEq, Eq)]
#[repr(u8)]
enum DirectionalKeypad {
    /*
        +---+---+
        | ^ | A |
    +---+---+---+
    | < | v | > |
    +---+---+---+
    */
    #[default]
    A = b'A',
    Up = b'^',
    Down = b'v',
    Right = b'>',
    Left = b'<',
    // Virtual button (leftmost in the top row)
    #[allow(dead_code)]
    X = b'X',
}

impl From<NumericKeypad> for char {
    fn from(k: NumericKeypad) -> char {
        k.as_char()
    }
}

impl From<DirectionalKeypad> for char {
    fn from(k: DirectionalKeypad) -> char {
        k.as_char()
    }
}

impl From<char> for DirectionalKeypad {
    fn from(c: char) -> DirectionalKeypad {
        DirectionalKeypad::from_char(c)
    }
}

impl From<char> for NumericKeypad {
    fn from(c: char) -> NumericKeypad {
        NumericKeypad::from_char(c)
    }
}

impl From<(usize, usize)> for NumericKeypad {
    fn from(p: (usize, usize)) -> NumericKeypad {
        let grid: Vec<Vec<_>> = vec![
            vec!['7', '8', '9'],
            vec!['4', '5', '6'],
            vec!['1', '2', '3'],
            vec!['X', '0', 'A'],
        ];
        NumericKeypad::from(grid[p.0][p.1])
    }
}

impl From<(usize, usize)> for DirectionalKeypad {
    fn from(p: (usize, usize)) -> DirectionalKeypad {
        let grid: Vec<Vec<_>> = vec![vec!['X', '^', 'A'], vec!['<', 'v', '>']];
        DirectionalKeypad::from(grid[p.0][p.1])
    }
}

impl NumericKeypad {
    fn as_char(&self) -> char {
        match self {
            NumericKeypad::A => 'A',
            NumericKeypad::Zero => '0',
            NumericKeypad::One => '1',
            NumericKeypad::Two => '2',
            NumericKeypad::Three => '3',
            NumericKeypad::Four => '4',
            NumericKeypad::Five => '5',
            NumericKeypad::Six => '6',
            NumericKeypad::Seven => '7',
            NumericKeypad::Eight => '8',
            NumericKeypad::Nine => '9',
            _ => 'X',
        }
    }

    fn dirs(&self, dest: char) -> Vec<Vec<DirectionalKeypad>> {
        let mut res = vec![];
        let paths = self.shortest_paths(NumericKeypad::from_char(dest));
        use NumericKeypad as Nk;
        for path in paths {
            let mut tmp = vec![];
            for i in 0..path.len() - 1 {
                let s = path[i];
                let d = path[i + 1];

                match (s, d) {
                    (Nk::Zero, Nk::Two)
                    | (Nk::A, Nk::Three)
                    | (Nk::One, Nk::Four)
                    | (Nk::Two, Nk::Five)
                    | (Nk::Three, Nk::Six)
                    | (Nk::Four, Nk::Seven)
                    | (Nk::Five, Nk::Eight)
                    | (Nk::Six, Nk::Nine) => tmp.push(DirectionalKeypad::Up),
                    (Nk::Zero, Nk::A)
                    | (Nk::One, Nk::Two)
                    | (Nk::Two, Nk::Three)
                    | (Nk::Four, Nk::Five)
                    | (Nk::Five, Nk::Six)
                    | (Nk::Seven, Nk::Eight)
                    | (Nk::Eight, Nk::Nine) => tmp.push(DirectionalKeypad::Right),
                    (Nk::A, Nk::Zero)
                    | (Nk::Three, Nk::Two)
                    | (Nk::Two, Nk::One)
                    | (Nk::Six, Nk::Five)
                    | (Nk::Five, Nk::Four)
                    | (Nk::Nine, Nk::Eight)
                    | (Nk::Eight, Nk::Seven) => tmp.push(DirectionalKeypad::Left),
                    (Nk::Seven, Nk::Four)
                    | (Nk::Eight, Nk::Five)
                    | (Nk::Nine, Nk::Six)
                    | (Nk::Four, Nk::One)
                    | (Nk::Five, Nk::Two)
                    | (Nk::Six, Nk::Three)
                    | (Nk::Two, Nk::Zero)
                    | (Nk::Three, Nk::A) => tmp.push(DirectionalKeypad::Down),
                    _ => unreachable!(),
                }
            }
            tmp.push('A'.into());
            res.push(tmp);
        }

        res
    }

    fn dirs_alt(&self, dest: char) -> Vec<Vec<DirectionalKeypad>> {
        use DirectionalKeypad as Dk;
        use NumericKeypad::*;
        let dest = NumericKeypad::from(dest);
        let empty: Vec<Vec<DirectionalKeypad>> = vec![vec![]];
        match *self {
            A => match dest {
                A => empty.clone(),
                Zero => vec![vec![Dk::Left]],
                One => vec![
                    vec![Dk::Left, Dk::Up, Dk::Left],
                    vec![Dk::Up, Dk::Left, Dk::Left],
                ],
                Two => vec![vec![Dk::Up, Dk::Left], vec![Dk::Left, Dk::Up]],
                Three => vec![vec![Dk::Up]],
                Four => vec![
                    vec![Dk::Up, Dk::Up, Dk::Left, Dk::Left],
                    vec![Dk::Up, Dk::Left, Dk::Left, Dk::Up],
                    vec![Dk::Up, Dk::Left, Dk::Up, Dk::Left],
                    vec![Dk::Left, Dk::Up, Dk::Left, Dk::Up],
                    vec![Dk::Left, Dk::Up, Dk::Up, Dk::Left],
                ],
                Five => todo!(),
                Six => todo!(),
                Seven => todo!(),
                Eight => todo!(),
                Nine => todo!(),
                X => todo!(),
            },
            Zero => todo!(),
            One => todo!(),
            Two => todo!(),
            Three => todo!(),
            Four => todo!(),
            Five => todo!(),
            Six => todo!(),
            Seven => todo!(),
            Eight => todo!(),
            Nine => todo!(),
            X => todo!(),
        }
    }

    fn directions(&self, buttons: Vec<char>) -> Directions {
        let mut state = *self;
        let mut res: Vec<Vec<Vec<DirectionalKeypad>>> = vec![];
        for b in buttons {
            let dirs = state.dirs(b);
            res.push(dirs);

            state = NumericKeypad::from_char(b);
        }

        generate_combinations(
            res.iter()
                .map(|r| {
                    r.iter()
                        .map(|v| v.iter().copied().map(Into::into).collect::<Vec<char>>())
                        .collect::<Directions>()
                })
                .collect::<Vec<Directions>>(),
        )
    }

    fn from_char(c: char) -> Self {
        match c {
            '0' => Self::Zero,
            '1' => Self::One,
            '2' => Self::Two,
            '3' => Self::Three,
            '4' => Self::Four,
            '5' => Self::Five,
            '6' => Self::Six,
            '7' => Self::Seven,
            '8' => Self::Eight,
            '9' => Self::Nine,
            'A' => Self::A,
            'X' => Self::X,
            _ => unreachable!("Unknown button in the numeric pad: {c:?}"),
        }
    }

    fn pos_in_grid(&self) -> (usize, usize) {
        let grid: Vec<Vec<_>> = [
            vec!['7', '8', '9'],
            vec!['4', '5', '6'],
            vec!['1', '2', '3'],
            vec!['X', '0', 'A'],
        ]
        .iter()
        .map(|v| {
            v.iter()
                .map(|b| NumericKeypad::from(*b))
                .collect::<Vec<_>>()
        })
        .collect::<Vec<_>>();

        let mut pos = (0, 0);
        pos.0 = grid.iter().position(|v| v.contains(self)).unwrap();
        pos.1 = grid[pos.0].iter().position(|c| c == self).unwrap();
        pos
    }

    fn successors(&self) -> Vec<(NumericKeypad, usize)> {
        use NumericKeypad::*;
        match self {
            A => vec![Zero, Three],
            Zero => vec![Two, A],
            One => vec![Two, Four],
            Two => vec![Zero, One, Three, Five],
            Three => vec![Two, A, Six],
            Four => vec![One, Five, Seven],
            Five => vec![Two, Four, Six, Eight],
            Six => vec![Three, Five, Nine],
            Seven => vec![Four, Eight],
            Eight => vec![Five, Seven, Nine],
            Nine => vec![Six, Eight],
            X => unreachable!(),
        }
        .into_iter()
        .map(|s| (s, 1))
        .collect()
    }

    fn manhattan(&self, other: NumericKeypad) -> usize {
        let start = self.pos_in_grid();
        let end = other.pos_in_grid();

        start.0.abs_diff(end.0) + start.1.abs_diff(end.1)
    }

    fn shortest_paths(&self, other: NumericKeypad) -> Vec<Vec<NumericKeypad>> {
        astar_bag_collect(
            self,
            |&n| n.successors(),
            |&n| n.manhattan(other),
            |&n| n == other,
        )
        .unwrap()
        .0
    }
}

impl DirectionalKeypad {
    fn as_char(&self) -> char {
        match self {
            DirectionalKeypad::A => 'A',
            DirectionalKeypad::Left => '<',
            DirectionalKeypad::Right => '>',
            DirectionalKeypad::Up => '^',
            DirectionalKeypad::Down => 'v',
            _ => 'X',
        }
    }

    fn dirs(&self, dest: char) -> Vec<Vec<DirectionalKeypad>> {
        let mut res = vec![];
        let paths = self.shortest_paths(DirectionalKeypad::from_char(dest));
        use DirectionalKeypad as Dk;

        // TODO: use windows() and map to collect directly into a vector
        for path in paths {
            let mut tmp = vec![];
            for i in 0..path.len() - 1 {
                let s = path[i];
                let d = path[i + 1];

                match (s, d) {
                    (Dk::Down, Dk::Up) | (Dk::Right, Dk::A) => tmp.push(DirectionalKeypad::Up),
                    (Dk::Left, Dk::Down) | (Dk::Up, Dk::A) | (Dk::Down, Dk::Right) => {
                        tmp.push(DirectionalKeypad::Right)
                    }
                    (Dk::A, Dk::Up) | (Dk::Right, Dk::Down) | (Dk::Down, Dk::Left) => {
                        tmp.push(DirectionalKeypad::Left)
                    }
                    (Dk::Up, Dk::Down) | (Dk::A, Dk::Right) => tmp.push(DirectionalKeypad::Down),
                    _ => unreachable!(),
                }
            }
            tmp.push('A'.into());
            res.push(tmp);
        }

        res
    }
    fn dirs_alt(&self, dest: char) -> Vec<Vec<DirectionalKeypad>> {
        use DirectionalKeypad::*;
        let dest: DirectionalKeypad = DirectionalKeypad::from_char(dest);
        match *self {
            A => match dest {
                A => vec![vec![]],
                Up => vec![vec![Left]],
                Right => vec![vec![Down]],
                Down => vec![vec![Down, Left], vec![Left, Down]],
                Left => vec![vec![Down, Left, Left], vec![Left, Down, Left]],
                X => unreachable!(),
            },
            Up => match dest {
                Up => vec![vec![]],
                A => vec![vec![Right]],
                Down => vec![vec![Down]],
                Right => vec![vec![Right, Down], vec![Down, Right]],
                Left => vec![vec![Down, Left], vec![Down, Left]],
                X => unreachable!(),
            },
            Left => match dest {
                Left => vec![vec![]],
                Down => vec![vec![Right]],
                Right => vec![vec![Right, Right]],
                Up => vec![vec![Right, Up]],
                A => vec![vec![Right, Right, Up], vec![Right, Up, Right]],
                X => unreachable!(),
            },
            Down => match dest {
                Down => vec![vec![]],
                Left => vec![vec![Left]],
                Right => vec![vec![Right]],
                Up => vec![vec![Up]],
                A => vec![vec![Up, Right], vec![Right, Up]],
                X => unreachable!(),
            },
            Right => match dest {
                Right => vec![vec![]],
                Down => vec![vec![Left]],
                Left => vec![vec![Left, Left]],
                A => vec![vec![Up]],
                Up => vec![vec![Up, Left], vec![Left, Up]],
                X => unreachable!(),
            },
            X => unreachable!(),
        }
    }

    fn directions(&self, buttons: Vec<char>) -> Directions {
        let mut state = *self;
        let mut res: Vec<Vec<Vec<DirectionalKeypad>>> = vec![];
        for b in buttons {
            // TODO: use dirs_alt (quicker but buggy...)
            let dirs = state.dirs(b);
            res.push(dirs);

            state = DirectionalKeypad::from_char(b);
        }

        generate_combinations(
            res.iter()
                .map(|r| {
                    r.iter()
                        .map(|v| v.iter().copied().map(Into::into).collect::<Vec<char>>())
                        .collect::<Directions>()
                })
                .collect::<Vec<Directions>>(),
        )
    }

    fn from_char(c: char) -> Self {
        match c {
            'A' => Self::A,
            '>' => Self::Right,
            '<' => Self::Left,
            '^' => Self::Up,
            'v' => Self::Down,
            'X' => Self::X,
            _ => unreachable!("Unknown button in the directional pad: {c:?}"),
        }
    }

    fn pos_in_grid(&self) -> (usize, usize) {
        let grid: Vec<Vec<_>> = [vec!['X', '^', 'A'], vec!['<', 'v', '>']]
            .iter()
            .map(|v| {
                v.iter()
                    .map(|b| DirectionalKeypad::from(*b))
                    .collect::<Vec<_>>()
            })
            .collect::<Vec<_>>();

        let mut pos = (0, 0);
        pos.0 = grid.iter().position(|v| v.contains(self)).unwrap();
        pos.1 = grid[pos.0].iter().position(|c| c == self).unwrap();
        pos
    }

    fn successors(&self) -> Vec<(DirectionalKeypad, usize)> {
        use DirectionalKeypad::*;
        match self {
            A => vec![Up, Right],
            Up => vec![A, Down],
            Down => vec![Up, Right, Left],
            Right => vec![A, Down],
            Left => vec![Down],
            X => unreachable!(),
        }
        .iter()
        .copied()
        .map(|s| (s, 1))
        .collect()
    }

    fn manhattan(&self, other: DirectionalKeypad) -> usize {
        let start = self.pos_in_grid();
        let end = other.pos_in_grid();

        start.0.abs_diff(end.0) + start.1.abs_diff(end.1)
    }

    fn shortest_paths(&self, other: DirectionalKeypad) -> Vec<Vec<DirectionalKeypad>> {
        astar_bag_collect(
            self,
            |&n| n.successors(),
            |&n| n.manhattan(other),
            |&n| n == other,
        )
        .unwrap()
        .0
    }
}

fn get_code_complexity(code: Vec<char>) -> usize {
    let mut res = NumericKeypad::A.directions(code);
    res.sort_by_key(|a| a.len());
    res = res
        .iter()
        .filter(|r| r.len() == res[0].len())
        .cloned()
        .collect();

    for _ in 0..2 {
        res = res
            .into_iter()
            .flat_map(|r| DirectionalKeypad::A.directions(r))
            .collect();

        res.sort_by_key(|a| a.len());
        res = res
            .iter()
            .filter(|r| r.len() == res[0].len())
            .cloned()
            .collect();
    }

    res[0].len()
}

pub fn main() {
    let mut part1: usize = 0;
    for code in std::fs::read_to_string(input!()).unwrap().lines() {
        let num = code
            .chars()
            .filter(|c| c.is_ascii_digit())
            .collect::<Vec<_>>()
            .iter()
            .fold(0, |acc, x| x.to_digit(10).unwrap() + 10 * acc) as usize;

        part1 += num * get_code_complexity(code.chars().collect());
    }
    println!("{part1:?}");
}
