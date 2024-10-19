# IOCP Chessboard Game Server (Midterm Project - Advanced Web Server Programming)

![ChessServer](https://raw.githubusercontent.com/kj1241/Server_Portfolio/refs/heads/main/Image/%EC%B2%B4%EC%8A%A4%20%EC%8D%A8%EB%B2%84%20%EB%8B%A4%EC%9D%B4%EC%96%B4%20%EA%B7%B8%EB%9E%A8.webp)

This project is a chessboard movement game implemented using C++ and IOCP (I/O Completion Port) for handling server-client communication over TCP/IP. The server manages multiple clients asynchronously, allowing them to interact with a shared chessboard in real-time. The server processes client requests and synchronizes the game state across all clients using multithreading and asynchronous I/O techniques.

### Key Features:
1. **IOCP-based Server**: The server uses I/O Completion Port (IOCP) to handle client requests asynchronously, improving scalability and performance when managing multiple clients.
2. **Multithreading**: Utilizes Windows Thread API (`CreateThread`) to spawn worker threads based on the number of system cores. This allows parallel processing of multiple client requests.
3. **Real-time Chessboard Updates**: The server processes chess piece movement requests and sends the results to all connected clients in real-time, ensuring game state synchronization.
4. **Asynchronous I/O**: Asynchronous communication is handled via IOCP, allowing the server to respond efficiently to multiple clients without blocking.

### Tech Stack:
- **Language**: C++
- **Platform**: Windows
- **Network Programming**: IOCP (I/O Completion Port), TCP/IP
- **APIs**: Windows Thread API (CreateThread), IOCP API, Windows Synchronization API
- **Development Tools**: Visual Studio 2015 (upgraded to 2019)
- **Concurrency**: Asynchronous I/O, Multithreading, Server-Client Communication

### Project Flow:
- **Server Thread Management**: Worker threads are created based on the system’s core count, allowing the server to handle multiple client requests in parallel using IOCP.
- **Client-Server Communication**: Clients send chess piece movement commands to the server, and the server processes these commands and updates the chessboard for all connected clients in real-time.
- **Real-time Synchronization**: The server sends movement results back to the clients, and the chessboard state is updated immediately on all client displays.

### Architecture Diagram:
The architecture showcases how client commands are processed asynchronously by the server using IOCP. The server creates worker threads and handles communication using the I/O Completion Port for efficient real-time updates.

### Project Links:
- [GitHub Repository](https://github.com/kj1241/Server_Portfolio/tree/main/Advanced%20Web%20Server%20Programming/IOCPServerAssignments)

For more details, visit the [Dev_Programming](https://kj1241.github.io/server_tp/Server_Client_IOCP_Chess).
