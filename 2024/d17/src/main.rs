use rayon::prelude::*;

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

    fn exec_single(&mut self, ins_op: (Instruction, Operand)) -> Option<String> {
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
            Instruction::Out => return Some(((self.get_combo_value(op) as u64) % 8).to_string()),
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

    fn exec(&mut self, ins_op: &Vec<(Instruction, Operand)>) -> String {
        let mut outputs: Vec<u64> = vec![];

        while let Some((next_instruction, op)) = ins_op.get(self.ip / 2) {
            let op = *op;
            match next_instruction {
                Instruction::Adv => self.regs[0] /= 2u64.pow(self.get_combo_value(op) as u32),
                Instruction::Bxl => self.regs[1] ^= op as u64,
                Instruction::Bst => self.regs[1] = (self.get_combo_value(op) as u64) % 8,
                Instruction::Jnz => {
                    if self.regs[0] != 0 {
                        self.ip = op as usize;
                        // skip ip += 2
                        continue;
                    }
                }
                Instruction::Bxc => self.regs[1] ^= self.regs[2],
                Instruction::Out => outputs.push((self.get_combo_value(op) as u64) % 8),
                Instruction::Bdv => {
                    self.regs[1] = self.regs[0] / 2u64.pow(self.get_combo_value(op) as u32)
                }
                Instruction::Cdv => {
                    self.regs[2] = self.regs[0] / 2u64.pow(self.get_combo_value(op) as u32)
                }
            }
            self.ip += 2;
        }

        outputs
            .iter()
            .map(|o| o.to_string())
            .collect::<Vec<String>>()
            .join(",")
    }

    fn find_rega(
        &self,
        instructions: &Vec<(Instruction, Operand)>,
        program_str: String,
    ) -> Option<u64> {
        let start = std::time::Instant::now();

        // number of values = number of ',' + 1
        let numbers = program_str.chars().filter(|c| c == &',').count() as u32 + 1;

        let initial_computer = self.clone();

        /*
         * a: 0..7 -> 1 number
         * a: 8..63 -> 2 numbers
         * a: 64..511 -> 3 numbers (8^2..8^3 -> 3 numbers)
         *
         * OR: 8^(n-1)..(8^n)-1 -> n numbers
         * a: 8^n..8^(n+1)-1 -> n + 1 numbers (or 2^(3n)..2^(3n + 3) -1)
         * we want:
         * 16 numbers:
         * it means a should be between: 8^16 and 8^17 -1, i.e.: 2^48 and 2^51 -1
         */
        // we should look for rega value in this space
        let rega_min = 8u64.pow(numbers - 1);
        let rega_max = 8u64.pow(numbers) - 1;
        let space_size = rega_max - rega_min;

        let mut last_elapsed = 0;
        for a in 0..=rega_max {
            let progress = (100 as f32 * (a.abs_diff(rega_min) / space_size) as f32) as u32;
            let elapsed = start.elapsed().as_secs() / 60;

            let mut c = initial_computer.clone();
            c.regs[0] = a;

            let output = c.exec(&instructions);
            //             assert!(output.len() == program_str.len());

            //             println!("chkeing {a:?} , {output:?} {program_str:?}");
            //             if (a as f64).log(8f64).fract() == 0.0 {
            println!("dbg {a:?} , {:>20},  {:>20}", output, program_str);
            //             }

            //             if elapsed % 2 == 0 && last_elapsed != elapsed {
            //                 println!("{:?} progress: {progress:?}% {}/{}", elapsed, a, rega_max);
            //                 println!("{a:?} , {output:?} {program_str:?}");
            //                 last_elapsed = elapsed;
            //             }

            if program_str == output {
                println!("FOUND !!!!!!!!!!!!!!");
                return Some(a);
            }
        }
        None
    }
}

fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = std::fs::read_to_string(args.get(1).map_or("input_test", |v| v)).unwrap();

    let (mut computer, instructions) = parse_input(&input);

    let part1 = computer.clone().exec(&instructions);
    println!("{part1:?}");
    */

    let program_str = instructions
        .iter()
        .map(|(i, op)| (*i, *op))
        .map(|(i, op)| [(i as u64).to_string(), (op as u64).to_string()].join(","))
        .collect::<Vec<String>>()
        .join(",");


    println!(
        "part2: {:?}",
        computer.find_rega(&instructions, program_str)
    );
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
