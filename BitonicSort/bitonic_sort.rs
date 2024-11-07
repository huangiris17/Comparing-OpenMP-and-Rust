use rayon::prelude::*;
use std::env;
use std::time::Instant;

const UP: i32 = -1;
const DOWN: i32 = 1;
const PARALLEL_THRESHOLD: usize = 16384;

fn main() {
    // Get array size from command line
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Please insert array size.");
        return;
    }

    let array_size: usize = args[1].parse().expect("Invalid number");

    if !is_power_of_two(array_size) {
        eprintln!("Please insert array size that is a power of two.");
        return;
    }

    // Generate array with random numbers
    let mut arr: Vec<i32> = (0..array_size).map(|_| rand::random::<i32>() % 30).collect();
    //print_array(&arr);
    let mut arr_p = arr.clone();

    // Apply bitonic sort
    let start = Instant::now();
    bitonic_sort(&mut arr, array_size, UP);
    let duration = start.elapsed();
    //print_array(&arr);
    println!("Execution time: {:.6} seconds", duration.as_secs_f64());

    // Apply parallelized bitonic sort
    let start_p = Instant::now();
    bitonic_sort_p(&mut arr_p, array_size, UP);
    let duration_p = start_p.elapsed();
    //print_array(&arr_p);
    println!("Execution time using parallelism: {:.6} seconds", duration_p.as_secs_f64());
    println!("Speedup: {:.6}", duration.as_secs_f64() / duration_p.as_secs_f64());
}

// Function to print the array
fn print_array(arr: &[i32]) {
    for &item in arr.iter() {
        print!("{} ", item);
    }
    println!();
}

// Check if a number is a power of two
fn is_power_of_two(n: usize) -> bool {
    n > 0 && (n & (n - 1)) == 0
}

// Bitonic sort
fn bitonic_sort(arr: &mut [i32],cnt: usize, dir: i32) {
    if cnt <= 1 {
        return;
    }

    let size = cnt / 2;
    let (first, second) = arr.split_at_mut(size);
    bitonic_sort(first, size, UP);
    bitonic_sort(second, size, DOWN);
    bitonic_merge(arr, cnt, dir);
}

// Bitonic merge
fn bitonic_merge(arr: &mut [i32], cnt: usize, dir: i32) {
    if cnt <= 1 {
        return;
    }

    let size = cnt / 2;
    for i in 0..size {
        if (arr[i] > arr[i + size]) == (dir == UP) {
            arr.swap(i, i + size);
        }
    }

    let (first, second) = arr.split_at_mut(size);
    bitonic_merge(first, size, dir);
    bitonic_merge(second, size, dir);
}

// Parallel bitonic sort
fn bitonic_sort_p(arr: &mut [i32], cnt: usize, dir: i32) {
    if cnt <= PARALLEL_THRESHOLD {
        bitonic_sort(arr, cnt, dir);
        return;
    }

    let size = cnt / 2;
    let (first, second) = arr.split_at_mut(size);

    rayon::join(
        || bitonic_sort_p(first, size, UP),
        || bitonic_sort_p(second, size, DOWN),
    );

    bitonic_merge_p(arr, cnt, dir);
}

// Parallel bitonic merge
fn bitonic_merge_p(arr: &mut [i32], cnt: usize, dir: i32) {
    if cnt <= PARALLEL_THRESHOLD {
        bitonic_merge(arr, cnt, dir);
        return;
    }

    let size = cnt / 2;

    let swap_indices: Vec<(usize, usize)> = arr.par_iter()
        .enumerate()
        .take(size)
        .filter_map(|(i, _)| {
            let idx1 = i;
            let idx2 = i + size;
            if (arr[idx1] > arr[idx2]) == (dir == UP) {
                Some((idx1, idx2))
            } else {
                None
            }
        }).collect();

    // Perform the swaps outside the initial borrow
    for (idx1, idx2) in swap_indices {
        arr.swap(idx1, idx2);
    }

    let (first, second) = arr.split_at_mut(size);
    rayon::join(
        || bitonic_merge_p(first, size, dir),
        || bitonic_merge_p(second, size, dir),
    );
}

