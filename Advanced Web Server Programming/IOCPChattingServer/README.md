# IOCP TCP/IP Chatting Server (Final Project - Advanced Web Server Programming)

![IOCP chat server add packet](https://raw.githubusercontent.com/kj1241/Server_Portfolio/refs/heads/main/Image/IOCP%20%EC%84%9C%EB%B2%84%20%ED%8C%A8%ED%82%B7%EC%B6%94%EA%B0%80%20%EC%95%84%ED%82%A4%ED%85%8D%EC%B3%90%20%EB%8B%A4%EC%9D%B4%EC%96%B4%EA%B7%B8%EB%9E%A8%20ver.2.webp)

This project is an IOCP-based TCP/IP chat server application developed using C++. The server utilizes I/O Completion Ports (IOCP) for asynchronous communication with multiple clients. When a client sends a message, the server asynchronously receives it and broadcasts it to all connected clients, enabling real-time chat. This project leverages WinAPI to build an efficient and scalable server application.

### Key Features:
1. **Asynchronous I/O with IOCP**: Utilizes I/O Completion Port (IOCP) to handle multiple client connections asynchronously, allowing efficient message broadcasting.
2. **Multithreaded Server**: The server is designed with multithreading capabilities using Windows Thread API (`CreateThread`) and synchronization techniques such as Critical Sections and Interlocked Functions.
3. **Packet Design**: Designed custom packets for transmitting user IDs and chat messages between clients and the server.
4. **Chat UI**: Developed a user-friendly chat interface using WinAPI that displays real-time chat updates from multiple clients.
5. **Client Management**: The server maintains a list of all connected clients using STL, enabling efficient message broadcasting to all clients.

### Tech Stack:
- **Language**: C++
- **Platform**: Windows
- **Network Programming**: IOCP (I/O Completion Port), TCP/IP
- **APIs**: Windows Thread API (`CreateThread`), Windows Synchronization API (`CreateCriticalSection`, `InterlockedIncrement`)
- **Development Tools**: Visual Studio 2015 (upgraded to 2019)
- **Concurrency**: Multithreading, Asynchronous I/O
- **File Handling**: STL for managing client socket lists

### Project Flow:
- **Multithreading & Synchronization**: The server uses Critical Sections and Interlocked Functions to synchronize multiple threads. This ensures that client requests are processed concurrently and safely without conflicts.
- **Packet Transmission**: Packets were designed to clearly separate user IDs from chat messages, enabling efficient communication between clients and the server.
- **Client-Server Communication**: Messages sent from one client are broadcasted to all connected clients in real time, allowing seamless communication in a chat environment.

### Architecture Diagram:
The architecture diagram illustrates the client-server communication model, leveraging IOCP for efficient asynchronous message handling. The server processes messages from clients and broadcasts them to all connected clients using a thread pool for optimized performance.

### Project Links:
- [GitHub Repository](https://github.com/kj1241/Server_Portfolio/tree/main/Advanced%20Web%20Server%20Programming/IOCPChattingServer)

For more details, visit the [Dev_Programming](https://kj1241.github.io/server_tp/Server_Client_IOCP_Chatting_Program)
