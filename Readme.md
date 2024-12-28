# File Similarity Finder (FSF)

## Overview

FSF is a high-performance C++20 command-line tool designed to find duplicate files across two directories by computing MD5 hashes. It efficiently identifies files with identical content, ignoring `.git` directories.

## Features

- Parallel file processing
- MD5 hash-based file comparison
- Skips `.git` directories
- Supports recursive directory scanning
- Low memory footprint
- Fast execution

## Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 10+)
- OpenSSL development libraries
- CMake 3.10+

## Installation

### Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libssl-dev
```

On macOS (using Homebrew):
```bash
brew install cmake openssl
```

### Building

```bash
# Clone the repository
git clone <repository-url>
cd fsf

# Build the project
./b
```

## Usage

```bash
$ fsf dir1 dir2
```

Shows all files that have different names/locations but same content. Warning: can take some time if you use entire drives.

### Output

The program outputs full paths of files with identical MD5 hashes across the specified directories.

## Build Script Options

```bash
$./b -c    # Clean build
$./b -t    # Build and test
$./b -v    # Verbose test output
$./b -ct   # Clean, build, and test
$./b -a    # All: Clean, build, and test with verbose output
```

## Performance

- Utilizes C++20 features
- Concurrent directory processing
- Optimized file reading
- Minimal memory allocation

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

MIT

## Contact

chrisian.schladetsch@gmail.com
