fn safe_report(v: Vec<u8>) -> bool {
    // there are always at least 2 levels per report
    let diff: i8 = if v[0] > v[1] {
        1
    } else if v[0] < v[1] {
        -1
    } else {
        return false;
    };

    v.windows(2).all(|w| {
        (w[0] as i8 - w[1] as i8) as i8 * diff > 0
            && w[0].abs_diff(w[1]) >= 1
            && w[0].abs_diff(w[1]) <= 3
    })
}

pub fn main() {
    //     let reports: Vec<Vec<u8>> = include_str!("input_test")
    let reports: Vec<Vec<u8>> = include_str!("input")
        .lines()
        .collect::<Vec<&str>>()
        .iter()
        .map(|r| {
            r.split(" ")
                .map(|l| l.parse::<u8>().unwrap())
                .collect::<Vec<u8>>()
        })
        .collect();

    let (safe_reports, unsafe_reports): (_, Vec<_>) = reports
        .clone()
        .into_iter()
        .partition(|r| safe_report(r.to_vec()));

    let fully_safe_reports = safe_reports.len();
    println!("part1: {fully_safe_reports:?}");

    // For part2, we iterate over unsafe reports, and see if any of them becomes safe if we
    // retain one level from it
    let mut safe_by_removing_one_level = 0;
    'next_report: for report in unsafe_reports {
        for level in 0..report.len() {
            // the hypothetical report obtained from removing the nth level from the current report
            let mut hypo_report = report.clone();
            hypo_report.remove(level);

            if safe_report(hypo_report) {
                safe_by_removing_one_level += 1;
                continue 'next_report;
            }
        }
    }

    println!(
        "part2: {:?}",
        safe_by_removing_one_level + fully_safe_reports
    );
}
