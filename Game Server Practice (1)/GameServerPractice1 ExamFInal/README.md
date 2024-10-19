# Echo TCP/IP Chatting Server (Final Project - Game Server Practice 1)

![architecture diagram](https://raw.githubusercontent.com/kj1241/Server_Portfolio/refs/heads/main/Image/%EC%97%90%EC%BD%94%20%EC%84%9C%EB%B2%84%20%EB%8B%A4%EC%9D%B4%EC%96%B4%20%EA%B7%B8%EB%9E%A8.webp)
![architecture diagram](https://raw.githubusercontent.com/kj1241/Server_Portfolio/refs/heads/main/Image/%EC%97%90%EC%BD%94%20%EC%84%9C%EB%B2%84%20%ED%8C%A8%ED%82%B7%20%EB%94%94%EC%9E%90%EC%9D%B8.webp)

This project is an Echo TCP/IP chat server and client program developed in C++. The server and clients communicate bidirectionally, with the server receiving messages from clients and sending them back. The project uses the Winsock library to handle TCP/IP socket communication, and it is designed with a multithreaded environment to handle multiple clients simultaneously.

### Key Features:
1. **TCP/IP Socket Communication**: Implemented using the Winsock2 library.
2. **Multithreading**: Handled with `WaitForSingleObject` and `Mutex` for thread synchronization, allowing the server to communicate with multiple clients concurrently.
3. **Packet Design**: Added a `PacketType` structure to support different packet types (e.g., login packets, message packets), making the communication protocol more robust.
4. **Server Logging**: Added logging functionality to track events and activity on the server for better management.

### Tech Stack:
- **Language**: C++
- **Platform**: Windows
- **Libraries**: WinSocket2
- **Development Tools**: Visual Studio 2015 (upgraded to 2019)
- **Multithreading**: `WaitForSingleObject`, `Mutex` for synchronization

### Project Flow:
- **Echo Server (Ver. 1)**: The server receives messages from clients and sends them back in real-time using TCP/IP socket communication.
- **Packet Design (Ver. 2)**: A structured packet format was added, separating login and message data, each processed with specific packet types.

### Project Links:
- [GitHub Repository](https://github.com/kj1241/Server_Portfolio/tree/main/Game%20Server%20Practice%20(1)/GameServerPractice1%20ExamFInal)

For more details, visit the [Dev_Programming](https://kj1241.github.io/server_tp/GameServerLab1_FinalExam).
