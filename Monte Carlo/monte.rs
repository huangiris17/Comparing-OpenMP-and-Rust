use rand::Rng;
use rand::SeedableRng;
use rand::rngs::StdRng;
use std::thread;

fn main() {
    let num_trials: u64 = 100_000_000;
    let num_threads = 4;

    let trials_per_thread = num_trials / num_threads;
    let leftover_trials = num_trials % num_threads;

    let mut handles = vec![];

    for thread_id in 0..num_threads {
        let trials = if thread_id == num_threads - 1 {
            trials_per_thread + leftover_trials
        } else {
            trials_per_thread
        };

        handles.push(thread::spawn(move || {
            let mut local_count = 0u64;
            let mut rng = StdRng::seed_from_u64(thread_id as u64);

            for _ in 0..trials {
                let x: f64 = rng.gen();
                let y: f64 = rng.gen();

                if x * x + y * y <= 1.0 {
                    local_count += 1;
                }
            }
            local_count
        }));
    }

    let total_count: u64 = handles.into_iter()
        .map(|handle| handle.join().unwrap())
        .sum();

    let pi_estimate = 4.0 * (total_count as f64) / (num_trials as f64);

    println!("Estimated Pi = {:.6}", pi_estimate);
}
