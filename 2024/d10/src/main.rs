use std::collections::HashSet;
use std::collections::VecDeque;

type Point = (isize, isize);

fn get_neighbors(p: Point) -> Vec<Point> {
    [(1, 0), (-1, 0), (0, 1), (0, -1)]
        .iter()
        .map(|d| (d.0 + p.0, d.1 + p.1))
        .collect()
}

fn find_paths(
    map: &Vec<Vec<usize>>,
    origin: Point,
    dest: Point,
    path: &mut Vec<Point>,
) -> Vec<Vec<Point>> {
    let mut result = Vec::new();
    let n = map.len() as isize;

    // Check if the neighbor is valid
    fn is_valid(
        current: Point,
        neighbor: Point,
        n: isize,
        map: &Vec<Vec<usize>>,
        visited: &Vec<Point>,
    ) -> bool {
        let (cx, cy) = current;
        let (nx, ny) = neighbor;
        nx >= 0 && ny >= 0 && nx < n && ny < n // Ensure neighbor is within bounds
            && !visited.contains(&neighbor)
            && map[nx as usize][ny as usize] as isize - map[cx as usize][cy as usize] as isize == 1
    }

    // Base case: if the origin is the same as the destination
    if origin == dest {
        path.push(origin);
        result.push(path.clone());
        path.pop(); // Backtrack
        return result;
    }

    // Mark the current cell as visited
    path.push(origin);

    // Explore all possible directions
    for next in get_neighbors(origin) {
        if is_valid(origin, next, n, map, path) {
            result.extend(find_paths(map, next, dest, path));
        }
    }

    // Backtrack: unmark the current cell as visited
    path.pop();

    result
}

fn get_rating(map: &Vec<Vec<usize>>, trailhead: Point) -> usize {
    map.iter()
        .enumerate()
        .map(|(i, row)| {
            let mut res: Vec<Point> = vec![];
            res.extend(
                row.iter()
                    .enumerate()
                    .filter(|(_, height)| *height == &9)
                    .map(|(j, _)| (i as isize, j as isize)),
            );
            res
        })
        .flatten()
        .map(|hiking_trail| {
            let mut paths: Vec<Point> = vec![];
            HashSet::<Vec<Point>>::from_iter(
                find_paths(map, trailhead, hiking_trail, &mut paths).into_iter(),
            )
            .len()
        })
        .sum()
}

fn get_score(map: &Vec<Vec<usize>>, trailhead: Point) -> usize {
    let mut visited: HashSet<Point> = HashSet::from([trailhead]);
    let mut queue: VecDeque<Point> = VecDeque::from([trailhead]);
    let map_size = map.len();

    while let Some(node) = queue.pop_front() {
        for neighbor in get_neighbors(node) {
            if neighbor.0 < 0
                || neighbor.0 >= map_size as isize
                || neighbor.1 < 0
                || neighbor.1 >= map_size as isize
            {
                continue;
            }

            if map[neighbor.0 as usize][neighbor.1 as usize] as isize
                - map[node.0 as usize][node.1 as usize] as isize
                != 1
            {
                continue;
            }

            if !visited.contains(&neighbor) {
                visited.insert(neighbor);
                queue.push_back(neighbor);
            }
        }
    }
    visited
        .iter()
        .filter(|(i, j)| map[*i as usize][*j as usize] == 9)
        .count()
}

fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = args.get(1).map_or("input_test", |v| v);

    let mut map: Vec<Vec<usize>> = vec![];
    for (_, l) in std::fs::read_to_string(input).unwrap().lines().enumerate() {
        map.push(
            l.chars()
                .map(|c| c.to_digit(10).unwrap_or(u32::MAX) as usize)
                .collect::<Vec<_>>(),
        );
    }

    // until the .flatten() call, get the trailheads from the map
    // then, map each trailhead to its score (part1) and its rating (part2)
    // the rest is a call to fold to sum the tuples for each trailhead
    let results = map
        .iter()
        .enumerate()
        .map(|(i, row)| {
            let mut res: Vec<Point> = vec![];
            res.extend(
                row.iter()
                    .enumerate()
                    .filter(|(_, height)| *height == &0)
                    .map(|(j, _)| (i as isize, j as isize)),
            );
            res
        })
        .flatten()
        .map(|t| (get_score(&map, t), get_rating(&map, t)))
        .collect::<Vec<(usize, usize)>>()
        .iter()
        .fold((0, 0), |(acc0, acc1), (score, rating)| {
            (acc0 + score, acc1 + rating)
        });
    println!("{results:?}");
}
