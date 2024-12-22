pub fn main() {
    let mut left: Vec<u32> = vec![];
    let mut right: Vec<u32> = vec![];
    for line in include_str!("input").lines() {
        let mut numbers = line.split("   ");
        left.push(numbers.next().unwrap().parse::<u32>().unwrap());
        right.push(numbers.next().unwrap().parse::<u32>().unwrap());
    }

    left.sort();
    right.sort();

    let distance: u32 = left
        .iter()
        .zip(right.clone())
        .map(|(l, r)| if *l > r { l - r } else { r - l })
        .sum();
    println!("distance {distance:}");

    let similarity: u32 = left
        .iter()
        .map(|l| *l * right.iter().filter(|&r| *r == *l).count() as u32)
        .sum();
    println!("similarity: {similarity:?}");
}
