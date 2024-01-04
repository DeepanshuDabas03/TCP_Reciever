
# TCP Receiver and Server Project

## Overview
This project involves the implementation of both a TCP Receiver and Server. The primary goal is to analyze the performance of synchronous and asynchronous TCP servers while ensuring reliable communication between the sender and receiver.

### TCP Receiver Implementation
This section focuses on the TCP Receiver implementation, which consists of building the `ByteStream` class, creating a reassembler to assemble segments received from the sender, and implementing the actual TCP receiver by integrating the `ByteStream` and `Reassembler`.

### Getting Started
Follow these steps to set up and test the TCP Receiver implementation:

1. Clone the following repository
2. Navigate to the project directory and create a new directory named 'build.'
3. Enter the 'build' directory and run `cmake ..` to configure the project.
4. Once configured, run `make` in the same directory to build the project.
5. Verify that the project builds without errors by running `ctest`.

### ByteStream Implementation
The `ByteStream` class serves as a container for storing a collection of bytes, facilitating reliable communication. Key implementation details include push and pop operations, finite capacity, end of file (EOF) handling, initialization, and suitability for single-threaded use.

#### Testing ByteStream Implementation
To validate the implementation of the `ByteStream` class, run the following commands in the build directory:
```bash
make
ctest -R '^byte_stream'
```

### Reassembler Implementation
The `Reassembler` component is responsible for reconstructing byte streams from substrings received from the sender. It efficiently manages three categories of knowledge: next bytes in the stream, buffered bytes within the stream's capacity, and bytes beyond the stream's capacity.

#### Testing Reassembler Implementation
To validate the implementation of the `Reassembler` class, run the following commands in the build directory:
```bash
make
ctest -R '^reassembler'
```

### TCP Server Implementation
This section outlines the tasks related to implementing different TCP server programs, including concurrent servers with multiple processes and threads, as well as non-blocking servers using select(), poll(), and epoll(). The servers perform factorial computations based on incoming requests.

#### Testing TCP Server Implementation
Perform experiments with different numbers of concurrent client connections and collect data using tools like tcpdump or Wireshark. Write Python scripts to compute average throughput and latency for each TCP flow, and observe server process CPU and memory utilization.

## Conclusion
The successful implementation of both the TCP Receiver and Server components deepens your understanding of TCP communication and its challenges. The combined efforts ensure reliable transmission and reception of data in various scenarios. Experimentation and analysis of server performance metrics provide valuable insights for further considerations in server design.
