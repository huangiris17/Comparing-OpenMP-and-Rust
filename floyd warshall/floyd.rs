use std::io::{self, BufRead};
use rayon::prelude::*;

const INF: i32 = 1_000_000_000;

fn floyd_warshall(n: usize, dist: &mut Vec<Vec<i32>>) {
    for k in 0..n {
        let dist_k = dist[k].clone();
        dist.par_iter_mut().for_each(|dist_i| {
            for j in 0..n {
                if dist_i[k] != INF && dist_k[j] != INF {
                    let new_dist = dist_i[k] + dist_k[j];
                    if new_dist < dist_i[j] {
                        dist_i[j] = new_dist;
                    }
                }
            }
        });
    }
}

fn main() {
    println!("Enter the number of vertices and edges: ");
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();

    let first_line = match lines.next() {
        Some(Ok(line)) => line,
        _ => {
            eprintln!("Error reading the number of vertices and edges.");
            return;
        }
    };

    let mut tokens = first_line.trim().split_whitespace();
    let n: usize = match tokens.next().and_then(|s| s.parse().ok()) {
        Some(num) if num > 0 => num,
        _ => {
            eprintln!("Number of vertices must be positive.");
            return;
        }
    };
    let m: usize = match tokens.next().and_then(|s| s.parse().ok()) {
        Some(num) => num,
        _ => {
            eprintln!("Error reading the number of edges.");
            return;
        }
    };

    let mut dist = vec![vec![INF; n]; n];
    for i in 0..n {
        dist[i][i] = 0;
    }

    println!("Enter each edge and its weight (format: u v w):");
    for i in 0..m {
        let line = match lines.next() {
            Some(Ok(l)) => l,
            _ => {
                eprintln!("Error reading edge {}.", i + 1);
                return;
            }
        };
        let mut tokens = line.trim().split_whitespace();
        let u: usize = match tokens.next().and_then(|s| s.parse().ok()) {
            Some(num) => num,
            _ => {
                eprintln!("Error parsing 'u' in edge {}.", i + 1);
                return;
            }
        };
        let v: usize = match tokens.next().and_then(|s| s.parse().ok()) {
            Some(num) => num,
            _ => {
                eprintln!("Error parsing 'v' in edge {}.", i + 1);
                return;
            }
        };
        let w: i32 = match tokens.next().and_then(|s| s.parse().ok()) {
            Some(num) => num,
            _ => {
                eprintln!("Error parsing 'w' in edge {}.", i + 1);
                return;
            }
        };

        let u = u - 1;
        let v = v - 1;

        if u >= n || v >= n {
            eprintln!(
                "Invalid vertex index in edge {}: u={}, v={}",
                i + 1,
                u + 1,
                v + 1
            );
            return;
        }

        dist[u][v] = w;
    }

    floyd_warshall(n, &mut dist);

    println!("Shortest distance matrix:");
    for i in 0..n {
        for j in 0..n {
            if dist[i][j] == INF {
                print!("INF ");
            } else {
                print!("{} ", dist[i][j]);
            }
        }
        println!();
    }
}
