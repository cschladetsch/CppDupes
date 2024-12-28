# File System Comparator (FSF)

## Overview

FSF is a high-performance C++20 command-line tool designed to compare files across multiple directories. It provides flexible file comparison modes, performance measurement, and detailed reporting.

## Features

- Multiple comparison modes
  - `all`: Compare all files
  - `different`: Show only different files
  - `same`: Show only identical files
  - `unique`: Show unique files

- Performance measurement
- Concurrent file processing
- MD5-based file comparison
- Automatic time logging
- Supports multiple directories

## Prerequisites

- C++20 compatible compiler (GCC 10+)
- CMake 3.10+
- OpenSSL development libraries

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

# Build the project (optional)
mkdir build
cd build
cmake ..
make
```

## Usage

### Basic Usage

```bash
# Compare files in two directories
./fsf same dir1 dir2

# Compare files in multiple directories
./fsf all dir1 dir2 dir3

# Show help
./fsf --help
```

### Performance Measurement

```bash
# Run comparison 10 times
./fsf same -r 10 dir1 dir2
```

### Modes

- `all`: Show all file comparisons
- `different`: Show only files that differ
- `same`: Show only identical files
- `unique`: Show files unique to specific directories

## Output

The tool generates detailed output about file similarities and can log performance metrics to `time-times.txt`.

## Performance Logging

Each run can be logged with:
- Timestamp
- Number of repetitions
- Mean execution time
- Standard deviation
- Comparison mode
- Directories compared

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

MIT.

## Contact

christian.schladetsch@gmail.com
