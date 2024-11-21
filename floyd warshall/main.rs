use rayon::prelude::*;
use std::env;
use std::fs::File;
use std::io::{BufReader, BufRead};
use std::process;
use std::time::Instant;

const INF: i32 = 1_000_000_000;

fn floyd_warshall(n: usize, dist: &mut Vec<i32>, num_threads: usize) {
    rayon::ThreadPoolBuilder::new()
        .num_threads(num_threads)
        .build_global()
        .unwrap();

    for k in 0..n {
        let k_row = dist[k * n..(k + 1) * n].to_vec();

        dist.par_chunks_mut(n)
            .enumerate()
            .for_each(|(_, row)| {
                for j in 0..n {
                    if k_row[j] != INF && row[k] != INF {
                        let via_k = row[k] + k_row[j];
                        if via_k < row[j] {
                            row[j] = via_k;
                        }
                    }
                }
            });
    }
}

fn read_graph(filename: &str) -> (usize, usize, Vec<(usize, usize, i32)>) {
    let file = File::open(filename).unwrap();
    let reader = BufReader::new(file);
    let mut lines = reader.lines();

    let first_line = lines.next().unwrap().unwrap();
    let mut parts = first_line.split_whitespace();
    let n: usize = parts.next().unwrap().parse().unwrap();
    let m: usize = parts.next().unwrap().parse().unwrap();

    let mut edges = Vec::with_capacity(m);
    for _ in 0..m {
        let line = lines.next().unwrap().unwrap();
        let mut parts = line.split_whitespace();
        let u: usize = parts.next().unwrap().parse().unwrap();
        let v: usize = parts.next().unwrap().parse().unwrap();
        let w: i32 = parts.next().unwrap().parse().unwrap();
        edges.push((u - 1, v - 1, w));
    }

    (n, m, edges)
}

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() != 4 || args[1] != "-f" {
        eprintln!("Usage: {} -f <filename> <num_threads>", args[0]);
        process::exit(1);
    }

    let filename = &args[2];
    let num_threads: usize = args[3].parse().unwrap();
    if num_threads == 0 {
        eprintln!("Number of threads must be positive.");
        process::exit(1);
    }

    let (n, _m, edges) = read_graph(filename);

    let mut dist = vec![INF; n * n];
    for i in 0..n {
        dist[i * n + i] = 0;
    }

    for (u, v, w) in edges {
        if w < dist[u * n + v] {
            dist[u * n + v] = w;
        }
    }

    let start_time = Instant::now();
    floyd_warshall(n, &mut dist, num_threads);
    let duration = start_time.elapsed();

    println!("Execution Time (s): {:.6}", duration.as_secs_f64());

    // println!("Shortest distance matrix:");
    // for i in 0..n {
    //     for j in 0..n {
    //         if dist[i * n + j] == INF {
    //             print!("INF ");
    //         } else {
    //             print!("{} ", dist[i * n + j]);
    //         }
    //     }
    //     println!();
    // }
}
