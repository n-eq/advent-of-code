use itertools::Itertools;

type Point = (usize, usize);

fn solve() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = args.get(1).map_or("input", |v| v);

    let binding = std::fs::read_to_string(input).unwrap();

    let ranges = binding
        .lines()
        .filter(|l| !l.is_empty())
        .map(|l| {
            let mut splits = l.split(" -> ");
            let mut subsplits = (
                splits.next().unwrap().split(","),
                splits.next().unwrap().split(","),
            );
            let (xs, ys) = (
                subsplits.0.next().unwrap().parse::<usize>().unwrap(),
                subsplits.0.next().unwrap().parse::<usize>().unwrap(),
            );
            let (xe, ye) = (
                subsplits.1.next().unwrap().parse::<usize>().unwrap(),
                subsplits.1.next().unwrap().parse::<usize>().unwrap(),
            );
            ((xs, ys), (xe, ye))
        })
        .collect::<Vec<(Point, Point)>>();

    let part1 = ranges
        .clone()
        .iter()
        .filter(|((xs, ys), (xe, ye))| xs == xe || ys == ye)
        .map(|((xs, ys), (xe, ye))| {
            let (minx, maxx) = if xs > xe { (xe, xs) } else { (xs, xe) };
            let (miny, maxy) = if ys > ye { (ye, ys) } else { (ys, ye) };

            let mut r = vec![];
            for x in *minx..=*maxx {
                for y in *miny..=*maxy {
                    r.push((x, y));
                }
            }
            r
        })
        .flatten()
        .duplicates()
        .count();

    let part2 = ranges
        .iter()
        .map(|((xs, ys), (xe, ye))| {
            if xs == xe || ys == ye {
                let (minx, maxx) = if xs > xe { (xe, xs) } else { (xs, xe) };
                let (miny, maxy) = if ys > ye { (ye, ys) } else { (ys, ye) };

                let mut r = vec![];
                for x in *minx..=*maxx {
                    for y in *miny..=*maxy {
                        r.push((x, y));
                    }
                }
                r
            } else {
                let mut points = vec![];
                let (mut y, y_inc, r) = if xs > xe {
                    (*ye as isize, if ye > ys { -1 } else { 1 }, *xe..=*xs)
                } else {
                    (*ys as isize, if ye < ys { -1 } else { 1 }, *xs..=*xe)
                };
                r.for_each(|x| {
                    let p = (x as usize, y as usize);
                    points.push(p);
                    y += y_inc;
                });
                points
            }
        })
        .flatten()
        .duplicates()
        .count();

    println!("part1: {part1:?}");
    println!("part2: {part2:?}");
}

fn main() {
    solve();
}
