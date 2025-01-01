type Pos = (isize, isize);

fn get_lowest_risk(part2: bool, cavern: &Vec<Vec<usize>>) -> usize {
    let len = cavern.len();
    let start = (0 as isize, 0 as isize);
    let end = if part2 {
        (5 * len as isize - 1, 5 * len as isize - 1)
    } else {
        (len as isize - 1, len as isize - 1)
    };
    pathfinding::prelude::dijkstra(
        &start,
        |&(x, y)| {
            [(-1, 0), (1, 0), (0, 1), (0, -1)]
                .iter()
                .map(|(dx, dy)| ((x + dx, y + dy)))
                .filter(|(x, y)| {
                    let upper_bound = if part2 { 5 * len } else { len } as isize;
                    *x >= 0 && *x < upper_bound && *y >= 0 && *y < upper_bound
                })
                .map(|(x, y)| {
                    let mut res = cavern[x as usize % len][y as usize % len]
                        + (x as usize).div_euclid(len)
                        + (y as usize).div_euclid(len);
                    if res > 9 {
                        res -= 9;
                    }
                    ((x, y), res)
                })
                .collect::<Vec<(Pos, usize)>>()
        },
        |&p| p == end,
    )
    .unwrap()
    .1
}

fn main() {
    let cavern: Vec<Vec<usize>> = include_str!("../input")
        .lines()
        .map(|line| {
            line.chars()
                .map(|c| c.to_digit(10).unwrap() as usize)
                .collect::<Vec<usize>>()
        })
        .collect();

    println!("Part 1: {:?}", get_lowest_risk(false, &cavern));
    println!("part2: {:?}", get_lowest_risk(true, &cavern));
}
