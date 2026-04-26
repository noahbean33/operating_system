// Library module for minigrep - a simple grep implementation in Rust
// Provides file searching functionality with case-sensitive and case-insensitive options
use std::error::Error;
use std::fs;

// Configuration struct holding command-line arguments and search options
pub struct Config {
    // The search query string to look for
    pub query: String,
    // Path to the file to search in
    pub file_path: String,
    // Whether to perform case-insensitive search
    pub ignore_case: bool,
}

impl Config {
    // Constructs a Config from command-line arguments
    // Returns Result with Config on success or error message on failure
    pub fn build(args: &[String]) -> Result<Config, &'static str> {
        // Validate that we have enough arguments (program name, query, file path)
        if args.len() < 3 {
            return Err("not enough arguments");
        }

        // Clone arguments to create owned strings for Config
        let query = args[1].clone();
        let file_path = args[2].clone();

        // Check if IGNORE_CASE environment variable is set (any value means true)
        let ignore_case = std::env::var("IGNORE_CASE").is_ok();

        Ok(Config {
            query,
            file_path,
            ignore_case,
        })
    }
}

// Main execution function that performs the search operation
// Returns Result with () on success or boxed Error on failure
pub fn run(config: Config) -> Result<(), Box<dyn Error>> {
    // Read entire file contents into a string (? propagates errors)
    let contents = fs::read_to_string(config.file_path)?;

    // Choose search function based on case sensitivity setting
    let results = if config.ignore_case {
        search_case_insensitive(&config.query, &contents)
    } else {
        search(&config.query, &contents)
    };

    // Print each matching line to stdout
    for line in results {
        println!("{line}");
    }

    // Return success (unit type wrapped in Ok)
    Ok(())
}

// Performs case-sensitive search for query in contents
// Lifetime 'a indicates returned references are tied to contents parameter
// Returns vector of string slices (lines) that contain the query
pub fn search<'a>(query: &str, contents: &'a str) -> Vec<&'a str> {
    // Vector to accumulate matching lines
    let mut results = Vec::new();

    // Iterate through each line in the file contents
    for line in contents.lines() {
        // Check if line contains the query string
        if line.contains(query) {
            results.push(line);
        }
    }

    results
}

// Performs case-insensitive search for query in contents
// Converts both query and each line to lowercase for comparison
// Returns vector of string slices (original case) that match the query
pub fn search_case_insensitive<'a>(
    query: &str,
    contents: &'a str,
) -> Vec<&'a str> {
    // Convert query to lowercase once (creates new String)
    let query = query.to_lowercase();
    // Vector to accumulate matching lines
    let mut results = Vec::new();

    // Iterate through each line in the file contents
    for line in contents.lines() {
        // Convert line to lowercase and check if it contains query
        if line.to_lowercase().contains(&query) {
            // Push original line (not lowercase version)
            results.push(line);
        }
    }

    results
}

#[cfg(test)]
mod tests {
    use super::*;

    // Test case-sensitive search functionality
    #[test]
    fn case_sensitive() {
        let query = "duct";
        // Test data with one matching line and one non-matching (Duct with capital D)
        let contents = "\
Rust:
safe, fast, productive.
Pick three.
Duct tape.";

        // Should only match "productive" line, not "Duct tape"
        assert_eq!(vec!["safe, fast, productive."], search(query, contents));
    }

    // Test case-insensitive search functionality
    #[test]
    fn case_insensitive() {
        let query = "rUsT";
        // Test data with mixed case query matching different case lines
        let contents = "\
Rust:
safe, fast, productive.
Pick three.
Trust me.";

        // Should match both "Rust:" and "Trust me." regardless of case
        assert_eq!(
            vec!["Rust:", "Trust me."],
            search_case_insensitive(query, contents)
        );
    }
}
