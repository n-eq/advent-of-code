use itertools::Itertools;
use std::collections::{HashMap, HashSet};

type Node<'a> = &'a str;

// Slight modifications to adapt the function that was intended at String types as nodes (not &str)
fn bron_kerbosch_v2<'a>(
    r: &HashSet<Node<'a>>,
    p: &mut HashSet<Node<'a>>,
    x: &mut HashSet<Node<'a>>,
    g: &HashMap<Node<'a>, HashSet<Node<'a>>>,
    cliques: &mut Vec<Vec<Node<'a>>>,
) {
    if p.is_empty() && x.is_empty() {
        if r.len() > 2 {
            let mut clique: Vec<Node> = r.iter().cloned().collect();
            clique.sort();
            cliques.push(clique);
        }
        return;
    }

    let pivot = p
        .union(x)
        .max_by_key(|v| g.get(*v).map_or(0, |neighbors| neighbors.len()))
        .cloned();

    if let Some(pivot_vertex) = pivot {
        let neighbors = g.get(&pivot_vertex).cloned().unwrap_or_default();
        let candidates: Vec<Node> = p.difference(&neighbors).cloned().collect();

        for v in candidates {
            let mut new_r = r.clone();
            new_r.insert(v);

            let neighbors_v = g.get(&v).cloned().unwrap_or_default();
            let mut new_p = p
                .intersection(&neighbors_v)
                .cloned()
                .collect::<HashSet<Node>>();

            let mut new_x = x
                .intersection(&neighbors_v)
                .cloned()
                .collect::<HashSet<Node>>();

            bron_kerbosch_v2(&new_r, &mut new_p, &mut new_x, g, cliques);

            p.remove(&v);
            x.insert(v);
        }
    }
}

pub fn main() {
    let binding = std::fs::read_to_string(crate::utils::input!()).unwrap();

    let mut graph: HashMap<Node, HashSet<Node>> = HashMap::new();
    for l in binding.lines() {
        let s = l.split('-').collect::<Vec<_>>();
        graph.entry(s[0]).or_insert_with(HashSet::new).insert(s[1]);
        graph.entry(s[1]).or_insert_with(HashSet::new).insert(s[0]);
    }

    let part1 = graph
        .keys()
        .combinations(3)
        .filter(|combination| {
            let (p0, p1, p2) = (combination[0], combination[1], combination[2]);
            graph
                .get(p0)
                .map_or_else(|| false, |v| v.contains(p1) && v.contains(p2))
                && graph
                    .get(p1)
                    .map_or_else(|| false, |v| v.contains(p0) && v.contains(p2))
                && graph
                    .get(p2)
                    .map_or_else(|| false, |v| v.contains(p0) && v.contains(p1))
                && combination.iter().any(|p| p.starts_with('t'))
        })
        .collect::<Vec<_>>()
        .len();

    println!("{part1:?}");

    // The code below and bron_kerbosch_v2 function were copied from Rosetta code
    // https://web.archive.org/web/20250108120245/https://rosettacode.org/wiki/Bron%E2%80%93Kerbosch_algorithm#Rust
    // to find the maximum clique in our graph (a classic graph theory problem)
    let r: HashSet<Node> = HashSet::new();
    let mut p: HashSet<Node> = graph.keys().cloned().collect();
    let mut x: HashSet<Node> = HashSet::new();

    let mut cliques: Vec<Vec<Node>> = Vec::new();
    bron_kerbosch_v2(&r, &mut p, &mut x, &graph, &mut cliques);
    cliques.sort_by(|c1, c2| c2.len().cmp(&c1.len()));

    let part2 = &mut cliques[0];
    part2.sort();
    let part2 = part2.join(",");
    println!("{part2:?}");
}
