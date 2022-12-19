// compile & run: cargo r
use std::collections::HashSet;

type Point = (i32, i32);

pub fn parse_point_from_input(line: &str) -> Point {
    let mut words = line.split(", ");
    let x: i32 = words.next().unwrap().replace("x=", "").parse().unwrap();
    let y: i32 = words.next().unwrap().replace("y=", "").parse().unwrap();
    (x, y)
}

fn taxicab_distance(p1: &Point, p2: &Point) -> i32 {
    (p1.0 - p2.0).abs() + (p1.1 - p2.1).abs()
}

fn main() {
    // let mut data = include_str!("../input_test")
    let mut data = include_str!("../input")
        .lines()
        .map(|line| {
            let line = line
                .replace("Sensor at ", "")
                .replace(" closest beacon is at ", "");
            let mut words = line.split(":");
            let sensor_position = parse_point_from_input(words.next().unwrap());
            let beacon_position = parse_point_from_input(words.next().unwrap());
            (sensor_position, beacon_position)
        })
        .collect::<Vec<(Point, Point)>>();
    let beacons = data.iter().map(|(_, b)| *b).collect::<Vec<Point>>();

    // println!("data: {:?}", data);

    let target_row = 2000000;

    /* Positions on which a beacon cannot be present on the target row */
    let mut forbidden: HashSet<Point> = HashSet::new();
    let mut part1 = 0;
    for (s, b) in &data {
        let ref_distance = taxicab_distance(&s, &b);

        if (s.1 > target_row && s.1 - ref_distance > target_row)
            || (s.1 < target_row && s.1 + ref_distance < target_row)
        {
            /* Sensor s radius (distance to its beacon) has not enough
             * radius to reach the target row, no need to check it */
            continue;
        }

        /* We store all the visited points in a hashsetp to avoid exploring them
         * more than once */
        let mut visited: HashSet<Point> = HashSet::new();

        /* Queue to implement a BFS starting from the position of the current sensor */
        let mut queue: Vec<Point> = vec![*s];
        while let Some(point) = queue.pop() {
            /* Reduce explored positions based on the direction we need to take: up, bottom, or
             * same row neighbors if we're on the target row.
             *
             * This is necessary to avoid a very long brute-force exploration (couldn't finish
             * part1 without it)
             */
            let neighbors_to_visit = if point.1 < target_row {
                vec![(0, 1)]
            } else if point.1 > target_row {
                vec![(0, -1)]
            } else {
                vec![(-1, 0), (1, 0)]
            };

            for (dx, dy) in neighbors_to_visit {
                let p: Point = (point.0 + dx, point.1 + dy);

                /* Avoid infinite looping */
                if visited.contains(&p) {
                    continue;
                }

                /* Positions already occupied by beacons shouldn't be considered */
                if beacons.contains(&p) {
                    continue;
                }

                /* Keep an eye on the ref_distance (distance from the current sensor to its closest
                 * beacon) to make sure we're in a valid range */
                if taxicab_distance(&s, &p) > ref_distance {
                    continue;
                }

                /*
                 * Target row check
                 * /!\\ don't return if it the current point not on the target_row, as the adjacent elements
                 * (-1, 0), (1, 0), (0, -1), (0, 1) could legitimately be valid positions
                 */
                if p.1 == target_row {
                    /* Avoid couting the same position more than once */
                    if forbidden.insert(p) {
                        part1 = part1 + 1;
                    }
                }

                /* Push to the queue */
                if !queue.contains(&p) {
                    queue.push(p);
                }
                visited.insert(p);
            }
        }
    }
    println!("part1: {}", part1);

    // part2
    let max = 4000000;
    // sort by column (x)
    data.sort_by(|e1, e2| e1.0.cmp(&e2.0));

    'outer: for row in 0..=max {
        let mut col = 0;
        while col <= max {
            let mut found: bool = true;
            for (s, b) in &data {
                let ref_dist = taxicab_distance(s, b);
                if taxicab_distance(s, &(col, row)) <= ref_dist {
                    let delta_col = ref_dist - (s.1 - row).abs();

                    col += (s.0 - col).signum() * (s.0 - col).abs() + delta_col;
                    found = false;
                    break;
                }
            }

            if found {
                println!("part2: {}", col * max + row);
                break 'outer;
            }

            col += 1;
        }
    }
}
