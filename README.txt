This project includes files of four algorithms. Each file contains the following components:

- Cargo.toml: Configuration file for the Rust project.
- main.rs: The Rust source code file.
- <algorithm_name>.c: A C source code file that uses OpenMP for parallel computation.

Rust:
1. Run the command to create a new Cargo: cargo new <algorithm_name>
2. Replace Cargo.toml with the provided configuration file. 
3. Replace main.rs under src/ directory with the provided Rust source code.
4. Run the command to comiple the Cargo: cargo build --release

OpenMP:
1. You compile it as: gcc -fopenmp -o <algorithm_name> <algorithm_name>.c

Notes
1. Replace <algorithm_name> with the name of the algorithm.
2. Avoid using the same names for Rust and OpenMP compiled executables.