// Binary entry point for minigrep command-line application
// Handles argument parsing, error handling, and program execution
use std::env;
use std::process;

use minigrep::Config;

fn main() {
    // Collect command-line arguments into a vector of strings
    let args: Vec<String> = env::args().collect();

    // Parse arguments into Config struct
    // If parsing fails, print error to stderr and exit with code 1
    let config = Config::build(&args).unwrap_or_else(|err| {
        eprintln!("Problem parsing arguments: {err}");
        process::exit(1);
    });

    // Execute the main search operation
    // If run returns an error, print to stderr and exit with code 1
    if let Err(e) = minigrep::run(config) {
        eprintln!("Application error: {e}");
        process::exit(1);
    }
}
