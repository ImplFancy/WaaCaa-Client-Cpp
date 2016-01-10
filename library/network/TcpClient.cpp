#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "TcpClient.h"

#ifdef TARGET_WINDOWS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif // TARGET_WINDOWS



TcpClient::TcpClient(std::string host, int port) :
    m_host(host), m_port(port)
{
#ifdef TARGET_WINDOWS
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::logic_error("winsock load failed");
    }
#endif
}

TcpClient::~TcpClient()
{
#ifdef TARGET_WINDOWS
    WSACleanup();
#endif
}

bool TcpClient::Connect()
{
    struct sockaddr_in serv; //addr of server

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "socket() failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(m_host.c_str());
    serv.sin_port = htons(m_port);

    if (connect(m_socket, (struct sockaddr*)&serv, sizeof(serv)) == INVALID_SOCKET) {
        std::cerr << "connect() failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}

void TcpClient::SendBytes(const char *buffer, const unsigned int bufferLength)
{
    auto iLen = send(m_socket, (const char*)buffer, bufferLength, 0);

    if (iLen == SOCKET_ERROR) {
        std::cerr << "send() failed: " << WSAGetLastError() << std::endl;
        return;
    }
}

void TcpClient::ReceiveBytes(LinearBuffer &buffer)
{
    auto receiveBufferLength = 1 << 10u;
    const char *receiveBuffer = (const char *)malloc(receiveBufferLength);

    auto recvLength = 0u;
    for (bool receiveDone = false; !receiveDone; ) {
        int nRead = recv(m_socket, (char *)(receiveBuffer), receiveBufferLength, 0);
        if (nRead == 0 || nRead < receiveBufferLength) { receiveDone = true; }
        if (nRead < 0) {
            closesocket(m_socket);
            receiveDone = true;
            break;
        }
        else if (nRead > 0) {
            buffer.Append(receiveBuffer, nRead);
        }
    }
}

bool TcpClient::DisConnect()
{
    closesocket(m_socket);

    return true;
}
