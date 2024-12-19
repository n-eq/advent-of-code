type Regs = [u64; 3];
type Operand = u8;

#[repr(u8)]
#[derive(Debug, Copy, Clone)]
enum Instruction {
    // TODO: maybe merge Adv, Bdv, Cdv ?
    Adv = 0,
    Bxl = 1,
    Bst = 2,
    Jnz = 3,
    Bxc = 4,
    Out = 5,
    Bdv = 6,
    Cdv = 7,
}

impl Instruction {
    fn from_u8(m: u8) -> Self {
        match m {
            0 => Instruction::Adv,
            1 => Instruction::Bxl,
            2 => Instruction::Bst,
            3 => Instruction::Jnz,
            4 => Instruction::Bxc,
            5 => Instruction::Out,
            6 => Instruction::Bdv,
            7 => Instruction::Cdv,
            _ => unreachable!(),
        }
    }
}

#[derive(Debug, Clone, Copy)]
struct Computer {
    regs: Regs,
    ip: usize,
}

fn parse_input(input: &String) -> (Computer, Vec<(Instruction, Operand)>) {
    let mut regs = [0; 3];
    let mut ins_op: Vec<(Instruction, Operand)> = vec![];
    for l in input.lines() {
        if l == "\n" {
            continue;
        }

        if l.contains("Register A") {
            regs[0] = l.split(": ").collect::<Vec<&str>>()[1]
                .parse::<u64>()
                .unwrap();
        }
        if l.contains("Register B") {
            regs[1] = l.split(": ").collect::<Vec<&str>>()[1]
                .parse::<u64>()
                .unwrap();
        }
        if l.contains("Register C") {
            regs[2] = l.split(": ").collect::<Vec<&str>>()[1]
                .parse::<u64>()
                .unwrap();
        }

        if l.contains("Program") {
            let splits = l.split(": ").collect::<Vec<&str>>()[1]
                .split(",")
                .collect::<Vec<&str>>();

            let mut i = 0usize;
            while i < splits.len() - 1 {
                let instruction = Instruction::from_u8(splits[i].parse::<u8>().unwrap());
                let operand = splits[i + 1].parse::<Operand>().unwrap();

                ins_op.push((instruction, operand));
                i += 2;
            }
        }
    }

    (Computer { regs, ip: 0 }, ins_op)
}

impl Computer {
    fn get_combo_value(&self, op: Operand) -> u64 {
        match op {
            0..=3 => op as u64,
            4..=6 => self.regs[op as usize - 4],
            _ => unreachable!(),
        }
    }

    fn exec_single(&mut self, ins_op: (Instruction, Operand)) -> Option<u64> {
        let (next_instruction, op) = ins_op;
        match next_instruction {
            Instruction::Adv => self.regs[0] /= 2u64.pow(self.get_combo_value(op) as u32),
            Instruction::Bxl => self.regs[1] ^= op as u64,
            Instruction::Bst => self.regs[1] = (self.get_combo_value(op) as u64) % 8,
            Instruction::Jnz => {
                if self.regs[0] != 0 {
                    self.ip = op as usize;
                    // skip ip += 2
                    return None;
                }
            }
            Instruction::Bxc => self.regs[1] ^= self.regs[2],
            Instruction::Out => return Some((self.get_combo_value(op) as u64) % 8),
            Instruction::Bdv => {
                self.regs[1] = self.regs[0] / 2u64.pow(self.get_combo_value(op) as u32)
            }
            Instruction::Cdv => {
                self.regs[2] = self.regs[0] / 2u64.pow(self.get_combo_value(op) as u32)
            }
        }
        self.ip += 2;
        None
    }

    fn exec(&mut self, ins_op: &Vec<(Instruction, Operand)>) -> Vec<u64> {
        ins_op
            .iter()
            .map(|ins_op| self.exec_single(*ins_op))
            .flatten()
            .collect()
    }

    #[allow(dead_code)]
    fn nth_output(&self, a: u64, n: usize) -> u64 {
        // after spending some time using a pen and a paper, I found that for my input:
        // if A = An A(n-1) A(n-2) ... A(1) A(0)
        // where A(i) is a number in base 8 (3 bits)
        // then
        // out = (an ^ 1 ^ 5) ^ (A / 2^(An ^1))
        let an: u32 = (a >> (3 * n) & 0b111) as u32;
        ((an ^ 4) as u64 ^ (a / 2u64.pow(an ^ 1))) % 8
    }

    // Execute instructions with a given value for register A
    fn sim_rega(&self, rega: u64, instructions: &Vec<(Instruction, Operand)>) -> u64 {
        let mut c = self.clone();
        c.regs[0] = rega;

        c.exec(instructions)[0]
    }

    // Find the value of register A that will produce the given program
    // This is a brute force solution, but it works by iterating over all possible values of
    // register A, taking chunks of 3 bits each time, and checking if the output is the expected
    // one (for part 2 A is of length 48 bits = 16 chunks of 3 bits)
    fn find_reg_a(
        &self,
        ainit: u64,
        prog: &Vec<u64>,
        instructions: &Vec<(Instruction, Operand)>,
        i: usize,
    ) -> Option<u64> {
        let next = |a| ainit << 3 | a;
        if i == 0 {
            for a in 0..8 {
                if self.sim_rega(next(a), instructions) == prog[0] {
                    return Some(next(a));
                }
            }
        }

        for a in 0..8 {
            if self.sim_rega(next(a), instructions) == prog[i] {
                if let Some(res) = self.find_reg_a(next(a), prog, instructions, i - 1) {
                    return Some(res);
                }
            }
        }
        None
    }
}

fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = std::fs::read_to_string(args.get(1).map_or("input_test", |v| v)).unwrap();

    let (computer, instructions) = parse_input(&input);

    let part1 = computer
        .clone()
        .exec(&instructions)
        .iter()
        .map(|o| o.to_string())
        .collect::<Vec<String>>()
        .join(",");
    println!("part1: {part1}");

    let prog = instructions
        .iter()
        .map(|(i, op)| (*i, *op))
        .map(|(i, op)| vec![(i as u64), (op as u64)])
        .flatten()
        .collect::<Vec<u64>>();

    let part2 = computer
        .find_reg_a(0, &prog, &instructions, prog.len() - 1)
        .unwrap();
    println!("part2: {part2:?}");
}

#[cfg(test)]
mod tests {
    use crate::*;

    #[test]
    fn test_parse_example_input() {
        let (c, _) = parse_input(&std::fs::read_to_string("input_test").unwrap());
        assert_eq!(c.regs, [729, 0, 0]);
    }
}
