use regex::Regex;

pub fn main() {
    let input = std::fs::read_to_string(crate::utils::input!()).unwrap();
    let re = Regex::new(r"(mul\([0-9]{1,3},[0-9]{1,3}\))").unwrap();

    let (mut part1, mut part2) = (0usize, 0usize);

    let mut prev_end = 0;
    let mut disable = false;
    for mul in re.find_iter(&input) {
        if prev_end != 0 && prev_end < mul.start() {
            // look for the most recent do/don't in the range [prev_end, current.start]
            let re_dos = Regex::new(r"(do\(\))").unwrap();
            let re_donts = Regex::new(r"(don't\(\))").unwrap();

            let window = &input[prev_end..mul.start()];
            match (
                re_dos.find_iter(window).last(),
                re_donts.find_iter(window).last(),
            ) {
                (None, None) => (),                 // nothing to do,
                (Some(_), None) => disable = false, // only do's found
                (None, Some(_)) => disable = true,  // only don'ts found
                (Some(last_do), Some(last_dont)) => {
                    disable = last_do.start() < last_dont.start();
                }
            }
        }

        let re = Regex::new(r"mul\(([0-9]{1,3}),([0-9]{1,3})\)").unwrap();
        let captures = re.captures(mul.as_str()).unwrap();

        let product = captures[1].parse::<usize>().unwrap() * captures[2].parse::<usize>().unwrap();
        part1 += product;
        if !disable {
            part2 += product;
        }

        prev_end = mul.end() - 1;
    }

    println!("part1: {part1}, part2: {part2}");
}
