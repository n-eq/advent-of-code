use std::collections::HashSet;

// Given the initial position, loop in the map and count the number
// of visited locations until leaving the area
// I GOT THIS WORKING FROM THE FIRST TRY!
fn part1(map: &Vec<Vec<char>>, pos: (isize, isize)) -> usize {
    let mut visited: HashSet<(isize, isize)> = HashSet::new();
    let mut guard = pos.clone();
    let mut local_map = map.clone();
    loop {
        visited.insert(guard);

        let next = match local_map[guard.0 as usize][guard.1 as usize] {
            '^' => (guard.0 - 1, guard.1),
            '>' => (guard.0, guard.1 + 1),
            'v' => (guard.0 + 1, guard.1),
            '<' => (guard.0, guard.1 - 1),
            _ => unreachable!(),
        };

        // if next coordinate is out of bounds, we should stop
        if next.0 < 0
            || next.0 >= local_map.len() as isize
            || next.1 < 0
            || next.1 >= local_map[0].len() as isize
        {
            break;
        }

        if local_map[next.0 as usize][next.1 as usize] == '#' {
            // we should turn right
            match local_map[guard.0 as usize][guard.1 as usize] {
                '^' => local_map[guard.0 as usize][guard.1 as usize] = '>',
                '>' => local_map[guard.0 as usize][guard.1 as usize] = 'v',
                'v' => local_map[guard.0 as usize][guard.1 as usize] = '<',
                '<' => local_map[guard.0 as usize][guard.1 as usize] = '^',
                _ => unreachable!(),
            }

            // guard (next position) stays as is
        } else {
            // we continue
            let tmp = local_map[next.0 as usize][next.1 as usize];
            local_map[next.0 as usize][next.1 as usize] =
                local_map[guard.0 as usize][guard.1 as usize];
            local_map[guard.0 as usize][guard.1 as usize] = tmp;

            guard = next;
        }
    }

    visited.len()
}

// blatant copy of part1, with a twist:
// the visited hashmap now takes an additional value: the direction, it's useful to detect when
// we're looping (going through the same position, in the same direction as before)
// As soon as we detect a visited position (with the same direction), we return true and consider
// the alternative map as a valid configuration, otherwise it's probably a map with an out of
// bounds config
fn part2(map: &Vec<Vec<char>>, pos: (isize, isize)) -> bool {
    let mut visited: HashSet<(isize, isize, char /* direction */)> = HashSet::new();
    let mut guard = pos.clone();
    let mut local_map = map.clone();
    loop {
        if !visited.insert((
            guard.0 as isize,
            guard.1 as isize,
            local_map[guard.0 as usize][guard.1 as usize],
        )) {
            return true;
        }

        let next = match local_map[guard.0 as usize][guard.1 as usize] {
            '^' => (guard.0 - 1, guard.1),
            '>' => (guard.0, guard.1 + 1),
            'v' => (guard.0 + 1, guard.1),
            '<' => (guard.0, guard.1 - 1),
            _ => unreachable!(),
        };

        // if next coordinate is out of bounds, we should stop
        if next.0 < 0
            || next.0 >= local_map.len() as isize
            || next.1 < 0
            || next.1 >= local_map[0].len() as isize
        {
            break;
        }

        if local_map[next.0 as usize][next.1 as usize] == '#' {
            // we should turn right
            match local_map[guard.0 as usize][guard.1 as usize] {
                '^' => local_map[guard.0 as usize][guard.1 as usize] = '>',
                '>' => local_map[guard.0 as usize][guard.1 as usize] = 'v',
                'v' => local_map[guard.0 as usize][guard.1 as usize] = '<',
                '<' => local_map[guard.0 as usize][guard.1 as usize] = '^',
                _ => unreachable!(),
            }

            // guard (next position) stays as is
        } else {
            // we continue
            let tmp = local_map[next.0 as usize][next.1 as usize];
            local_map[next.0 as usize][next.1 as usize] =
                local_map[guard.0 as usize][guard.1 as usize];
            local_map[guard.0 as usize][guard.1 as usize] = tmp;

            guard = next;
        }
    }

    false
}

pub fn main() {
    let input = crate::utils::input!();

    let map: Vec<Vec<char>> = std::fs::read_to_string(input)
        .unwrap()
        .lines()
        .map(|l| l.chars().collect())
        .collect();

    // a bit crappy to find the initial position of '^'
    let guard_pos = map
        .iter()
        .enumerate()
        .map(|(i, l)| (i, l.iter().position(|&c| c == '^')))
        .filter(|(_, j)| j.is_some())
        .map(|(i, j)| (i, j.unwrap()))
        .collect::<Vec<_>>()[0];

    println!(
        "{}",
        part1(&map, (guard_pos.0 as isize, guard_pos.1 as isize))
    );

    // bruteforce solution for part 2:
    // iterate all over the map, and try to see if the map with an added obstruction
    // at position (i,j) makes it a valid configuration (internal loop)
    let mut p2 = 0usize;
    for i in 0..map.len() {
        for j in 0..map[0].len() {
            if map[i][j] == '.' {
                let mut map_alt = map.clone();
                map_alt[i][j] = '#';

                if part2(&map_alt, (guard_pos.0 as isize, guard_pos.1 as isize)) {
                    p2 += 1;
                }
            }
        }
    }
    println!("{p2}");
}
