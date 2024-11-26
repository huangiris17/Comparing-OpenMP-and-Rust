use rand::Rng;
use rand::SeedableRng;
use rand::rngs::StdRng;
use rayon::prelude::*;
use std::time::{Instant, SystemTime, UNIX_EPOCH};
use std::env;
use rayon::ThreadPoolBuilder;
use std::thread;
use std::hash::{Hash, Hasher};
use std::collections::hash_map::DefaultHasher;

fn main() {
    let args: Vec<String> = env::args().collect();
    let num_threads: usize = if args.len() > 1 {
        match args[1].parse() {
            Ok(n) if n > 0 => n,
            _ => {
                eprintln!(
                    "Invalid number of threads specified. Using default: {}.",
                    rayon::current_num_threads()
                );
                rayon::current_num_threads()
            }
        }
    } else {
        rayon::current_num_threads()
    };

    let num_trials: usize = if args.len() > 2 {
        match args[2].parse::<usize>() {
            Ok(n) if n > 0 => n,
            _ => {
                eprintln!("Invalid number of trials specified. Using default (1_000_000_000).");
                1_000_000_000
            }
        }
    } else {
        1_000_000_000
    };

    let pool = match ThreadPoolBuilder::new().num_threads(num_threads).build() {
        Ok(p) => p,
        Err(e) => {
            eprintln!("Failed to build Rayon thread pool: {}. Using default.", e);
            rayon::ThreadPoolBuilder::new().build().unwrap()
        }
    };

    let base_seed = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .expect("Time went backwards")
        .as_nanos();

    let start_time = Instant::now();

    let total_count: usize = pool.install(|| {
        (0..num_trials)
            .into_par_iter()
            .chunks(10_000)
            .map_init(
                || {
                    let thread_id = thread::current().id();
                    let mut hasher = DefaultHasher::new();
                    thread_id.hash(&mut hasher);
                    let seed = base_seed.wrapping_add(hasher.finish() as u128);
                    StdRng::seed_from_u64(seed as u64)
                },
                |rng, chunk| {
                    let mut count = 0usize;
                    for _ in chunk {
                        let x: f64 = rng.gen::<f64>();
                        let y: f64 = rng.gen::<f64>();

                        if x * x + y * y <= 1.0 {
                            count += 1;
                        }
                    }
                    count
                },
            )
            .sum()
    });

    let pi_estimate = 4.0 * (total_count as f64) / (num_trials as f64);

    let elapsed = start_time.elapsed();

    println!("Estimated Pi = {:.6}", pi_estimate);
    println!(
        "Threads: {}, Elapsed time: {:.4} seconds",
        num_threads, elapsed.as_secs_f64()
    );
}
