/************************************************************************/
/* WaaCaa Socket API Example 02                                         */
/* Example 02: show how to plot 1-D data in high frequency              */
/************************************************************************/
#ifndef _CRT_SECURE_NO_WARNINGS
#define  _CRT_SECURE_NO_WARNINGS
#endif

#include "Client.h"
#include "Chart.h"

#include <iostream>


namespace Example02
{
    // PcmFile
    struct PcmFile
    {
        PcmFile() :
            m_fileBuffer(nullptr),
            m_fileLength(0u)
        {

        }

        ~PcmFile()
        {
            if (m_fileBuffer != nullptr) {
                free(m_fileBuffer);
            }
        }

        bool loadPcm(const char *fileInput)
        {
            FILE *fp;
            if ((fp = fopen(fileInput, "rb")) == nullptr) {
                printf("cannot open this file\n");
                return false;
            }

            fseek(fp, 0, SEEK_END);
            m_fileLength = ftell(fp);
            m_fileBuffer = (char *)malloc(m_fileLength);
            fseek(fp, 0, SEEK_SET);

            fread(m_fileBuffer, 1, m_fileLength, fp);
            return true;
        }

        unsigned int m_fileLength;
        char * m_fileBuffer;
    };

    unsigned int sampleTimeInMs = 10;
    unsigned int frameCnt = 44100 * sampleTimeInMs / 1000.f;
}


int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "please infer the path of PCM file (2 channel, 16-bit)" << std::endl;
        return -1;
    }

    using namespace Example02;

    if (WaaCaa::Client::Instance().Connect("127.0.0.1", 6666)) {
        WaaCaa::Client::Instance().CloseAllChart();
        
        PcmFile pcmFile;
        if (pcmFile.loadPcm(argv[1])) {
            unsigned int lenOffset(0);

            auto m_pData = (float *)malloc(frameCnt * sizeof(float));

            auto pChart = WaaCaa::Client::Instance().CreateOneChart(WaaCaa::Chart::MainChartType::Cartesian);
            pChart->SetMaxNumbersOfDataset(1u);
            pChart->SetViewport(-1, 1, 0, frameCnt);
            pChart->ViewportHoldOn();

            for ( ; ; ) {
                short *charData = (short *)((pcmFile.m_fileBuffer) + lenOffset * 4);

                for (auto i = 0u; i < frameCnt; i++) {
                    m_pData[i] = charData[i * 2/*channel*/] / 32767.f;
                }

                pChart->Line(m_pData, frameCnt, "");
                lenOffset += frameCnt;

                if ((lenOffset + frameCnt) * 2/*channel*/ * 2/*16bit*/ >= pcmFile.m_fileLength) break;
            }

            free(m_pData);
        }
    }

    return 0;
}
