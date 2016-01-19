#pragma once

#include "utils/System.h"

#include "utils/LinearBuffer.h"

#include <iostream>

namespace WaaCaa { class Client; }

class TcpClient
{
public:
    void SendBytes(const char *buffer, const unsigned int bufferLength);

    void ReceiveBytes(LinearBuffer &buffer);

private:
    friend class WaaCaa::Client;

    bool Connect();
    bool DisConnect();

    std::string GetHost() { return m_host; }
    int GetPort() { return m_port; }

    TcpClient(std::string host, int port);
    ~TcpClient();

    int m_socket;

    std::string m_host;

    int m_port;
};