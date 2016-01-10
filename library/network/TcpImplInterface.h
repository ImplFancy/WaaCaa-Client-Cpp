#pragma once

class TcpReceiverListenner;

class TcpImplInterface
{
public:
    virtual bool Init(const char *host, const int port) = 0;

    virtual void SendBytes(const char *buffer, const unsigned int bufferLength) = 0;
    
    virtual void ReceiveBytes(TcpReceiverListenner *pReceiveListenner) = 0;
    
    virtual bool DeInit() = 0;
};