#pragma once

#include "utils/System.h"

#include "utils/LinearBuffer.h"

#include <iostream>


class TcpClient
{
public:
    TcpClient(std::string host, int port);

    ~TcpClient();

    bool Connect();

    void SendBytes(const char *buffer, const unsigned int bufferLength);

    void ReceiveBytes(LinearBuffer &buffer);

    bool DisConnect();

    std::string GetHost() { return m_host; }
    int GetPort() { return m_port; }

private:
    int m_socket;

    std::string m_host;

    int m_port;
};