use std::collections::HashMap;

type ValveName = [char; 2];

#[derive(Hash, Eq, PartialEq, Clone, Debug, Default)]
pub struct Valve {
    pub id: usize,
    pub name: ValveName,
    pub flow_rate: u32,
    pub links: Vec<ValveName>,
    /* None if still closed, Some(t) where t is the timestamp where it was opened */
    pub opened: Option<u32>,
}

#[derive(Debug, Clone, Eq, Hash, PartialEq)]
pub struct State {
    current_valve_human: usize,
    current_valve_elephant: usize,
    time_left_human: u32,
    time_left_elephant: u32,
    open_valves: Vec<usize>,
}

// TODO: group fields to use State
#[derive(Debug, Clone, Eq, Hash, PartialEq)]
pub struct Solution {
    valves: Vec<Valve>,
    current: [usize; 2],
    dist: Vec<Vec<u32>>,
    time_left: [u32; 2],
    pressure_released: u32,
}

impl Solution {
    // Floyd-Warshall
    fn get_dist(&mut self) {
        self.dist = Vec::with_capacity(self.valves.len());
        for i in 0..self.valves.len() {
            self.dist.push(vec![1000; self.valves.len()]);
            self.dist[i][i] = 0;
        }
        for v in self.valves.clone() {
            for w in v.links {
                let id = self.valves.iter().find(|v| v.name == w).unwrap().id;
                self.dist[v.id][id] = 1;
                self.dist[id][v.id] = 1;
            }
        }

        for v in self.valves.clone() {
            for w in self.valves.clone() {
                for x in self.valves.clone() {
                    self.dist[w.id][x.id] = u32::min(
                        self.dist[w.id][x.id],
                        self.dist[w.id][v.id] + self.dist[v.id][x.id],
                    );
                }
            }
        }
    }

    // DFS using self.distances
    fn solve(self, memo: &mut HashMap<State, u32>) -> u32 {
        let state = State {
            current_valve_human: self.current[0],
            time_left_human: self.time_left[0],
            current_valve_elephant: self.current[1],
            time_left_elephant: self.time_left[1],
            open_valves: self
                .valves
                .iter()
                .filter(|v| v.opened.is_some())
                .map(|v| v.id)
                .collect(),
        };
        if let Some(val) = memo.get(&state) {
            return *val;
        }

        let mut best_pressure = self.pressure_released;
        for v in self
            .valves
            .iter()
            .filter(|v| v.flow_rate > 0 && v.opened.is_none())
        {
            let travel_time_human = self.clone().dist[self.current[0]][v.id];
            let travel_time_elephant = self.clone().dist[self.current[1]][v.id];

            // if the valve "v" is reachable (and openable) from the current valve before time runs out
            if let Some(new_time_left) = self.time_left[0].checked_sub(travel_time_human + 1) {
                let new_pressure = self.pressure_released + (v.flow_rate * new_time_left);
                let mut new_valves = self.valves.clone();
                let vid = new_valves
                    .iter()
                    .position(|valve| v.id == valve.id)
                    .unwrap();
                new_valves[vid].opened = Some(new_time_left);
                best_pressure = u32::max(
                    best_pressure,
                    Solution {
                        current: [vid, self.current[1]],
                        valves: new_valves,
                        dist: self.dist.clone(),
                        time_left: [new_time_left, self.time_left[1]],
                        pressure_released: new_pressure,
                    }
                    .solve(memo),
                );
            }

            if let Some(new_time_left) = self.time_left[1].checked_sub(travel_time_elephant + 1) {
                let new_pressure = self.pressure_released + (v.flow_rate * new_time_left);
                let mut new_valves = self.valves.clone();

                let vid = new_valves
                    .iter()
                    .position(|valve| v.id == valve.id)
                    .unwrap();
                new_valves[vid].opened = Some(new_time_left);

                best_pressure = u32::max(
                    best_pressure,
                    Solution {
                        current: [self.current[0], vid],
                        valves: new_valves,
                        dist: self.dist.clone(),
                        time_left: [self.time_left[0], new_time_left],
                        pressure_released: new_pressure,
                    }
                    .solve(memo),
                );
            }
        }

        memo.insert(state, best_pressure);
        best_pressure
    }
}

fn parse_input() -> Vec<Valve> {
    include_str!("../input")
        .lines()
        .enumerate()
        .map(|(i, line)| {
            let (left, right) = line.split_once(";").expect("Wrong input");

            let flow_rate = left.split_once("=").unwrap().1.parse::<u32>().unwrap();
            let links: Vec<ValveName> = right
                .split(" ")
                .skip(5)
                .map(|n| {
                    let mut name = n.get(..2).unwrap().chars();
                    [name.next().unwrap(), name.next().unwrap()]
                })
                .collect();

            let mut valve_name = left.trim().strip_prefix("Valve ").unwrap().chars();
            Valve {
                id: i,
                flow_rate,
                name: [valve_name.next().unwrap(), valve_name.next().unwrap()],
                links,
                opened: None,
            }
        })
        .collect()
}

fn main() {
    let valves = parse_input();

    let max_time = 30u32;
    let start_valve = valves.iter().find(|v| v.name == ['A', 'A']).unwrap().id;

    let mut solution = Solution {
        valves,
        current: [start_valve, start_valve],
        time_left: [max_time, 0],
        pressure_released: 0,
        dist: vec![],
    };
    solution.get_dist();

    let mut memo = HashMap::new();
    let part1 = solution.clone().solve(&mut memo);
    println!("part1: {part1:?}");

    solution.current = [start_valve, start_valve];
    solution.time_left = [max_time - 4, max_time - 4];
    solution.pressure_released = 0;
    let mut memo = HashMap::new();
    let part2 = solution.solve(&mut memo);
    println!("part2: {part2:?}");
}
