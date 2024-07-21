// compile & run: cargo r
use std::collections::{HashMap, HashSet};

const WIDTH: usize = 114;

// use with input_test
// const WIDTH: usize = 8;

/*
 * I lost way too much on this function that is at the core of building the DAG (forcing
 * me to reconsider and rewrite my dijkstra implementation which was correct...)
 * TL;DR: Rust handles very poorly char arithmetic, yielding 'weird' results on a - b
 * when b is greater than a (after casting to u32)
 */
fn can_move(from: &char, to: &char) -> bool {
    if (*from as u32).checked_sub(*to as u32).is_none() {
        // our destination node has higher elevation, we verify
        // this elevation doesn't exceed 1
        return (*to as u32).checked_sub(*from as u32).unwrap() <= 1;
    }
    true
}

// returns the neighbor's indices heights[idx] is allowed to move to
fn allowed_neighbors(heights: &Vec<&char>, idx: usize) -> Vec<usize> {
    [idx - 1, idx + 1, idx - WIDTH, idx + WIDTH]
        .iter()
        .map(|i| *i)
        .filter(|i| heights.get(*i).is_some())
        .filter(|i| can_move(heights[idx], heights[*i]))
        .collect::<Vec<usize>>()
}

struct DAG {
    /* vertex (indices in the input vector), edges (*/
    graph: HashMap<usize, Vec<usize>>,
}

impl DAG {
    fn new() -> Self {
        Self {
            graph: HashMap::new(),
        }
    }

    fn from_heightmap(heightmap: &Vec<&char>) -> Self {
        let mut dag = DAG::new();
        for i in 0..heightmap.len() {
            for n in allowed_neighbors(&heightmap, i) {
                dag.add(i, n);
            }
        }
        dag
    }

    fn add(self: &mut Self, vertex: usize, edge: usize) {
        if let Some(v) = self.graph.get_mut(&vertex) {
            v.push(edge);
        } else {
            self.graph.insert(vertex, vec![edge]);
        }
    }

    // too lazy to implement Display
    fn display(&self) {
        for vertex in self.graph.keys() {
            println!("{} -> {:?}", vertex, self.graph.get(vertex).unwrap());
        }
    }

    /* Get the distance and path from source to dest node */
    fn dijkstra(self, source: usize, dest: usize) -> usize {
        let mut dist: HashMap<usize, usize> = HashMap::new();
        let mut queue: HashSet<usize> = HashSet::new();

        for vertex in self.graph.keys() {
            dist.insert(*vertex, usize::MAX);
            let _ = queue.insert(*vertex);
        }

        dist.insert(source, 0);
        while !queue.is_empty() {
            // closest element (with least distance) still in the queue
            let mut closest = 0usize;
            let mut tmp = usize::MAX;
            for k in dist.keys() {
                if queue.contains(k) {
                    if dist.get(k).unwrap() <= &tmp {
                        tmp = *dist.get(k).unwrap();
                        closest = *k;
                    }
                }
            }

            if !queue.remove(&closest) {
                panic!(
                    "shoultrying to remove an item that doesn't exist! {:?}, {:?}",
                    closest, queue
                );
            }

            if closest == dest {
                break;
            }

            /*
             * for each node that is reachable from the current vertex (closest),
             * if it's still in the queue, we check its current distance.
             * If the current vertex (closest)'s distance + 1 (elevation)
             * is lower than its current distance, we update it with this new value
             */
            for v in self.graph.get(&closest) {
                for vertex in v {
                    if queue.contains(vertex) {
                        let tmp = dist[&closest] + 1;
                        if tmp < dist[vertex] {
                            dist.insert(*vertex, tmp);
                        }
                    }
                }
            }
        }

        *dist.get(&dest).unwrap()
    }
}

fn main() {
    let input = include_str!("input");
    // let input = include_str!("input_test");

    let lines: Vec<&str> = input.split("\n").collect();

    let input = lines
        .iter()
        .map(|l| l.chars().collect::<Vec<char>>())
        .collect::<Vec<Vec<char>>>();

    let mut heightmap = input.iter().flatten().collect::<Vec<&char>>();

    let start_pos = heightmap.iter().position(|&r| r == &'S').unwrap();
    let end_pos = heightmap.iter().position(|&r| r == &'E').unwrap();
    println!("start end: {} {}", start_pos, end_pos);

    heightmap[start_pos] = &'a';
    heightmap[end_pos] = &'z';
    // println!("heightmap: {:?}", heightmap);

    let dag = DAG::from_heightmap(&heightmap);
    // println!("dag: {:?}", dag.display());
    println!("part1: {}", dag.dijkstra(start_pos, end_pos));

    let mut distances: Vec<usize> = vec![];
    for i in 0..heightmap.clone().len() {
        if *heightmap[i] == 'a' {
            heightmap[i] = &'a';

            let mut dag = DAG::new();
            for j in 0..heightmap.len() {
                for n in allowed_neighbors(&heightmap, j) {
                    dag.add(j, n);
                }
            }
            distances.push(dag.dijkstra(i, end_pos));
        }
    }
    println!("part2: {:?}", distances.iter().min().unwrap());
}
