# Cache-Simulator

A tool for simulating single-level and multi-level cache systems

## Project Overview

This project is a cache simulator designed to provide hands-on experience with cache systems and their role in improving system performance. The simulator analyzes cache behavior within the memory hierarchy, supporting both single-level and multi-level cache simulation to evaluate performance under different configurations.

## Key Features

- Support for both single-level and multi-level cache structures
- Configurable cache parameters such as size, block size (or cache line size), and associativity
- Performance evaluation metrics, including hit rate, miss rate, and total latency
- Memory access trace-based simulation to analyze cache performance
- Implementation of **Write Back**, **Write Allocate**, and **Least Recently Used (LRU)** replacement policy for efficient cache update and eviction management

## Memory Access Traces

To facilitate cache simulation, memory access traces (Trace) are used to capture memory access patterns of programs. Each trace entry follows this format:

- Each line represents one memory access
- Format: `<op> <address>`, where `op` indicates the operation (e.g., `r` for read, `w` for write), and `address` specifies the memory location accessed
- Memory traces are processed sequentially to simulate cache operations

## Project Structure

```
Cache-Simulator/
├── CMakeLists.txt                       - Project build configuration file
├── include
│   ├── Cache.h                          - Core cache system class definitions
│   ├── Debug.h                          - Debugging utility functions
│   ├── elfio                            - (Can be ignored)
│   ├── MemoryManager.h                  - Memory management
│   └── MultiLevelCacheConfig.h          - Multi-level cache configuration parameters
├── PINTool.tar.gz                       - Will be introduced in Part 4
├── README.md
├── report.md                            - report template
├── src
│   ├── Cache.cpp                        - Implementation of cache system functionality
│   ├── MainMulCache.cpp                 - Multi-level cache simulator entry point
│   ├── MainSinCache.cpp                 - Single-level cache simulator entry point
│   └── MemoryManager.cpp                - Implementation of memory management system
└── trace
    ├── Part1                            - trace files used in Part 1
    ├── Part2                            - trace files used in Part 2
    ├── Part3                            - trace files used in Part 3
    └── Part4                            - trace files you generate in Part 4

```

## How to Run

We use the Docker environment from Project 3.

1. Create and enter the build directory:
```bash
mkdir build
cd build
```

2. Compile the program:
```bash
cmake ..
make
```

3. Run the simulator:
   - Single-level cache simulator example:
     ```bash
     ./CacheSingle ../trace/Part2/test.trace
     ```
   - Multi-level cache simulator:
     ```bash
     ./CacheMulti ../trace/Part2/test.trace
     ```

## Project Developers

- Danyang Chen (123090018@link.cuhk.edu.cn)
- Yuhao Liu (224040365@link.cuhk.edu.cn)

## References

- Hao He. RISCV-Simulator. https://github.com/hehao98/RISCV-Simulator, 2019.
