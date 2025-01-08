use aoc::*;
use std::collections::HashMap;

type GateValue = u64;
type GateName<'a> = &'a str;

enum Operand {
    AND,
    OR,
    XOR,
}

struct Operation<'a> {
    op1: GateName<'a>,
    op2: GateName<'a>,
    op: Operand,
    op3: GateName<'a>,
}

pub(crate) fn main() {
    let input = std::fs::read_to_string(input!()).unwrap();
    let mut gates: HashMap<GateName, GateValue> = input
        .lines()
        .filter(|l| l.contains(':'))
        .map(|l| {
            let mut splits = l.split(": ");
            (
                splits.next().unwrap(),
                splits.next().unwrap().parse::<GateValue>().unwrap(),
            )
        })
        .collect();
    let mut operations = input
        .lines()
        .filter(|l| l.contains("->"))
        .map(|line| {
            let mut splits = line.split(" -> ");
            let lhs = splits.next().unwrap();
            let rhs = splits.next().unwrap();

            let mut lhs_split = lhs.split(' ');
            let gate1 = lhs_split.next().unwrap();
            let op = lhs_split.next().unwrap();
            let gate2 = lhs_split.next().unwrap();

            Operation {
                op1: gate1,
                op2: gate2,
                op: match op {
                    "AND" => Operand::AND,
                    "OR" => Operand::OR,
                    "XOR" => Operand::XOR,
                    _ => unreachable!(),
                },

                op3: rhs,
            }
        })
        .collect::<Vec<Operation>>();

    while let Some(next) = operations
        .iter()
        .position(|o| gates.get(o.op1).is_some() && gates.get(o.op2).is_some())
    {
        let op = operations.remove(next);

        let g1 = &gates.get(op.op1).unwrap();
        let g2 = &gates.get(op.op2).unwrap();

        let res = match op.op {
            Operand::AND => *g1 & *g2,
            Operand::OR => *g1 | *g2,
            Operand::XOR => *g1 ^ *g2,
        };
        gates.insert(op.op3, res);
    }

    let mut z = gates
        .into_iter()
        .filter(|(k, _)| k.starts_with("z"))
        .collect::<Vec<(GateName, GateValue)>>();
    z.sort_by(|(k1, _), (k2, _)| k2.cmp(k1));

    let part1 = z.iter().fold(0, |acc, (_, val)| acc * 2 + val);
    println!("{part1}");

    // For part2, I used graphviz to identify the swapped gates, see
    // graph.dot for the input files (reconstructed using Vimscript),
    // and graph.svg for the result data.
    // My main "contribution" to help visualize is to mark xii and yii
    // nodes as "max" so they're at the top of the image, and zii as "sink"
    // to have them at the bottom as much as possible.
}
