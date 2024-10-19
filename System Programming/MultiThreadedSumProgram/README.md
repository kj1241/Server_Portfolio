# System Programming Practice - Multi-Threaded Addition Program (Final Project)

![MultiThreadedSumProgram](https://raw.githubusercontent.com/kj1241/Server_Portfolio/refs/heads/main/Image/%EB%A9%80%ED%8B%B0%20%EC%93%B0%EB%A0%88%EB%93%9C%20%EC%8B%9C%ED%80%80%EC%8A%A4%20%EB%8B%A4%EC%9D%B4%EC%96%B4%20%EA%B7%B8%EB%9E%A8.webp)

This project is a multi-threaded addition program written in C++. It processes large sets of data in parallel using multiple threads, and synchronizes their results efficiently. The program reads data from a file, assigns the data to threads for calculation, and uses semaphores to ensure safe execution and synchronization between threads.

### Key Features:
1. **Thread Creation**: Uses the `CreateThread` API to create multiple threads, each processing a portion of the data in parallel.
2. **Thread Synchronization**: Implements synchronization using semaphores (`CreateSemaphore`) to prevent resource conflicts between threads.
3. **File I/O**: Reads 30 numbers from a `number.txt` file and divides them equally between three threads.
4. **Result Aggregation**: Each thread calculates the sum of its assigned numbers, and the main thread aggregates the results to output the final sum.

### Tech Stack:
- **Language**: C++
- **Platform**: Windows
- **APIs**: Windows Thread API (`CreateThread`), Windows Synchronization API (`CreateSemaphore`)
- **Development Tools**: Visual Studio 2015 (upgraded to 2019)
- **File Handling**: `ifstream` for text file input

### Project Flow:
1. **File Reading**: The program reads 30 numbers from `number.txt`.
2. **Thread Creation and Data Distribution**: Three threads are created, with each thread receiving 10 numbers for calculation.
3. **Parallel Calculation**: Each thread sums its assigned numbers.
4. **Result Aggregation**: The main thread gathers the results from each thread and outputs the final sum.

### Project Links:
- [GitHub Repository](https://github.com/kj1241/Server_Portfolio/tree/main/System%20Programming/MultiThreadedSumProgram)

For more details, visit the [Dev_Programming](https://kj1241.github.io/server_tp/MultiThread)).
