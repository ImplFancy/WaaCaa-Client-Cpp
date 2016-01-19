#pragma once

#include <vector>

class TcpClient;
class Request;

namespace WaaCaa
{

    class Client;
    class Chart
    {
    public:

        // Clear all data
        bool Clear() const {
            return SendTypeC(0x11);
        }

        // 1D data
        bool Circle(const std::vector<float> &datasetY, const std::string &legend) const;
        bool Circle(const float *datasetY, const unsigned int nbData, const std::string &legend) const;
        bool Line(const std::vector<float> &datasetY, const std::string &legend) const;
        bool Line(const float *datasetY, const unsigned int nbData, const std::string &legend) const;


        // 2D data
        bool Circle(const std::vector<float> &datasetX, const std::vector<float> &datasetY, const std::string &legend) const;
        bool Line(const std::vector<float> &datasetX, const std::vector<float> &datasetY, const std::string &legend) const;


        // Set the max numbers of dataset
        bool SetMaxNumbersOfDataset(const unsigned char &num) const;
        bool SetViewport(const float &top, const float &bottom, const float &left, const float &right) const;
        bool ViewportHoldOn() const;
        bool ViewportHoldOff() const;

        // Main Chart Type
        enum class MainChartType : unsigned char
        {
            Cartesian = 0x01,
            Pie = 0xE0,
        };

        // Destructor
        ~Chart();





        // TODO
        /*
        void Line(const float *datasetX, const float *datasetY, const int datasetLength);
        void Line(const float *datasetX, const int *datasetY, const int datasetLength);

        void DashLine(const float *datasetX, const float *datasetY, const unsigned int datasetLength);
        void Square(const float *datasetX, const float *datasetY, const unsigned int datasetLength);

        bool Circle(const float *datasetX, const float *datasetY, const unsigned int datasetLength);

        bool Circle(const float *datasetY, const unsigned int datasetLength); // 1D data
        void Circle(const std::vector<double> &datasetY); // 1D data
        void Circle(const std::vector<float> &datasetY); // 1D data
        void Circle(const std::vector<long> &datasetY); // 1D data
        void Circle(const std::vector<unsigned long> &datasetY); // 1D data
        void Circle(const std::vector<int> &datasetY); // 1D data
        void Circle(const std::vector<unsigned int> &datasetY); // 1D data
        void Circle(const std::vector<short> &datasetY); // 1D data
        void Circle(const std::vector<unsigned short> &datasetY); // 1D data
        void Circle(const std::vector<char> &datasetY); // 1D data
        void Circle(const std::vector<unsigned char> &datasetY); // 1D data
        */

        // TODO
        /*
        void SetTitle(const char *title);
        void SetTitle(const wchar_t *title);
        */

    private:
        friend class WaaCaa::Client;

        Chart(TcpClient *pTcpClient, MainChartType type, unsigned char chartIndex);

        // Same as Service 
        enum class Dimension : unsigned char
        {
            _1D = 0x01,
            _2D
        };

        // Same as Service 
        enum class ArrangeType : unsigned char
        {
            _Seperate = 0x01,
            _Compact
        };

        // Same as Service 
        enum class ElemDataType : unsigned char
        {
            _ElemFloat64 = 0x01, // double
            _ElemFloat32,
            _ElemInt64,
            _ElemUInt64,
            _ElemInt32,
            _ElemUInt32,
            _ElemInt16, // short
            _ElemUInt16, // unsigned short
            _ElemInt8,
            _ElemUInt8
        };

        bool SendTypeB(const unsigned char &subType, const char *attachment, const unsigned int &attachmentLen) const;

        bool SendTypeC(const unsigned char &subType) const;

        unsigned int SendTypeC(const Dimension &dim, const ElemDataType &elemType,
            const unsigned int &nbBytes, const char * dataBuffer,
            const ArrangeType &arrangeType = ArrangeType::_Seperate) const;

        bool SendTypeD(const unsigned int &dataId, const unsigned char &dataShape) const;


    private:
        unsigned char m_ChartIndex;

        MainChartType m_type;

        TcpClient *m_pTcpClient;
    };

} // end of namespace WaaCaa