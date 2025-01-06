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
    let input = std::fs::read_to_string(crate::utils::input!()).unwrap();
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

    let mut v = gates
        .clone()
        .into_iter()
        .filter(|(k, _)| k.starts_with("z"))
        .collect::<Vec<(&str, GateValue)>>();
    v.sort_by(|(k1, _), (k2, _)| k1.cmp(k2));
    v.reverse();

    let part1 = v.iter().fold(0, |acc, (_, val)| acc * 2 + val);
    println!("{part1}");
}
