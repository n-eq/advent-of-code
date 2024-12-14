const WIDTH: isize = 101;
const HEIGHT: isize = 103;
// const WIDTH: isize = 11;
// const HEIGHT: isize = 7;

#[derive(Debug, PartialEq)]
struct Robot {
    pos: (usize, usize),
    velocity: (isize, isize),
}

impl Robot {
    fn new(l: &str) -> Self {
        let splits = l.split(' ').collect::<Vec<_>>();

        let pos = splits[0].split('=').collect::<Vec<_>>()[1];

        let px = pos.split(',').collect::<Vec<_>>()[0]
            .parse::<usize>()
            .unwrap();
        let py = pos.split(',').collect::<Vec<_>>()[1]
            .parse::<usize>()
            .unwrap();

        let veloc = splits[1].split('=').collect::<Vec<_>>()[1];
        let vx = veloc.split(',').collect::<Vec<_>>()[0]
            .parse::<isize>()
            .unwrap();
        let vy = veloc.split(',').collect::<Vec<_>>()[1]
            .parse::<isize>()
            .unwrap();

        Self {
            pos: (px, py),
            velocity: (vx, vy),
        }
    }

    fn move_in_space(&mut self) {
        let new_pos = (
            self.pos.0 as isize + self.velocity.0,
            self.pos.1 as isize + self.velocity.1,
        );

        self.pos.0 = if new_pos.0 < 0 {
            (new_pos.0 + WIDTH) as usize
        } else if new_pos.0 >= WIDTH {
            (new_pos.0 - WIDTH) as usize
        } else {
            new_pos.0 as usize
        };

        if new_pos.1 < 0 {
            self.pos.1 = (new_pos.1 + HEIGHT) as usize;
        } else if new_pos.1 >= HEIGHT {
            self.pos.1 = (new_pos.1 - HEIGHT) as usize;
        } else {
            self.pos.1 = new_pos.1 as usize;
        }
    }
}

fn parse_robots(file: &str) -> Vec<Robot> {
    std::fs::read_to_string(file)
        .unwrap()
        .lines()
        .map(|l| Robot::new(l))
        .collect::<Vec<Robot>>()
}

fn place_robots_in_map(robots: &Vec<Robot>) -> Vec<Vec<char>> {
    let mut map = vec![vec!['.'; WIDTH as usize]; HEIGHT as usize];
    robots.iter().for_each(|r| {
        let (y, x) = r.pos;
        if map[x][y] == '.' {
            map[x][y] = '1';
        } else {
            // increment
            let current_number = map[x][y].to_string().parse::<u32>().unwrap();
            let new_number = current_number + 1;
            // assume there can't be more than 9 robots at the same position at the same time
            map[x][y] = char::from_digit(new_number, 10).unwrap();
        }
    });
    map
}

fn show_map(map: &mut Vec<Vec<char>>) {
    for l in map {
        for c in l {
            if c == &'.' {
                print!(" ");
            } else {
                print!("{c}");
            }
        }
    }
    println!();
}

fn safety_factor(robots: &Vec<Robot>) -> usize {
    let mut robots_in_quadrants: Vec<usize> = vec![0; 4]; // UL, UR, LL, LR

    let binding = Vec::from_iter(0..WIDTH);
    let (splits_x_left, _) = binding.as_slice().split_at(WIDTH as usize / 2);

    let binding = Vec::from_iter(0..HEIGHT);
    let (splits_y_up, _) = binding.as_slice().split_at(HEIGHT as usize / 2);

    for r in robots {
        // skip the middle positions (the input has an odd width and height)
        if (r.pos.0 as isize) == WIDTH / 2 || (r.pos.1 as isize) == HEIGHT / 2 {
            continue;
        }

        // ugly but OK
        if splits_x_left.contains(&(r.pos.0 as isize)) {
            if splits_y_up.contains(&(r.pos.1 as isize)) {
                robots_in_quadrants[0] += 1; // UL
            } else {
                robots_in_quadrants[3] += 1; // LL
            }
        } else {
            if splits_y_up.contains(&(r.pos.1 as isize)) {
                robots_in_quadrants[1] += 1; // UR
            } else {
                robots_in_quadrants[2] += 1; // LR
            }
        }
    }
    robots_in_quadrants.iter().product()
}

fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = args.get(1).map_or("input_test", |v| v);
    let mut robots = parse_robots(input);
    println!("{robots:?}");

    let mut map: Vec<Vec<char>> = vec![];
    let mut part1: usize = 0;
    for i in 0..100 {
        robots.iter_mut().for_each(|r| r.move_in_space());
        map = place_robots_in_map(&robots);
    }

    part1 = safety_factor(&robots);
    println!("part1 {}", part1);
}

#[cfg(test)]
mod tests {
    use crate::*;

    #[test]
    fn test_example() {
        // doesn't work unless you change WIDTH AND HEIGHT values
        assert_eq!(WIDTH, 11);
        assert_eq!(HEIGHT, 7);
        let mut robots = parse_robots("input_test");
        for _ in 0..100 {
            robots.iter_mut().for_each(|r| r.move_in_space());
        }
        assert_eq!(safety_factor(&robots), 12);
    }
}
