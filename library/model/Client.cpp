#include "Client.h"

#include "Request.h"
#include "ResponseUtils.h"
#include "network/TcpClient.h"

#include <vector>
#include <iostream>

WaaCaa::Client & WaaCaa::Client::Instance()
{
    static WaaCaa::Client instance;
    return instance;
}

bool WaaCaa::Client::Connect(const char *host, const int port)
{
    if (m_pTcpClient != nullptr) delete m_pTcpClient;
    m_pTcpClient = new TcpClient(host, port);

    return true;
}

bool WaaCaa::Client::Connect()
{
    return Connect("127.0.0.1", 6666);
}

WaaCaa::Chart * WaaCaa::Client::CreateOneChart(const Chart::MainChartType &type) const
{
    if (m_pTcpClient == nullptr) return nullptr;

    Request request(1, 1);
    request.AppendBody((const char *)&type, 1);

    request.Generate();

    LinearBuffer response;
    if (m_pTcpClient->Connect()) {
        m_pTcpClient->SendBytes(request.GetBuffer(), request.GetBufferLen());
        m_pTcpClient->ReceiveBytes(response);
        m_pTcpClient->DisConnect();
        if (*(response.Sub(8).Buffer()) != ResponseType::ALL_RIGHT) {
            return nullptr;
        }
    }
    else {
        return nullptr;
    }

    return new WaaCaa::Chart(m_pTcpClient, type, *(response.Sub(9).Buffer()));
}

bool WaaCaa::Client::CloseChart(const WaaCaa::Chart &chart) const
{
    if (m_pTcpClient == nullptr) return false;

    Request request(1, 2);
    request.AppendBody((const char *)chart.m_ChartIndex, 1);

    request.Generate();

    LinearBuffer response;
    if (m_pTcpClient->Connect()) {
        m_pTcpClient->SendBytes(request.GetBuffer(), request.GetBufferLen());
        m_pTcpClient->ReceiveBytes(response);
        m_pTcpClient->DisConnect();
        if (*(response.Sub(8).Buffer()) != ResponseType::ALL_RIGHT) {
            return false;
        }
    }
    else {
        return false;
    }

    return true;
}

bool WaaCaa::Client::CloseAllChart()
{
    if (m_pTcpClient == nullptr) return false;

    Request request(1, 3);
    request.Generate();

    LinearBuffer response;
    if (m_pTcpClient->Connect()) {
        m_pTcpClient->SendBytes(request.GetBuffer(), request.GetBufferLen());
        m_pTcpClient->ReceiveBytes(response);
        m_pTcpClient->DisConnect();
        if (*(response.Sub(8).Buffer()) != ResponseType::ALL_RIGHT) {
            return false;
        }
    }
    else {
        return false;
    }

    return true;
}

WaaCaa::Client::Client() :
    m_pTcpClient(nullptr)
{

}
