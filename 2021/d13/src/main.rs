use std::collections::HashSet;

type Point = (usize, usize);

fn parse() -> (HashSet<Point>, Vec<Point>) {
    let mut points: HashSet<Point> = HashSet::new();
    let mut instructions: Vec<Point> = vec![];

    for line in include_str!("../input").lines().filter(|l| !l.is_empty()) {
        if line.contains("fold") {
            let mut sp = line.split('=');
            sp.next();
            if line.contains("x") {
                instructions.push((sp.next().unwrap().parse::<usize>().unwrap(), 0));
            } else {
                instructions.push((0, sp.next().unwrap().parse::<usize>().unwrap()));
            }
        } else {
            points.insert((
                line.split(',').collect::<Vec<_>>()[0]
                    .parse::<usize>()
                    .unwrap(),
                line.split(',').collect::<Vec<_>>()[1]
                    .parse::<usize>()
                    .unwrap(),
            ));
        }
    }

    (points, instructions)
}

fn main() {
    let (mut points, instructions) = parse();

    for (i, (insx, insy)) in instructions.iter().enumerate() {
        points = if insx == &0 {
            // fold y=...
            points
                .iter()
                .map(|(col, row)| {
                    if row > insy {
                        (*col, insy * 2 - *row)
                    } else {
                        (*col, *row)
                    }
                })
                .collect::<HashSet<Point>>()
        } else if insy == &0 {
            // fold x=...
            points
                .iter()
                .map(|(col, row)| {
                    if col > insx {
                        (insx * 2 - *col, *row)
                    } else {
                        (*col, *row)
                    }
                })
                .collect::<HashSet<Point>>()
        } else {
            unreachable!();
        };

        if i == 0 {
            println!("part1: {:?}", points.len());
        }
    }

    let maxc = points.iter().max_by_key(|(col, _)| col).unwrap().0;
    let maxr = points.iter().max_by_key(|(_, row)| row).unwrap().1;
    for r in 0..=maxr {
        for c in 0..=maxc {
            if points.contains(&(c, r)) {
                print!("#");
            } else {
                print!(".");
            }
        }
        println!();
    }
}
