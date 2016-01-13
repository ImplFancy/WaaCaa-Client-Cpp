/************************************************************************/
/* WaaCaa Socket API Example 01                                         */
/* Example 01: show how to plot scatter diagram and line                */
/************************************************************************/


#include "Client.h"
#include "Chart.h"

#include "example_01_data.h"

#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <cmath>

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
                    // plot scatter diagram (in 2D)
                    pChart->Circle(iceList[i], gameList[i], "ice-game, didntLike"); i++;
                    pChart->Circle(iceList[i], gameList[i], "ice-game, smallDoses"); i++;
                    pChart->Circle(iceList[i], gameList[i], "ice-game, largeDoses"); i++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                {
                    auto i = 0u;
                    pChart->Clear();
                    // plot scatter diagram (in 2D)
                    pChart->Circle(flyList[i], iceList[i], "fly-ice, didntLike"); i++;
                    pChart->Circle(flyList[i], iceList[i], "fly-ice, smallDoses"); i++;
                    pChart->Circle(flyList[i], iceList[i], "fly-ice, largeDoses"); i++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                {
                    auto i = 0u;
                    pChart->Clear();
                    // plot scatter diagram (in 2D)
                    pChart->Circle(flyList[i], gameList[i], "fly-game, didntLike"); i++;
                    pChart->Circle(flyList[i], gameList[i], "fly-game, smallDoses"); i++;
                    pChart->Circle(flyList[i], gameList[i], "fly-game, largeDoses"); i++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 2));
                }

                // least square fitting
                {
                    auto n = flyList->size();
                    auto sum_x = 0.f;
                    auto sum_y = 0.f;
                    auto sum_x2 = 0.f;
                    auto sum_xy = 0.f;

                    for (auto i = 0u; i < n; i++) {
                        const auto &x = flyList[0][i];
                        const auto &y = gameList[0][i];
                        sum_x += x;
                        sum_y += y;
                        sum_x2 += (x * x);
                        sum_xy += (x * y);
                    }
                    auto a = (sum_x*sum_xy - sum_y*sum_x2) / (sum_x*sum_x - n*sum_x2);
                    auto b = (sum_x*sum_y - n*sum_xy) / (sum_x*sum_x - n*sum_x2);

                    pChart->Clear();
                    pChart->Circle(flyList[0], gameList[0], "fly-game, didntLike");
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                    // y = a + b * x;
                    auto y = [&](float a, float b, float x) -> float { return a + b * x; };
                    std::vector<float> lineX; lineX.push_back(0.f); lineX.push_back(1.2f);
                    std::vector<float> lineY; lineY.push_back(y(a, b, lineX[0])); lineY.push_back(y(a, b, lineX[1]));

                    // plot line (in 2D)
                    pChart->Line(lineX, lineY, "least square fitting");
                }

                //WaaCaa::Client::Instance().CloseChart(*pChart);
                delete pChart;
            }
        }

        // show how to plot lines
        {
            std::vector<float> lines01, lines02, lines03 ;
            for (auto i = 0u; i < 100; i++) {
                lines01.push_back(std::sin(i / 10.f));
                lines02.push_back(std::sin(i / 20.f));
                lines03.push_back(std::sin(i / 30.f));
            }
            auto pChart = WaaCaa::Client::Instance().CreateOneChart(WaaCaa::Chart::MainChartType::Cartesian);
            if (pChart != nullptr) {
                // plot line (in 1D)
                pChart->Line(lines01, "sin(i/10)");
                pChart->Line(lines02, "sin(i/20)");
                pChart->Line(lines03, "sin(i/30)");

                delete pChart;
            }
        }
    }

    return 0;
}