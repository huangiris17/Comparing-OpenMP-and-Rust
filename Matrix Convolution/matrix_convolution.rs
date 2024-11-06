use rand::Rng;
use rayon::prelude::*;
use std::time::Instant;

const FILTER_SIZE: usize = 3;

fn main() {
    // Get matrix size from command line
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        eprintln!("Please insert matrix size.");
        return;
    }

    let matrix_size: usize = args[1].parse().expect("Invalid matrix size");

    // Allocate memory for input matrix
    let mut input_matrix = create_matrix(matrix_size);
    let output_size = matrix_size - FILTER_SIZE + 1;
    let mut output_matrix = create_matrix(output_size);

    // Fill the matrix with random numbers
    let mut rng = rand::thread_rng();
    for row in input_matrix.iter_mut() {
        for elem in row.iter_mut() {
            *elem = rng.gen_range(0..10);
        }
    }

    //println!("Input Matrix:");
    //print_matrix(&input_matrix);
    
    // Define the filter matrix
    let filter: [[i32; FILTER_SIZE]; FILTER_SIZE] = [
        [0, -1, 0],
        [-1, 5, -1],
        [0, -1, 0],
    ];

    // Apply matrix convolution (single-threaded)
    let padding = 1;
    let start = Instant::now();
    convolution(&input_matrix, &filter, &mut output_matrix, matrix_size, padding);
    let duration = start.elapsed();
    println!("Execution time (single-threaded): {:?}", duration);

    //println!("Output Matrix (Sequential):");
    //print_matrix(&output_matrix);

    // Apply matrix convolution (parallelized)
    let start_p = Instant::now();
    convolution_parallel(&input_matrix, &filter, &mut output_matrix, matrix_size, padding);
    let duration_p = start_p.elapsed();
    println!("Execution time (parallelized): {:?}", duration_p);

    //println!("Output Matrix (Parallel):");
    //print_matrix(&output_matrix);
}

fn print_matrix(matrix: &Vec<Vec<i32>>) {
    for row in matrix {
        for &value in row {
            print!("{:3} ", value);
        }
        println!();
    }
    println!();
}

fn create_matrix(size: usize) -> Vec<Vec<i32>> {
    vec![vec![0; size]; size]
}

fn convolution(
    input: &Vec<Vec<i32>>,
    filter: &[[i32; FILTER_SIZE]; FILTER_SIZE],
    output: &mut Vec<Vec<i32>>,
    size: usize,
    padding: usize,
) {
    for i in padding..size - padding {
        for j in padding..size - padding {
            let mut sum = 0;
            for fi in 0..FILTER_SIZE {
                for fj in 0..FILTER_SIZE {
                    sum += input[i + fi - 1][j + fj - 1] * filter[fi][fj];
                }
            }
            output[i - padding][j - padding] = sum;
        }
    }
}

fn convolution_parallel(
    input: &Vec<Vec<i32>>,
    filter: &[[i32; FILTER_SIZE]; FILTER_SIZE],
    output: &mut Vec<Vec<i32>>,
    size: usize,
    padding: usize,
) {
    output.par_iter_mut().enumerate().for_each(|(i, row)| {
        if i >= padding && i < size - padding {
            for j in 0..row.len() {
                if j >= padding && j < size - padding {
                    let mut sum = 0;
                    for fi in 0..FILTER_SIZE {
                        for fj in 0..FILTER_SIZE {
                            sum += input[i + fi - 1][j + fj - 1] * filter[fi][fj];
                        }
                    }
                    row[j - padding] = sum;  // Adjust index to account for padding in the output
                }
            }
        }
    });
}

