use once_cell::sync::Lazy;
use std::{collections::BTreeMap, sync::Mutex};

pub type Direction = Vec<char>;
pub type Directions = Vec<Direction>;

pub static DIRECTIONS: Lazy<Mutex<BTreeMap<(char, char), Directions>>> = Lazy::new(|| {
    let mut m = BTreeMap::new();
    m.insert(('A', 'A'), vec![vec!['A']]);
    m.insert(('A', '0'), vec![vec!['<', 'A']]);
    m.insert(
        ('A', '1'),
        vec![vec!['<', '^', '<', 'A'], vec!['^', '<', '<', 'A']],
    );
    m.insert(('A', '2'), vec![vec!['^', '<', 'A'], vec!['<', '^', 'A']]);
    m.insert(('A', '3'), vec![vec!['^', 'A']]);
    m.insert(
        ('A', '4'),
        vec![
            vec!['^', '^', '<', '<', 'A'],
            vec!['<', '^', '^', '<', 'A'],
            vec!['^', '<', '<', '^', 'A'],
            vec!['^', '<', '^', '<', 'A'],
            vec!['<', '^', '<', '^', 'A'],
        ],
    );
    m.insert(
        ('A', '5'),
        vec![
            vec!['^', '^', '<', 'A'],
            vec!['<', '^', '^', 'A'],
            vec!['^', '<', '^', 'A'],
        ],
    );
    m.insert(('A', '6'), vec![vec!['^', '^', 'A']]);
    m.insert(
        ('A', '7'),
        vec![
            vec!['^', '<', '^', '<', '^', 'A'],
            vec!['<', '^', '^', '<', '^', 'A'],
            vec!['^', '^', '<', '<', '^', 'A'],
            vec!['^', '<', '<', '^', '^', 'A'],
            vec!['<', '^', '<', '^', '^', 'A'],
            vec!['^', '^', '^', '<', '<', 'A'],
            vec!['^', '<', '^', '^', '<', 'A'],
            vec!['<', '^', '^', '^', '<', 'A'],
            vec!['^', '^', '<', '^', '<', 'A'],
        ],
    );
    m.insert(
        ('A', '8'),
        vec![
            vec!['^', '^', '^', '<', 'A'],
            vec!['^', '<', '^', '^', 'A'],
            vec!['<', '^', '^', '^', 'A'],
            vec!['^', '^', '<', '^', 'A'],
        ],
    );
    m.insert(('A', '9'), vec![vec!['^', '^', '^', 'A']]);
    m.insert(('0', 'A'), vec![vec!['>', 'A']]);
    m.insert(('0', '0'), vec![vec!['A']]);
    m.insert(('0', '1'), vec![vec!['^', '<', 'A']]);
    m.insert(('0', '2'), vec![vec!['^', 'A']]);
    m.insert(('0', '3'), vec![vec!['>', '^', 'A'], vec!['^', '>', 'A']]);
    m.insert(
        ('0', '4'),
        vec![vec!['^', '^', '<', 'A'], vec!['^', '<', '^', 'A']],
    );
    m.insert(('0', '5'), vec![vec!['^', '^', 'A']]);
    m.insert(
        ('0', '6'),
        vec![
            vec!['>', '^', '^', 'A'],
            vec!['^', '>', '^', 'A'],
            vec!['^', '^', '>', 'A'],
        ],
    );
    m.insert(
        ('0', '7'),
        vec![
            vec!['^', '^', '^', '<', 'A'],
            vec!['^', '<', '^', '^', 'A'],
            vec!['^', '^', '<', '^', 'A'],
        ],
    );
    m.insert(('0', '8'), vec![vec!['^', '^', '^', 'A']]);
    m.insert(
        ('0', '9'),
        vec![
            vec!['^', '^', '>', '^', 'A'],
            vec!['>', '^', '^', '^', 'A'],
            vec!['^', '>', '^', '^', 'A'],
            vec!['^', '^', '^', '>', 'A'],
        ],
    );
    m.insert(
        ('1', 'A'),
        vec![vec!['>', '>', 'v', 'A'], vec!['>', 'v', '>', 'A']],
    );
    m.insert(('1', '0'), vec![vec!['>', 'v', 'A']]);
    m.insert(('1', '1'), vec![vec!['A']]);
    m.insert(('1', '2'), vec![vec!['>', 'A']]);
    m.insert(('1', '3'), vec![vec!['>', '>', 'A']]);
    m.insert(('1', '4'), vec![vec!['^', 'A']]);
    m.insert(('1', '5'), vec![vec!['>', '^', 'A'], vec!['^', '>', 'A']]);
    m.insert(
        ('1', '6'),
        vec![
            vec!['>', '^', '>', 'A'],
            vec!['^', '>', '>', 'A'],
            vec!['>', '>', '^', 'A'],
        ],
    );
    m.insert(('1', '7'), vec![vec!['^', '^', 'A']]);
    m.insert(
        ('1', '8'),
        vec![
            vec!['>', '^', '^', 'A'],
            vec!['^', '>', '^', 'A'],
            vec!['^', '^', '>', 'A'],
        ],
    );
    m.insert(
        ('1', '9'),
        vec![
            vec!['^', '>', '^', '>', 'A'],
            vec!['>', '^', '^', '>', 'A'],
            vec!['^', '^', '>', '>', 'A'],
            vec!['>', '>', '^', '^', 'A'],
            vec!['^', '>', '>', '^', 'A'],
            vec!['>', '^', '>', '^', 'A'],
        ],
    );
    m.insert(('2', 'A'), vec![vec!['>', 'v', 'A']]);
    m.insert(('2', '0'), vec![vec!['v', '>', 'A']]);
    m.insert(('2', '1'), vec![vec!['v', 'A']]);
    m.insert(('2', '2'), vec![vec!['A']]);
    m.insert(('2', '3'), vec![vec!['>', 'A']]);
    m.insert(('2', '4'), vec![vec!['<', '^', 'A'], vec!['^', '<', 'A']]);
    m.insert(('2', '5'), vec![vec!['^', 'A']]);
    m.insert(('2', '6'), vec![vec!['^', '>', 'A'], vec!['>', '^', 'A']]);
    m.insert(
        ('2', '7'),
        vec![
            vec!['^', '^', '<', 'A'],
            vec!['^', '<', '^', 'A'],
            vec!['<', '^', '^', 'A'],
        ],
    );
    m.insert(('2', '8'), vec![vec!['^', '^', 'A']]);
    m.insert(
        ('2', '9'),
        vec![
            vec!['^', '^', '>', 'A'],
            vec!['^', '>', '^', 'A'],
            vec!['>', '^', '^', 'A'],
        ],
    );
    m.insert(('3', 'A'), vec![vec!['v', 'A']]);
    m.insert(
        ('3', '0'),
        vec![vec!['<', 'v', '<', 'A'], vec!['v', '<', 'A']],
    );
    m.insert(('3', '1'), vec![vec!['<', '<', 'A']]);
    m.insert(('3', '2'), vec![vec!['<', 'A']]);
    m.insert(('3', '3'), vec![vec!['A']]);
    m.insert(
        ('3', '4'),
        vec![
            vec!['^', '<', '<', 'A'],
            vec!['<', '^', '<', 'A'],
            vec!['<', '<', '^', 'A'],
        ],
    );
    m.insert(('3', '5'), vec![vec!['^', '<', 'A'], vec!['<', '^', 'A']]);
    m.insert(('3', '6'), vec![vec!['^', 'A']]);
    m.insert(
        ('3', '7'),
        vec![
            vec!['^', '^', '<', '<', 'A'],
            vec!['^', '<', '^', '<', 'A'],
            vec!['<', '^', '^', '<', 'A'],
            vec!['^', '<', '<', '^', 'A'],
            vec!['<', '^', '<', '^', 'A'],
            vec!['<', '<', '^', '^', 'A'],
        ],
    );
    m.insert(
        ('3', '8'),
        vec![
            vec!['^', '^', '<', 'A'],
            vec!['<', '^', '^', 'A'],
            vec!['^', '<', '^', 'A'],
        ],
    );
    m.insert(('3', '9'), vec![vec!['^', '^', 'A']]);
    m.insert(
        ('4', 'A'),
        vec![
            vec!['v', '>', '>', 'v', 'A'],
            vec!['>', 'v', '>', 'v', 'A'],
            vec!['>', '>', 'v', 'v', 'A'],
            vec!['v', '>', 'v', '>', 'A'],
            vec!['>', 'v', 'v', '>', 'A'],
        ],
    );
    m.insert(
        ('4', '0'),
        vec![vec!['>', 'v', 'v', 'A'], vec!['v', '>', 'v', 'A']],
    );
    m.insert(('4', '1'), vec![vec!['v', 'A']]);
    m.insert(('4', '2'), vec![vec!['>', 'v', 'A'], vec!['v', '>', 'A']]);
    m.insert(
        ('4', '3'),
        vec![
            vec!['>', '>', 'v', 'A'],
            vec!['v', '>', '>', 'A'],
            vec!['>', 'v', '>', 'A'],
        ],
    );
    m.insert(('4', '4'), vec![vec!['A']]);
    m.insert(('4', '5'), vec![vec!['>', 'A']]);
    m.insert(('4', '6'), vec![vec!['>', '>', 'A']]);
    m.insert(('4', '7'), vec![vec!['^', 'A']]);
    m.insert(('4', '8'), vec![vec!['^', '>', 'A'], vec!['>', '^', 'A']]);
    m.insert(
        ('4', '9'),
        vec![
            vec!['>', '>', '^', 'A'],
            vec!['^', '>', '>', 'A'],
            vec!['>', '^', '>', 'A'],
        ],
    );
    m.insert(
        ('5', 'A'),
        vec![
            vec!['>', 'v', 'v', 'A'],
            vec!['v', '>', 'v', 'A'],
            vec!['v', 'v', '>', 'A'],
        ],
    );
    m.insert(('5', '0'), vec![vec!['v', 'v', 'A']]);
    m.insert(('5', '1'), vec![vec!['<', 'v', 'A'], vec!['v', '<', 'A']]);
    m.insert(('5', '2'), vec![vec!['v', 'A']]);
    m.insert(('5', '3'), vec![vec!['>', 'v', 'A'], vec!['v', '>', 'A']]);
    m.insert(('5', '4'), vec![vec!['<', 'A']]);
    m.insert(('5', '5'), vec![vec!['A']]);
    m.insert(('5', '6'), vec![vec!['>', 'A']]);
    m.insert(('5', '7'), vec![vec!['^', '<', 'A'], vec!['<', '^', 'A']]);
    m.insert(('5', '8'), vec![vec!['^', 'A']]);
    m.insert(('5', '9'), vec![vec!['>', '^', 'A'], vec!['^', '>', 'A']]);
    m.insert(('6', 'A'), vec![vec!['v', 'v', 'A']]);
    m.insert(
        ('6', '0'),
        vec![
            vec!['v', 'v', '<', 'A'],
            vec!['v', '<', 'v', 'A'],
            vec!['<', 'v', 'v', 'A'],
        ],
    );
    m.insert(
        ('6', '1'),
        vec![
            vec!['<', '<', 'v', 'A'],
            vec!['v', '<', '<', 'A'],
            vec!['<', 'v', '<', 'A'],
        ],
    );
    m.insert(('6', '2'), vec![vec!['<', 'v', 'A'], vec!['v', '<', 'A']]);
    m.insert(('6', '3'), vec![vec!['v', 'A']]);
    m.insert(('6', '4'), vec![vec!['<', '<', 'A']]);
    m.insert(('6', '5'), vec![vec!['<', 'A']]);
    m.insert(('6', '6'), vec![vec!['A']]);
    m.insert(
        ('6', '7'),
        vec![
            vec!['^', '<', '<', 'A'],
            vec!['<', '^', '<', 'A'],
            vec!['<', '<', '^', 'A'],
        ],
    );
    m.insert(('6', '8'), vec![vec!['<', '^', 'A'], vec!['^', '<', 'A']]);
    m.insert(('6', '9'), vec![vec!['^', 'A']]);
    m.insert(
        ('7', 'A'),
        vec![
            vec!['v', 'v', '>', '>', 'v', 'A'],
            vec!['v', '>', 'v', '>', 'v', 'A'],
            vec!['>', 'v', 'v', '>', 'v', 'A'],
            vec!['>', '>', 'v', 'v', 'v', 'A'],
            vec!['v', '>', '>', 'v', 'v', 'A'],
            vec!['>', 'v', '>', 'v', 'v', 'A'],
            vec!['v', 'v', '>', 'v', '>', 'A'],
            vec!['v', '>', 'v', 'v', '>', 'A'],
            vec!['>', 'v', 'v', 'v', '>', 'A'],
        ],
    );
    m.insert(
        ('7', '0'),
        vec![
            vec!['v', 'v', '>', 'v', 'A'],
            vec!['>', 'v', 'v', 'v', 'A'],
            vec!['v', '>', 'v', 'v', 'A'],
        ],
    );
    m.insert(('7', '1'), vec![vec!['v', 'v', 'A']]);
    m.insert(
        ('7', '2'),
        vec![
            vec!['>', 'v', 'v', 'A'],
            vec!['v', '>', 'v', 'A'],
            vec!['v', 'v', '>', 'A'],
        ],
    );
    m.insert(
        ('7', '3'),
        vec![
            vec!['v', 'v', '>', '>', 'A'],
            vec!['v', '>', 'v', '>', 'A'],
            vec!['>', 'v', 'v', '>', 'A'],
            vec!['v', '>', '>', 'v', 'A'],
            vec!['>', 'v', '>', 'v', 'A'],
            vec!['>', '>', 'v', 'v', 'A'],
        ],
    );
    m.insert(('7', '4'), vec![vec!['v', 'A']]);
    m.insert(('7', '5'), vec![vec!['v', '>', 'A'], vec!['>', 'v', 'A']]);
    m.insert(
        ('7', '6'),
        vec![
            vec!['>', '>', 'v', 'A'],
            vec!['>', 'v', '>', 'A'],
            vec!['v', '>', '>', 'A'],
        ],
    );
    m.insert(('7', '7'), vec![vec!['A']]);
    m.insert(('7', '8'), vec![vec!['>', 'A']]);
    m.insert(('7', '9'), vec![vec!['>', '>', 'A']]);
    m.insert(
        ('8', 'A'),
        vec![
            vec!['v', 'v', 'v', '>', 'A'],
            vec!['>', 'v', 'v', 'v', 'A'],
            vec!['v', '>', 'v', 'v', 'A'],
            vec!['v', 'v', '>', 'v', 'A'],
        ],
    );
    m.insert(('8', '0'), vec![vec!['v', 'v', 'v', 'A']]);
    m.insert(
        ('8', '1'),
        vec![
            vec!['v', '<', 'v', 'A'],
            vec!['<', 'v', 'v', 'A'],
            vec!['v', 'v', '<', 'A'],
        ],
    );
    m.insert(('8', '2'), vec![vec!['v', 'v', 'A']]);
    m.insert(
        ('8', '3'),
        vec![
            vec!['v', 'v', '>', 'A'],
            vec!['>', 'v', 'A'],
            vec!['v', '>', 'v', 'A'],
        ],
    );
    m.insert(('8', '4'), vec![vec!['v', '<', 'A'], vec!['<', 'v', 'A']]);
    m.insert(('8', '5'), vec![vec!['v', 'A']]);
    m.insert(('8', '6'), vec![vec!['v', '>', 'A'], vec!['>', 'v', 'A']]);
    m.insert(('8', '7'), vec![vec!['<', 'A']]);
    m.insert(('8', '8'), vec![vec!['A']]);
    m.insert(('8', '9'), vec![vec!['>', 'A']]);
    m.insert(('9', 'A'), vec![vec!['v', 'v', 'v', 'A']]);
    m.insert(
        ('9', '0'),
        vec![
            vec!['<', 'v', 'v', 'v', 'A'],
            vec!['v', '<', 'v', 'v', 'A'],
            vec!['v', 'v', '<', 'v', 'A'],
            vec!['v', 'v', 'v', '<', 'A'],
        ],
    );
    m.insert(
        ('9', '1'),
        vec![
            vec!['<', '<', 'v', 'v', 'A'],
            vec!['<', 'v', '<', 'v', 'A'],
            vec!['v', '<', '<', 'v', 'A'],
            vec!['v', 'v', '<', '<', 'A'],
            vec!['<', 'v', 'v', '<', 'A'],
            vec!['v', '<', 'v', '<', 'A'],
        ],
    );
    m.insert(
        ('9', '2'),
        vec![
            vec!['<', 'v', 'v', 'A'],
            vec!['v', '<', 'v', 'A'],
            vec!['v', 'v', '<', 'A'],
        ],
    );
    m.insert(('9', '3'), vec![vec!['v', 'v', 'A']]);
    m.insert(
        ('9', '4'),
        vec![
            vec!['<', '<', 'v', 'A'],
            vec!['<', 'v', '<', 'A'],
            vec!['v', '<', '<', 'A'],
        ],
    );
    m.insert(('9', '5'), vec![vec!['<', 'v', 'A'], vec!['v', '<', 'A']]);
    m.insert(('9', '6'), vec![vec!['v', 'A']]);
    m.insert(('9', '7'), vec![vec!['<', '<', 'A']]);
    m.insert(('9', '8'), vec![vec!['<', 'A']]);
    m.insert(('9', '9'), vec![vec!['A']]);

    // directional keypad
    m.insert(
        ('A', '<'),
        vec![vec!['v', '<', '<', 'A'], vec!['<', 'v', '<', 'A']],
    );
    m.insert(('A', 'v'), vec![vec!['v', '<', 'A'], vec!['<', 'v', 'A']]);
    m.insert(('A', '>'), vec![vec!['v', 'A']]);
    m.insert(('A', '^'), vec![vec!['<', 'A']]);
    m.insert(
        ('<', 'A'),
        vec![vec!['>', '>', '^', 'A'], vec!['>', '^', '>', 'A']],
    );
    m.insert(('<', '<'), vec![vec!['A']]);
    m.insert(('<', 'v'), vec![vec!['>', 'A']]);
    m.insert(('<', '>'), vec![vec!['>', '>', 'A']]);
    m.insert(('<', '^'), vec![vec!['>', '^', 'A']]);
    m.insert(('v', 'A'), vec![vec!['^', '>', 'A'], vec!['>', '^', 'A']]);
    m.insert(('v', '<'), vec![vec!['<', 'A']]);
    m.insert(('v', 'v'), vec![vec!['A']]);
    m.insert(('v', '>'), vec![vec!['>', 'A']]);
    m.insert(('v', '^'), vec![vec!['^', 'A']]);
    m.insert(('>', 'A'), vec![vec!['^', 'A']]);
    m.insert(('>', '<'), vec![vec!['<', '<', 'A']]);
    m.insert(('>', 'v'), vec![vec!['<', 'A']]);
    m.insert(('>', '>'), vec![vec!['A']]);
    m.insert(('>', '^'), vec![vec!['^', '<', 'A'], vec!['<', '^', 'A']]);
    m.insert(('^', 'A'), vec![vec!['>', 'A']]);
    m.insert(('^', '<'), vec![vec!['v', '<', 'A'], vec!['<', 'v', 'A']]);
    m.insert(('^', 'v'), vec![vec!['v', 'A']]);
    m.insert(('^', '>'), vec![vec!['>', 'v', 'A'], vec!['v', '>', 'A']]);
    m.insert(('^', '^'), vec![vec!['A']]);

    Mutex::new(m)
});
