use aoc::*;
use std::collections::HashMap;

type Rules = HashMap<usize, Vec<usize>>;

fn parse_input(lines: Vec<&str>) -> (Rules, Vec<Vec<usize>>) {
    let mut rules: Rules = HashMap::new();
    let mut updates: Vec<Vec<usize>> = vec![];

    for line in lines {
        if line.contains("|") {
            let pages = line
                .split("|")
                .map(|s| s.parse::<usize>().unwrap())
                .collect::<Vec<usize>>();
            let (lhs, rhs) = (pages[0], pages[1]);
            if let std::collections::hash_map::Entry::Vacant(e) = rules.entry(lhs) {
                // insert directly
                e.insert(vec![rhs]);
            } else {
                if let Some(vals) = rules.get_mut(&lhs) {
                    vals.push(rhs);
                }
            }
        } else if line.contains(",") {
            updates.push(
                line.split(",")
                    .map(|s| s.parse::<usize>().unwrap())
                    .collect::<Vec<usize>>(),
            );
        }
    }

    (rules, updates)
}

// reorder an incorrectly ordered update and get its result for part 2
fn reorder_faulty_update(update: &mut Vec<usize>, rules: &Rules) -> usize {
    use std::cmp::Ordering;

    update.sort_by(|a, b| {
        if let Some(r) = rules.get(a) {
            if r.contains(b) {
                return Ordering::Less;
            }
        } else if let Some(r) = rules.get(b) {
            if r.contains(a) {
                return Ordering::Greater;
            }
        }
        std::cmp::Ordering::Equal
    });
    update[update.len() / 2]
}

pub fn main() {
    let input = input!();

    let binding = std::fs::read_to_string(input).unwrap();
    let lines = binding.lines().collect::<Vec<&str>>();

    let (rules, updates) = parse_input(lines);

    // println!("rules: {rules:?}, updates: {:?}", updates);

    let mut part1 = 0usize;
    let mut part2 = 0usize;
    'updates_loop: for mut update in updates {
        // for each page number, we check that none of the page numbers in its rules are left behind it
        for i in 0..update.len() {
            let current = update[i];

            // we only check page numbers that have rules associated to them
            if let Some(rules_for_current) = &rules.get(&current) {
                if update
                    .get(0..i)
                    .unwrap_or_default()
                    .iter()
                    .any(|p| rules_for_current.contains(p))
                {
                    part2 += reorder_faulty_update(&mut update, &rules);
                    continue 'updates_loop;
                }
            }
        }

        // if we've reached here, the current update is correct, let's get it's middle page number
        part1 += update[update.len() / 2];
    }
    println!("part1 : {part1:?}");
    println!("part2 : {part2:?}");
}
