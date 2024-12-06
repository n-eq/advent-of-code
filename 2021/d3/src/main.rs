fn part1(numbers: &Vec<Vec<char>>) -> isize {
    let mut gamma = String::new();
    let mut epsilon = String::new();

    for col in 0..numbers[0].len() {
        if numbers
            .iter()
            .fold(0, |acc, n| if n[col] == '0' { acc + 1 } else { acc - 1 })
            > 0
        {
            gamma.push('0');
            epsilon.push('1');
        } else {
            gamma.push('1');
            epsilon.push('0');
        }
    }

    isize::from_str_radix(&gamma, 2).unwrap() * isize::from_str_radix(&epsilon, 2).unwrap()
}

fn part2(numbers: &Vec<Vec<char>>) -> isize {
    let mut numbers_to_consider = numbers.clone();
    for col in 0..numbers_to_consider[0].len() {
        if numbers_to_consider.len() == 1 {
            break;
        }

        let occurrences_col = numbers_to_consider
            .iter()
            .map(|n| n[col])
            .collect::<Vec<char>>();
        let it = occurrences_col.into_iter();

        let ones = it.clone().filter(|c| *c == '1').count();
        let zeros = it.clone().filter(|c| *c == '0').count();
        let new_numbers = if ones >= zeros {
            // more 1s than 0s; take number that have '1' in their current col
            numbers_to_consider
                .iter()
                .filter(|n| n[col] == '1')
                .map(|v| v.clone())
                .collect()
        } else {
            numbers_to_consider
                .iter()
                .filter(|n| n[col] == '0')
                .map(|v| v.clone())
                .collect()
        };

        numbers_to_consider = new_numbers;
    }

    let oxygen_rating: String = numbers_to_consider[0].clone().into_iter().collect();

    let mut numbers_to_consider = numbers.clone();
    for col in 0..numbers_to_consider[0].len() {
        if numbers_to_consider.len() == 1 {
            break;
        }

        let occurrences_col = numbers_to_consider
            .iter()
            .map(|n| n[col])
            .collect::<Vec<char>>();
        let it = occurrences_col.into_iter();

        let ones = it.clone().filter(|c| *c == '1').count();
        let zeros = it.clone().filter(|c| *c == '0').count();
        let new_numbers = if zeros <= ones {
            numbers_to_consider
                .iter()
                .filter(|n| n[col] == '0')
                .map(|v| v.clone())
                .collect()
        } else {
            numbers_to_consider
                .iter()
                .filter(|n| n[col] == '1')
                .map(|v| v.clone())
                .collect()
        };

        numbers_to_consider = new_numbers;
    }
    let scrubber_rating: String = numbers_to_consider[0].clone().into_iter().collect();

    isize::from_str_radix(&oxygen_rating, 2).unwrap()
        * isize::from_str_radix(&scrubber_rating, 2).unwrap()
}

fn main() {
    let args = std::env::args().collect::<Vec<String>>();
    let input = args.get(1).map_or("input_test", |v| v);

    let numbers = std::fs::read_to_string(input)
        .unwrap()
        .lines()
        .map(|l| l.chars().collect::<Vec<char>>())
        .collect::<Vec<Vec<char>>>();

    println!("part1: {}", part1(&numbers));
    println!("part2: {}", part2(&numbers));
}
