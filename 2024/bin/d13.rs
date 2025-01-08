use aoc::*;
use num::Integer;

#[derive(Default, Debug, Copy, Clone)]
struct ClawMachine {
    button_a: (usize, usize),
    button_b: (usize, usize),
    prize: (usize, usize),
}

impl ClawMachine {
    // At first I went for Diophantine equations but there were too many difficulties
    // minimizing the cost using that method.
    // Instead, you can just notice that find the number of presses for each button
    // means find the intersection (when it exists, otherwise the case is unsolvable)
    // of two 2D lines (equations):
    //
    // for the example:
    // Button A: X+94, Y+34
    // Button B: X+22, Y+67
    // Prize: X=8400, Y=5400
    //
    // you need to solve
    //  / a        b      c
    //    v        v      v
    //  ( 94x +    34y = 8400
    //  ( 22x +    67y = 5400
    //    ^        ^      ^
    //    a'       b'     c'
    //  \
    //
    // 2 equations with 2 unknown variables the result is straighforward
    fn solve(self) -> usize {
        let a = self.button_a.0 as isize;
        let b = self.button_a.1 as isize;

        let aprime = self.button_b.0 as isize;
        let bprime = self.button_b.1 as isize;

        let c = self.prize.0 as isize;
        let cprime = self.prize.1 as isize;

        // just grab a pen and paper
        let (y, r) = (aprime * c - a * cprime).div_rem(&(aprime * b - a * bprime));
        let (x, rp) = (c - b * y).div_rem(&a);

        if rp != 0 || r != 0 {
            0
        } else {
            (x as usize) * 3 + y as usize
        }
    }
}

fn get_machines(input: String) -> Vec<ClawMachine> {
    let mut machines: Vec<ClawMachine> = vec![];

    let mut ba: (usize, usize) = (0, 0);
    let mut bb: (usize, usize) = (0, 0);
    let mut prize = (0usize, 0usize);

    for l in input.lines() {
        if l.is_empty() {
            machines.push(ClawMachine {
                button_a: ba,
                button_b: bb,
                prize,
            });
            continue;
        }

        if l.contains("A") {
            let splits = l.split([':', '+']).collect::<Vec<_>>();
            ba.0 = splits[2].split(",").collect::<Vec<_>>()[0]
                .parse::<usize>()
                .unwrap();
            bb.0 = splits[3].parse::<usize>().unwrap();
        } else if l.contains("B") {
            let splits = l.split([':', '+']).collect::<Vec<_>>();
            ba.1 = splits[2].split(",").collect::<Vec<_>>()[0]
                .parse::<usize>()
                .unwrap();
            bb.1 = splits[3].parse::<usize>().unwrap();
        } else if l.contains("Prize") {
            let splits = l.split([':', '=']).collect::<Vec<_>>();
            prize = (
                splits[2].split(",").collect::<Vec<_>>()[0]
                    .parse::<usize>()
                    .unwrap(),
                splits[3].parse::<usize>().unwrap(),
            );
        }
    }
    machines
}

pub fn main() {
    let input = input!();

    let machines = get_machines(std::fs::read_to_string(input).unwrap());

    let res = machines
        .iter()
        .map(|m| {
            let m_with_offset = ClawMachine {
                prize: (m.prize.0 + 10000000000000, m.prize.1 + 10000000000000),
                ..*m
            };
            (m.solve(), m_with_offset.solve())
        })
        .collect::<Vec<(usize, usize)>>();
    let part1 = res.iter().fold(0, |acc, c| acc + c.0);
    let part2 = res.iter().fold(0, |acc, c| acc + c.1);

    println!("{part1} {part2}");
}

#[cfg(test)]
mod tests {
    use super::get_machines;
    #[test]
    fn test_example_input() {
        let m = get_machines(std::fs::read_to_string("input_test").unwrap())[0];
        println!("machine {:?}", m.prize);
        let res = m.solve();
        assert_eq!(res, 280);
    }

    #[test]
    fn test_input_part1() {
        let machines = get_machines(std::fs::read_to_string("input").unwrap());
        let res = machines
            .iter()
            .map(|m| m.solve())
            .filter(|sum| *sum != usize::MAX)
            .sum::<usize>();
        assert_eq!(res, 35082);
    }
}
