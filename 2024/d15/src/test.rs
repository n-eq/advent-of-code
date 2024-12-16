#[cfg(test)]
mod tests {
    use crate::*;

    #[test]
    fn test_example_input() {
        let input = "########
#..O.O.#
##@.O..#
#...O..#
#.#.O..#
#...O..#
#......#
########

<^^>>>vv<v>>v<<"
            .to_string();
        let mut map = Problem::new(input.clone());
        let s = map.solve();
        assert_eq!(s, 2028);
    }

    #[test]
    fn test_large_example() {
        let mut map = Problem::new(std::fs::read_to_string("input_test").unwrap());
        assert_eq!(map.solve(), 10092);
        let mut map = Problem::new(std::fs::read_to_string("input_test").unwrap());
        map.extend();
        assert_eq!(map.solve(), 9021);
    }

    #[test]
    fn test_move() {
        let mut map = Problem::new(
            "####################
##[].......[].[][]##
##[]..[]...@...[].##
##[].[].....[][][]##
##[]..@......[].[]##
##..##....[][]....##
##..[]....@.@.....##
##..@......[]@[][]##
##......[][]..[]..##
####################"
                .to_string(),
        );
        assert_eq!(false, map.try_move((7, 4), &Move::Up, false));
        assert_eq!(true, map.try_move((4, 6), &Move::Up, false));
        assert_eq!(false, map.try_move((2, 12), &Move::Up, false));

        assert_eq!(true, map.try_move((6, 10), &Move::Up, false));
        assert_eq!(false, map.try_move((6, 12), &Move::Up, false));
    }
}
