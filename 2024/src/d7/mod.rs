use itertools::Itertools;
use std::iter::repeat_n;

fn eval(res: usize, vals: &[&str]) -> usize {
    let value = vals[1].parse::<usize>().unwrap();
    match vals[0] {
        "+" => res + value,
        "*" => res * value,
        "|" => (res.to_string() + vals[1]).parse::<usize>().unwrap(),
        _ => panic!("unsupported"),
    }
}

fn solve(equations: &Vec<(usize, Vec<&str>)>) -> (usize, usize) {
    let (mut r1, mut r2) = (0usize, 0usize);
    for (expected, vals) in equations {
        'perm_loop: for perm in
            repeat_n(["*", "+", "|"].iter(), vals.len() - 1).multi_cartesian_product()
        {
            let mut v_to_eval = vec![vals[0]];
            for i in 0..perm.len() {
                v_to_eval.push(&perm[i]);
                v_to_eval.push(vals[i + 1]);
            }
            let mut res = match v_to_eval[1] {
                "+" => {
                    v_to_eval[0].parse::<usize>().unwrap() + v_to_eval[2].parse::<usize>().unwrap()
                }
                "*" => {
                    v_to_eval[0].parse::<usize>().unwrap() * v_to_eval[2].parse::<usize>().unwrap()
                }
                "|" => (v_to_eval[0].to_string() + v_to_eval[2])
                    .parse::<usize>()
                    .unwrap(),
                _ => panic!("problem"),
            };

            for i in (3..v_to_eval.len()).step_by(2) {
                let slice = [v_to_eval[i], v_to_eval[i + 1]];
                res = eval(res, &slice);
                // small optimization
                if res > *expected {
                    continue 'perm_loop;
                }
            }

            if res == *expected {
                r2 += expected;
                if perm.iter().all(|p| *p != &"|") {
                    r1 += expected;
                }
                break 'perm_loop;
            }
        }
    }
    (r1, r2)
}

pub fn main() {
    let input = crate::utils::input!();

    let binding = std::fs::read_to_string(input).unwrap();
    let equations: Vec<(usize, Vec<&str>)> = binding
        .lines()
        .map(|l| {
            let splits = l.split(": ").collect::<Vec<&str>>();
            (
                splits[0].parse::<usize>().unwrap(),
                splits[1].split(" ").collect::<Vec<&str>>(),
            )
        })
        .collect();

    println!("{:?}", solve(&equations));
}
