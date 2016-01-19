#include "Chart.h"

#include "model/Request.h"
#include "model/ResponseUtils.h"
#include "network/TcpClient.h"


bool WaaCaa::Chart::Circle(const std::vector<float> &datasetY, const std::string &legend) const
{
    if (datasetY.empty()) return false;

    auto len = datasetY.size() * 4;
    auto p = (const char *)malloc(len);
    float *pFloatY = (float *)(p + 0);
    for (auto index = 0u; index < datasetY.size(); index++) {
        pFloatY[index] = datasetY.at(index);
    }

    auto dataId = SendTypeC(Dimension::_1D, ElemDataType::_ElemFloat32, len, p);
    free((void *)p);

    if (dataId == 0) return false;

    SendTypeD(dataId, 1);
    return true;
}

bool WaaCaa::Chart::Circle(const float *datasetY, const unsigned int nbData, const std::string &legend) const
{
    auto dataId = SendTypeC(Dimension::_1D, ElemDataType::_ElemFloat32, nbData * 4, (char *)datasetY);
    if (dataId == 0) return false;

    SendTypeD(dataId, 1);
    return true;
}

bool WaaCaa::Chart::Circle(const std::vector<float> &datasetX, const std::vector<float> &datasetY,
    const std::string &legend) const
{
    if (datasetX.size() != datasetY.size()) return false;

    auto len = datasetX.size() * 4 * 2;
    auto p = (const char *)malloc(len);
    float *pFloatX = (float *)(p + 0);
    float *pFloatY = (float *)(p + len / 2);
    for (auto index = 0u; index < datasetX.size(); index++) {
        pFloatX[index] = datasetX.at(index);
        pFloatY[index] = datasetY.at(index);
    }

    auto dataId = SendTypeC(Dimension::_2D, ElemDataType::_ElemFloat32, len, p);
    free((void *)p);

    if (dataId == 0) return false;

    SendTypeD(dataId, 1);
    return true;
}

bool WaaCaa::Chart::Line(const std::vector<float> &datasetY, const std::string &legend) const
{
    if (datasetY.empty()) return false;

    auto len = datasetY.size() * 4;
    auto p = (const char *)malloc(len);
    float *pFloatY = (float *)(p + 0);
    for (auto index = 0u; index < datasetY.size(); index++) {
        pFloatY[index] = datasetY.at(index);
    }

    auto dataId = SendTypeC(Dimension::_1D, ElemDataType::_ElemFloat32, len, p);
    free((void *)p);

    if (dataId == 0) return false;

    SendTypeD(dataId, 2);
    return true;
}

bool WaaCaa::Chart::SetMaxNumbersOfDataset(const unsigned char &num) const
{
    return SendTypeB(0x20, (const char *)&num, 1u);
}

bool WaaCaa::Chart::SetViewport(const float &top, const float &bottom, const float &left, const float &right) const
{
    float arr[4];
    arr[0] = top; arr[1] = bottom; arr[2] = left; arr[3] = right;
    return SendTypeB(0x32, (char *)arr, 4*sizeof(float));
}

bool WaaCaa::Chart::ViewportHoldOn() const
{
    return SendTypeB(0x30, nullptr, 0u);
}

bool WaaCaa::Chart::ViewportHoldOff() const
{
    return SendTypeB(0x31, nullptr, 0u);
}

bool WaaCaa::Chart::Line(const float *datasetY, const unsigned int nbData, const std::string &legend) const
{
    auto dataId = SendTypeC(Dimension::_1D, ElemDataType::_ElemFloat32, nbData * 4, (char *)datasetY);
    if (dataId == 0) return false;

    SendTypeD(dataId, 2);
    return true;
}

bool WaaCaa::Chart::Line(const std::vector<float> &datasetX, const std::vector<float> &datasetY, const std::string &legend) const
{
    if (datasetX.size() != datasetY.size()) return false;

    auto len = datasetX.size() * 4 * 2;
    auto p = (const char *)malloc(len);
    float *pFloatX = (float *)(p + 0);
    float *pFloatY = (float *)(p + len / 2);
    for (auto index = 0u; index < datasetX.size(); index++) {
        pFloatX[index] = datasetX.at(index);
        pFloatY[index] = datasetY.at(index);
    }

    auto dataId = SendTypeC(Dimension::_2D, ElemDataType::_ElemFloat32, len, p);
    free((void *)p);

    if (dataId == 0) return false;

    SendTypeD(dataId, 2);
    return true;
}

WaaCaa::Chart::Chart(TcpClient *pTcpClient, MainChartType type, unsigned char chartIndex) :
    m_pTcpClient(pTcpClient), m_type(type), m_ChartIndex(chartIndex)
{

}

WaaCaa::Chart::~Chart()
{

}

bool WaaCaa::Chart::SendTypeB(const unsigned char &subType, const char *attachment, const unsigned int &attachmentLen) const
{
    Request request(2, subType);
    request.AppendOneByte(m_ChartIndex);

    if (0x20 == subType) {
        // Set the max numbers of dataset
        request.AppendOneByte(*attachment); // 0~255
    }
    else if (0x30 == subType) {
        // hold on view port
    }
    else if (0x31 == subType) {
        // hold off view port
    }
    else if (0x32 == subType) {
        request.AppendBody(attachment, attachmentLen);
    }
    else {
        // unknow request
        return false;
    }

    request.Generate();

    LinearBuffer response;
    m_pTcpClient->SendBytes(request.GetBuffer(), request.GetBufferLen());
    m_pTcpClient->ReceiveBytes(response);
    if (*(response.Sub(8).Buffer()) != ResponseType::ALL_RIGHT) {
        return false;
    }
    
    return true;
}

unsigned int WaaCaa::Chart::SendTypeC(const Dimension &dim, const ElemDataType &elemType, const unsigned int &nbBytes, const char * dataBuffer, const ArrangeType &arrangeType /*= ArrangeType::_Seperate*/) const
{
    Request request(3, (unsigned char)dim);
    request.AppendOneByte(m_ChartIndex);
    request.AppendOneByte((unsigned char)arrangeType);
    request.AppendOneByte((unsigned char)elemType);

    request.AppendOneByte((unsigned char)(nbBytes >> (3 * 8u)));
    request.AppendOneByte((unsigned char)((nbBytes << 1 * 8u) >> (3 * 8u)));
    request.AppendOneByte((unsigned char)((nbBytes << 2 * 8u) >> (3 * 8u)));
    request.AppendOneByte((unsigned char)(nbBytes & 0xff));

    request.AppendBody(dataBuffer, nbBytes);

    request.Generate();

    unsigned int dataId = 0;

    LinearBuffer response;
    //if (m_pTcpClient->Connect()) 
    {
        m_pTcpClient->SendBytes(request.GetBuffer(), request.GetBufferLen());
        m_pTcpClient->ReceiveBytes(response);
        //m_pTcpClient->DisConnect();
        if (*(response.Sub(8).Buffer()) != ResponseType::ALL_RIGHT) {
            return 0;
        }
        const char *pDataId = response.Sub(9).Buffer();
        {
            unsigned char tempByte;
            tempByte = *(pDataId++); dataId += (tempByte << (3 * 8u));
            tempByte = *(pDataId++); dataId += (tempByte << (2 * 8u));
            tempByte = *(pDataId++); dataId += (tempByte << (1 * 8u));
            tempByte = *(pDataId++); dataId += tempByte;
        }
    }
    //else {
    //    return 0;
    //}

    return dataId;
}

bool WaaCaa::Chart::SendTypeC(const unsigned char &subType) const
{
    if (subType < 0x10) return false;

    Request request(3, subType);
    request.AppendOneByte(m_ChartIndex);
    request.Generate();

    LinearBuffer response;
    //if (m_pTcpClient->Connect())
    {
        m_pTcpClient->SendBytes(request.GetBuffer(), request.GetBufferLen());
        m_pTcpClient->ReceiveBytes(response);
        //m_pTcpClient->DisConnect();
        if (*(response.Sub(8).Buffer()) != ResponseType::ALL_RIGHT) {
            return false;
        }
    }
    //else {
    //    return false;
    //}

    return true;
}

bool WaaCaa::Chart::SendTypeD(const unsigned int &dataId, const unsigned char &dataShape) const
{
    Request request(4, (unsigned char)1);
    request.AppendOneByte(m_ChartIndex);

    request.AppendOneByte((unsigned char)(dataId >> (3 * 8u)));
    request.AppendOneByte((unsigned char)((dataId << 1 * 8u) >> (3 * 8u)));
    request.AppendOneByte((unsigned char)((dataId << 2 * 8u) >> (3 * 8u)));
    request.AppendOneByte((unsigned char)(dataId & 0xff));

    request.AppendOneByte(dataShape);

    request.Generate();

    LinearBuffer response;
    //if (m_pTcpClient->Connect()) 
    {
        m_pTcpClient->SendBytes(request.GetBuffer(), request.GetBufferLen());
        m_pTcpClient->ReceiveBytes(response);
        //m_pTcpClient->DisConnect();
        if (*(response.Sub(8).Buffer()) != ResponseType::ALL_RIGHT) {
            return false;
        }
    }
    //else {
    //    return false;
    //}

    return true;
}
