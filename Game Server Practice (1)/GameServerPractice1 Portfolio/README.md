# Game Server Practice (1) - General Assignment

This project is part of the "Game Server Practice (1)" course assignments, where network socket programming was implemented using C++. It includes server-client communication over TCP/IP, multi-threading, and various asynchronous I/O models for optimizing performance. The project covers a range of topics from basic socket communication to advanced techniques like IOCP, broadcast, and multicast communication.

### Key Features:
1. **Socket Communication**: Implemented server-client communication using both TCP and UDP protocols.
2. **Multi-threading**: Used `WaitForSingleObject` and `Mutex` to handle thread synchronization issues.
3. **Asynchronous I/O**: Developed non-blocking and overlapped I/O models to ensure efficient communication.
4. **Packet Handling**: Designed and processed packet structures between the client and server.
5. **Broadcast and Multicast**: Implemented broadcast and multicast communication for sending data to multiple clients at once.

### Tech Stack:
- **Language**: C++
- **Platform**: Windows
- **Libraries**: WinSocket2
- **Networking Protocols**: TCP/UDP
- **Development Tools**: Visual Studio 2015 (upgraded to 2019)
- **Asynchronous I/O Models**: Non-blocking, Select, Overlapped, Completion Port I/O

### Project Flow:
- **Client IP Handling**: Replaced deprecated `inet_ntoa()` with `inet_ntop()` for improved IP handling.
- **Byte Order Conversion**: Converted between little-endian and big-endian for network transmission.
- **Name Resolution**: Used `getaddrinfo()` and `GetAddrInfoW()` to resolve domain names to IP addresses, supporting both IPv4 and IPv6.
- **Communication Protocols**: Implemented TCP and UDP servers, fixed/variable-length data transmission, and ensured secure data handling.
- **Multi-threading Synchronization**: Used critical sections and events for thread synchronization, ensuring proper resource sharing between threads.
- **Asynchronous I/O Models**: Developed various models such as non-blocking TCP, Select TCP, and Overlapped TCP to optimize network performance.

### Project Links:
- [GitHub Repository](https://github.com/kj1241/Server_Portfolio/tree/main/Game%20Server%20Practice%20(1)/GameServerPractice1%20Portfolio)

For more details, visit the [Dev_Programming](https://kj1241.github.io/server_tp/GameServerLab1).
