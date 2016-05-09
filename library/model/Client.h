#pragma once

#include "Chart.h"

class TcpClient;

#ifdef WaaCaa_Client_EXPORTS
#define WaaCaa_Client_API __declspec(dllexport)
#else
#define WaaCaa_Client_API __declspec(dllimport)
#endif

namespace WaaCaa
{
    class WaaCaa_Client_API Client
    {
    public:
        static Client &Instance();

        bool Connect();
        bool Connect(const char *host, const int port);
        bool DisConnect();

        Chart *CreateOneChart(const Chart::MainChartType &type) const;

        bool CloseChart(const Chart &chart) const;
        bool CloseAllChart();

    private:
        Client();
        ~Client();

        TcpClient *m_pTcpClient;
    };

} // end of namespace WaaCaa
