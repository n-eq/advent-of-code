use aoc::*;
use itertools::*;

type Directions = Vec<Vec<char>>;

fn generate_combinations(v: impl Iterator<Item = Directions>) -> Directions {
    v.fold(vec![Vec::new()], |acc, group| {
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

    fn paths(&self, dest: char) -> Directions {
        use NumericKeypad::*;
        let res = match *self {
            A => match dest {
                'A' => vec!["A"],
                '0' => vec!["<A"],
                '1' => vec!["<^<A", "^<<A"],
                '2' => vec!["^<A", "<^A"],
                '3' => vec!["^A"],
                '4' => vec!["^^<<A", "<^^<A", "^<<^A", "^<^<A", "<^<^A"],
                '5' => vec!["^^<A", "<^^A", "^<^A"],
                '6' => vec!["^^A"],
                '7' => vec![
                    "^<^<^A", "<^^<^A", "^^<<^A", "^<<^^A", "<^<^^A", "^^^<<A", "^<^^<A", "<^^^<A",
                    "^^<^<A",
                ],
                '8' => vec!["^^^<A", "^<^^A", "<^^^A", "^^<^A"],
                '9' => vec!["^^^A"],
                _ => unreachable!(),
            },
            Zero => match dest {
                'A' => vec![">A"],
                '0' => vec!["A"],
                '1' => vec!["^<A"],
                '2' => vec!["^A"],
                '3' => vec![">^A", "^>A"],
                '4' => vec!["^^<A", "^<^A"],
                '5' => vec!["^^A"],
                '6' => vec![">^^A", "^>^A", "^^>A"],
                '7' => vec!["^^^<A", "^<^^A", "^^<^A"],
                '8' => vec!["^^^A"],
                '9' => vec!["^^>^A", ">^^^A", "^>^^A", "^^^>A"],
                _ => unreachable!(),
            },
            One => match dest {
                'A' => vec![">>vA", ">v>A"],
                '0' => vec![">vA"],
                '1' => vec!["A"],
                '2' => vec![">A"],
                '3' => vec![">>A"],
                '4' => vec!["^A"],
                '5' => vec![">^A", "^>A"],
                '6' => vec![">^>A", "^>>A", ">>^A"],
                '7' => vec!["^^A"],
                '8' => vec![">^^A", "^>^A", "^^>A"],
                '9' => vec!["^>^>A", ">^^>A", "^^>>A", ">>^^A", "^>>^A", ">^>^A"],
                _ => unreachable!(),
            },
            Two => match dest {
                'A' => vec![">vA"],
                '0' => vec!["v>A"],
                '1' => vec!["vA"],
                '2' => vec!["A"],
                '3' => vec![">A"],
                '4' => vec!["<^A", "^<A"],
                '5' => vec!["^A"],
                '6' => vec!["^>A", ">^A"],
                '7' => vec!["^^<A", "^<^A", "<^^A"],
                '8' => vec!["^^A"],
                '9' => vec!["^^>A", "^>^A", ">^^A"],
                _ => unreachable!(),
            },
            Three => match dest {
                'A' => vec!["vA"],
                '0' => vec!["<v<A", "v<A"],
                '1' => vec!["<<A"],
                '2' => vec!["<A"],
                '3' => vec!["A"],
                '4' => vec!["^<<A", "<^<A", "<<^A"],
                '5' => vec!["^<A", "<^A"],
                '6' => vec!["^A"],
                '7' => vec!["^^<<A", "^<^<A", "<^^<A", "^<<^A", "<^<^A", "<<^^A"],
                '8' => vec!["^^<A", "<^^A", "^<^A"],
                '9' => vec!["^^A"],
                _ => unreachable!(),
            },
            Four => match dest {
                'A' => vec!["v>>vA", ">v>vA", ">>vvA", "v>v>A", ">vv>A"],
                '0' => vec![">vvA", "v>vA"],
                '1' => vec!["vA"],
                '2' => vec![">vA", "v>A"],
                '3' => vec![">>vA", "v>>A", ">v>A"],
                '4' => vec!["A"],
                '5' => vec![">A"],
                '6' => vec![">>A"],
                '7' => vec!["^A"],
                '8' => vec!["^>A", ">^A"],
                '9' => vec![">>^A", "^>>A", ">^>A"],
                _ => unreachable!(),
            },
            Five => match dest {
                'A' => vec![">vvA", "v>vA", "vv>A"],
                '0' => vec!["vvA"],
                '1' => vec!["<vA", "v<A"],
                '2' => vec!["vA"],
                '3' => vec![">vA", "v>A"],
                '4' => vec!["<A"],
                '5' => vec!["A"],
                '6' => vec![">A"],
                '7' => vec!["^<A", "<^A"],
                '8' => vec!["^A"],
                '9' => vec![">^A", "^>A"],
                _ => unreachable!(),
            },
            Six => match dest {
                'A' => vec!["vvA"],
                '0' => vec!["vv<A", "v<vA", "<vvA"],
                '1' => vec!["<<vA", "v<<A", "<v<A"],
                '2' => vec!["<vA", "v<A"],
                '3' => vec!["vA"],
                '4' => vec!["<<A"],
                '5' => vec!["<A"],
                '6' => vec!["A"],
                '7' => vec!["^<<A", "<^<A", "<<^A"],
                '8' => vec!["<^A", "^<A"],
                '9' => vec!["^A"],
                _ => unreachable!(),
            },
            Seven => match dest {
                'A' => vec![
                    "vv>>vA", "v>v>vA", ">vv>vA", ">>vvvA", "v>>vvA", ">v>vvA", "vv>v>A", "v>vv>A",
                    ">vvv>A",
                ],
                '0' => vec!["vv>vA", ">vvvA", "v>vvA"],
                '1' => vec!["vvA"],
                '2' => vec![">vvA", "v>vA", "vv>A"],
                '3' => vec!["vv>>A", "v>v>A", ">vv>A", "v>>vA", ">v>vA", ">>vvA"],
                '4' => vec!["vA"],
                '5' => vec!["v>A", ">vA"],
                '6' => vec![">>vA", ">v>A", "v>>A"],
                '7' => vec!["A"],
                '8' => vec![">A"],
                '9' => vec![">>A"],
                _ => unreachable!(),
            },
            Eight => match dest {
                'A' => vec!["vvv>A", ">vvvA", "v>vvA", "vv>vA"],
                '0' => vec!["vvvA"],
                '1' => vec!["v<vA", "<vvA", "vv<A"],
                '2' => vec!["vvA"],
                '3' => vec!["vv>A", ">vA", "v>vA"],
                '4' => vec!["v<A", "<vA"],
                '5' => vec!["vA"],
                '6' => vec!["v>A", ">vA"],
                '7' => vec!["<A"],
                '8' => vec!["A"],
                '9' => vec![">A"],
                _ => unreachable!(),
            },
            Nine => match dest {
                'A' => vec!["vvvA"],
                '0' => vec!["<vvvA", "v<vvA", "vv<vA", "vvv<A"],
                '1' => vec!["<<vvA", "<v<vA", "v<<vA", "vv<<A", "<vv<A", "v<v<A"],
                '2' => vec!["<vvA", "v<vA", "vv<A"],
                '3' => vec!["vvA"],
                '4' => vec!["<<vA", "<v<A", "v<<A"],
                '5' => vec!["<vA", "v<A"],
                '6' => vec!["vA"],
                '7' => vec!["<<A"],
                '8' => vec!["<A"],
                '9' => vec!["A"],
                _ => unreachable!(),
            },
            X => unreachable!(),
        };
        res.iter()
            .map(|s| s.chars().collect::<Vec<char>>())
            .collect()
    }

    fn directions(&self, buttons: Vec<char>) -> Directions {
        let mut state = *self;
        let res = buttons.iter().map(|&b| {
            let r = state.paths(b);
            state = NumericKeypad::from_char(b);
            r
        });

        generate_combinations(res)
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

    fn paths(&self, dest: char) -> Directions {
        use DirectionalKeypad::*;
        let res = match *self {
            A => match dest {
                'A' => vec!["A"],
                '^' => vec!["<A"],
                '>' => vec!["vA"],
                'v' => vec!["v<A", "<vA"],
                '<' => vec!["v<<A", "<v<A"],
                _ => unreachable!(),
            },
            Up => match dest {
                '^' => vec!["A"],
                'A' => vec![">A"],
                'v' => vec!["vA"],
                '>' => vec![">vA", "v>A"],
                '<' => vec!["v<A", "<vA"],
                _ => unreachable!(),
            },
            Left => match dest {
                '<' => vec!["A"],
                'v' => vec![">A"],
                '>' => vec![">>A"],
                '^' => vec![">^A"],
                'A' => vec![">>^A", ">^>A"],
                _ => unreachable!(),
            },
            Down => match dest {
                'v' => vec!["A"],
                '<' => vec!["<A"],
                '>' => vec![">A"],
                '^' => vec!["^A"],
                'A' => vec!["^>A", ">^A"],
                _ => unreachable!(),
            },
            Right => match dest {
                '>' => vec!["A"],
                'v' => vec!["<A"],
                '<' => vec!["<<A"],
                'A' => vec!["^A"],
                '^' => vec!["^<A", "<^A"],
                _ => unreachable!(),
            },
            _ => unreachable!(),
        };
        res.iter()
            .map(|s| s.chars().collect::<Vec<char>>())
            .collect()
    }

    fn directions(&self, buttons: Vec<char>) -> Directions {

        let mut state = *self;
        let res = buttons.iter().map(|&b| {
            let r = state.paths(b);
            state = DirectionalKeypad::from_char(b);
            r
        });

        generate_combinations(res)
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
}

fn get_code_complexity<const NB_ROBOTS: usize>(code: Vec<char>) -> usize {
    let mut res = NumericKeypad::A.directions(code);
    res.sort_unstable_by_key(|a| a.len());
    res = res
        .iter()
        .filter(|r| r.len() == res[0].len())
        .cloned()
        .collect();

    for _ in 0..NB_ROBOTS - 1 {
        res = res
            .into_iter()
            .flat_map(|r| DirectionalKeypad::A.directions(r))
            .collect();

        res.sort_unstable_by_key(|a| a.len());
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
        let chars = code.chars();
        let num = chars
            .clone()
            .filter(|c| c.is_ascii_digit())
            .collect::<Vec<_>>()
            .iter()
            .fold(0, |acc, x| x.to_digit(10).unwrap() + 10 * acc) as usize;

        part1 += num * get_code_complexity(chars.collect());
    }
    println!("{part1:?}");
}
