/************************************************************************/
/* WaaCaa Socket API Example 01                                         */
/* Example 01: show how to plot scatter diagram                         */
/************************************************************************/


#include "Client.h"
#include "Chart.h"

#include "example_01_data.h"

#include <vector>
#include <chrono>
#include <thread>
#include <fstream>

namespace Example01
{
    enum class LikeLabel : unsigned char
    {
        _00 = 0x01,
        _01 = 0x02,
        _02 = 0x03
    };

    struct Person
    {
        float flyLength, gameTime, iceCream;
        LikeLabel label;
    };

    std::vector<Person> m_origPersonList;

    bool LoadTxt(const char *dataStrs, const unsigned int &len)
    {
        m_origPersonList.clear();

        auto lastPos = 0u;
        auto colIndex = 0u;
        Person person;

        for (auto currPos = 0u; currPos < len; currPos++) {
            if (dataStrs[currPos] == '\t') {
                lastPos++;
                colIndex++;
                if (colIndex == 1u) {
                    person.flyLength = atoi(dataStrs + lastPos);
                }
                else if (colIndex == 2u) {
                    person.gameTime = atof(dataStrs + lastPos);
                }
                else if (colIndex == 3u) {
                    person.iceCream = atof(dataStrs + lastPos);
                }
                lastPos = currPos;
            }
            else if (dataStrs[currPos] == '\n') {
                if (dataStrs[lastPos] == '\t') lastPos++;
                switch (dataStrs[lastPos])
                {
                case 'd':
                    person.label = LikeLabel::_00;
                    break;
                case 's':
                    person.label = LikeLabel::_01;
                    break;
                case 'l':
                    person.label = LikeLabel::_02;
                    break;
                default:
                    break;
                }
                m_origPersonList.push_back(person);
                colIndex = 0u;
                lastPos = currPos;
            }
        }

        return true;
    }


    void NormData()
    {
        float flyMinMax[2] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
        float gameMinMax[2] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
        float iceMinMax[2] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };

        for (const auto &v : m_origPersonList) {
            if (v.flyLength < flyMinMax[0]) flyMinMax[0] = v.flyLength;
            if (v.flyLength > flyMinMax[1]) flyMinMax[1] = v.flyLength;
            if (v.gameTime < gameMinMax[0]) gameMinMax[0] = v.gameTime;
            if (v.gameTime > gameMinMax[1]) gameMinMax[1] = v.gameTime;
            if (v.iceCream < iceMinMax[0]) iceMinMax[0] = v.iceCream;
            if (v.iceCream > iceMinMax[1]) iceMinMax[1] = v.iceCream;
        }

        std::vector<float> gameList, flyList, iceList;
        for (auto &v : m_origPersonList) {
            v.flyLength = (v.flyLength - flyMinMax[0]) / (flyMinMax[1] - flyMinMax[0]);
            v.gameTime = (v.gameTime - gameMinMax[0]) / (gameMinMax[1] - gameMinMax[0]);
            v.iceCream = (v.iceCream - iceMinMax[0]) / (iceMinMax[1] - iceMinMax[0]);
        }
    }
} // end of namespace




int main(void)
{
    if (WaaCaa::Client::Instance().Connect("127.0.0.1", 6666)) {
        WaaCaa::Client::Instance().CloseAllChart();

        if (Example01::LoadTxt(dataStrs, sizeof(dataStrs))) {
            Example01::NormData();
            std::vector<float> gameList[3], flyList[3], iceList[3];

            for (auto &v : Example01::m_origPersonList) {
                flyList[(unsigned int)v.label - 1].push_back(v.flyLength);
                gameList[(unsigned int)v.label - 1].push_back(v.gameTime);
                iceList[(unsigned int)v.label - 1].push_back(v.iceCream);
            }

            auto pChart = WaaCaa::Client::Instance().CreateOneChart(WaaCaa::Chart::MainChartType::Cartesian);
            if (pChart != nullptr) {
                {
                    auto i = 0u;
                    pChart->Clear();
                    pChart->Circle(iceList[i], gameList[i], "ice-game, didntLike"); i++;
                    pChart->Circle(iceList[i], gameList[i], "ice-game, smallDoses"); i++;
                    pChart->Circle(iceList[i], gameList[i], "ice-game, largeDoses"); i++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                {
                    auto i = 0u;
                    pChart->Clear();
                    pChart->Circle(flyList[i], iceList[i], "fly-ice, didntLike"); i++;
                    pChart->Circle(flyList[i], iceList[i], "fly-ice, smallDoses"); i++;
                    pChart->Circle(flyList[i], iceList[i], "fly-ice, largeDoses"); i++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                {
                    auto i = 0u;
                    pChart->Clear();
                    pChart->Circle(flyList[i], gameList[i], "fly-game, didntLike"); i++;
                    pChart->Circle(flyList[i], gameList[i], "fly-game, smallDoses"); i++;
                    pChart->Circle(flyList[i], gameList[i], "fly-game, largeDoses"); i++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                delete pChart;
            }
        }
    }

    return 0;
}