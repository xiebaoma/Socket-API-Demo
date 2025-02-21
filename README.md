# Socket-API-Demo

This repository contains demonstration code for introducing basic socket programming using the Socket API. The code showcases client-server communication using sockets, with examples for both **blocking** and **non-blocking** modes. The demos are designed to run on both **Windows** and **Linux** systems, making them ideal for cross-platform learning.

## Key Topics Covered:
- **Socket Creation**: How to create and configure sockets.
- **Connection**: Using `connect()` for the client-side and `accept()` for the server-side.
- **Communication**: Sending and receiving data with `send()` and `recv()`.
- **Blocking vs Non-blocking Sockets**: Understanding the differences between blocking and non-blocking sockets.
- **Server-Client Communication Logic**: How the server and client interact with each other using sockets.

## Files Overview:

- `server_blocking.cpp`: A simple server implementation in blocking mode (Windows/Linux).
- `server_nonblocking.cpp`: A simple server implementation in non-blocking mode (Windows/Linux).
- `client_blocking.cpp`: A simple client implementation in blocking mode (Windows/Linux).
- `client_nonblocking.cpp`: A simple client implementation in non-blocking mode (Windows/Linux).

## System Requirements:

- **Windows** or **Linux** operating system.
- A C++ compiler (e.g., GCC for Linux or MinGW/MSVC for Windows).

## How to Build:

### On Linux:
1. Clone the repository:
   ```bash
   git clone https://github.com/xiebaoma/Socket-API-Demo.git
   cd Socket-API-Demo
   ```

2. Compile the code:
   ```bash
   g++ server_blocking.cpp -o server_blocking
   g++ server_nonblocking.cpp -o server_nonblocking
   g++ client_blocking.cpp -o client_blocking
   g++ client_nonblocking.cpp -o client_nonblocking
   ```

### On Windows:
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/Socket-API-Demo.git
   cd Socket-API-Demo
   ```

2. Use `g++` (from MinGW) or MSVC to compile the code.

## Usage:

### Running the Server:
1. **Blocking Server:**
   ```bash
   ./server_blocking
   ```

2. **Non-blocking Server:**
   ```bash
   ./server_nonblocking
   ```

### Running the Client:
1. **Blocking Client:**
   ```bash
   ./client_blocking
   ```

2. **Non-blocking Client:**
   ```bash
   ./client_nonblocking
   ```

### Example of Server-Client Interaction:
1. Start the server first (either blocking or non-blocking).
2. Then start the client to connect to the server.
3. Observe the server handling requests in different modes.

## How It Works:

- **Blocking Mode**: In blocking mode, the server waits for incoming client connections, and each call to `recv()` and `accept()` will block the program's execution until data is received or a connection is established.
  
- **Non-blocking Mode**: In non-blocking mode, the server/client does not block during the connection or data transmission. The `recv()` and `accept()` functions return immediately if no data is available or no connection is made.

## License:

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.