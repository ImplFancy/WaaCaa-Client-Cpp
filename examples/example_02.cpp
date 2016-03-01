/************************************************************************/
/* WaaCaa Socket API Example 02                                         */
/* Example 02: show how to plot 1-D wave rapidly with synchronous sound */
/* This example is running on Windows (DirectSound)                     */
/* Usage: example_02 xxx.wav                                            */
/*                   xxx.wav is the 16-bit RIFF WAVE sound file         */
/************************************************************************/

#define  _CRT_SECURE_NO_WARNINGS

#include "Client.h"
#include "Chart.h"

#include <list>
#include <iostream>
#include <fstream>
#include <thread>

#include <dsound.h>
#include <Mmreg.h>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")


namespace Example02
{
    // define SafeRelease
    template<class Interface>
    inline void SafeRelease(Interface *pInterfaceToRelease) {
        if (pInterfaceToRelease != nullptr) {
            (pInterfaceToRelease)->Release();
            (pInterfaceToRelease) = nullptr;
        }
    }

    template <typename T>
    class Buffer
    {
    public:
        Buffer() = default;

        // copy constructor
        Buffer(const Buffer& another) {
            mSize = another.mSize;
            mData = another.mData;
        }

        Buffer(const size_t size) {
            alloc(size);
        }

        Buffer(T *p, const size_t size) {
            mData = p;
            mSize = size;
        }

        ~Buffer() {
            // deallocate();
        }

        void alloc(const size_t size) {
            mSize = size;
            mData = (T*)malloc(size * sizeof(T));
            memset(mData, 1, size);
        }

        void deallocate() {
            if (mData != nullptr) {
                delete mData;
            }
            mSize = 0;
        }

        bool dotMultiply(const Buffer &another) const {
            if (mSize != another.mSize) {
                // g_log.e("mSize != another->mSize in another");
                return false;
            }
            if ((mData == nullptr) || (mData == nullptr)) {
                // g_log.e("((mData==nullptr) || (mData==nullptr))");
                return false;
            }

            for (size_t i = 0; i < mSize; i++) {
                mData[i] = mData[i] * another.mData[i];
            }

            return true;
        }

        void set(size_t index, const T &val) const {
            if (index > mSize)
                return;
            mData[index] = val;
        }

        void clear() const {
            for (size_t index = 0; index < mSize; index++) {
                mData[index] = 0;
            }
        }

        T *get() const {
            return mData;
        }

        const T &get(size_t index) const {
            return mData[index];
        }

        Buffer sub(const size_t start, const size_t length) const {
            Buffer newBuffer;
            newBuffer.mData = &(mData[start]);
            newBuffer.mSize = length;

            return newBuffer;
        }

        size_t size() const {
            return mSize;
        }


    private:
        T *mData;
        size_t mSize;
    };

    typedef Buffer<double> Buffer_real;
    typedef Buffer<int16_t> Buffer_int16;
    typedef Buffer<uint16_t> Buffer_uint16;
    typedef Buffer<uint32_t> Buffer_uint32;
    typedef Buffer<char> Buffer_char;


    struct WavInfo {
        uint32_t fileSize;
        uint32_t headerLength;
        uint16_t nbChannel;
        uint32_t sampleRate;
        uint32_t bytePerSecond;
        uint16_t bolckAlign;
        uint16_t sampleBits;
        bool operator==(const WavInfo &rhs) {
            if (bolckAlign != rhs.bolckAlign) return false;
            if (bytePerSecond != rhs.bytePerSecond) return false;
            if (fileSize != rhs.fileSize) return false;
            if (headerLength != rhs.headerLength) return false;
            if (nbChannel != rhs.nbChannel) return false;
            if (sampleBits != rhs.sampleBits) return false;
            if (sampleRate != rhs.sampleRate) return false;
            return true;
        }
    };

    namespace WavConst {
        static const std::string RIFF = "RIFF"; // 0x52494646,
        static const std::string WAVE = "WAVE"; // 0x57415645,
        static const std::string FMT_ = "fmt "; // 0x666d7420,
        static const std::string DATA = "data"; // 0x64617461
                                                //    const char *STREAM_BUFFER_SIZE = 4096
    }

    class WavFile
    {
    public:
        WavFile() = default;

        bool openWavFile(const std::string fileName)
        {
            mFileName = fileName;
            // g_log.d(("wave file: " + mFileName).c_str());
            mFileStream = new std::ifstream(mFileName.c_str(), std::ios::binary);
            if (!mFileStream->is_open()) {
                // g_log.e((fileName + " could not open").c_str());
                return false;
            }

            return true;
        }
        bool getWavHeaderInfo(WavInfo &mWavInfo)
        {
            if (!mFileStream) { return false; }

            uint32_t tmp_uint32;
            uint16_t tmp_uint16;

            // "RIFF"
            if (!readString(WavConst::RIFF)) return false;
            // mFileSize
            if (!readUInt32_LE(&mWavInfo.fileSize)) return false;
            // "WAVE"
            if (!readString(WavConst::WAVE)) return false;
            // "fmt "
            if (!readString(WavConst::FMT_))  return false;

            // 16
            if (!readUInt32_LE(&tmp_uint32)) return false;

            // 1
            if (!readUInt16_LE(&tmp_uint16)) return false;


            // mWavInfo.nbChannel
            if (!readUInt16_LE(&mWavInfo.nbChannel)) return false;
            // mWavInfo.sampleRate
            if (!readUInt32_LE(&mWavInfo.sampleRate)) return false;
            // mWavInfo.bytePerSample
            if (!readUInt32_LE(&mWavInfo.bytePerSecond)) return false;
            // mWavInfo.bolckAlign
            if (!readUInt16_LE(&mWavInfo.bolckAlign)) return false;
            // mWavInfo.bolckAlign
            if (!readUInt16_LE(&mWavInfo.sampleBits)) return false;

            mWavInfo.headerLength = 44;


            // pass other syntax
            for (; ; ) {
                auto ret = readString(WavConst::DATA);
                readUInt32_LE(&tmp_uint32);
                if (ret) {
                    break;
                }
                skip(tmp_uint32);
                mWavInfo.headerLength += tmp_uint32;
            }

            return true;
        }

        bool readAllPayload(WavInfo &mWavInfo, Buffer_int16 &mSignal)
        {
            auto payloadSize = (mWavInfo.fileSize + 8) - mWavInfo.headerLength;
            mSignal.alloc(payloadSize);
            mFileStream->read((char *)mSignal.get(), payloadSize);

            return true;
        }

    private:
        bool readString(std::string goldString)
        {
            uint8_t *goldBuffer = (uint8_t *)goldString.c_str();
            uint8_t goldBufferSize = goldString.length();
            char *tmpBuffer = (char *)malloc(goldBufferSize);
            auto success = true;

            mFileStream->read(tmpBuffer, goldBufferSize);
            for (int i = 0; i < goldBufferSize; i++) {
                if (tmpBuffer[i] != goldBuffer[i]) {
                    success = false;
                    break;
                }
            }


            if (!success) {
                free(tmpBuffer);
                return false;
            }

            free(tmpBuffer);
            return success;
        }

        bool skip(uint8_t length)
        {
            mFileStream->seekg(length, std::ios::cur);
            return true;
        }

        bool readUInt32_LE(uint32_t *p_val)
        {
            if (p_val == nullptr) {
                // g_log.e("p_val == nullptr in  WavFile::readUInt32_LE");
                return false;
            }

            mFileStream->read((char *)p_val, 4);

            return true;
        }

        bool readUInt16_LE(uint16_t *p_val)
        {
            if (p_val == nullptr) {
                // g_log.e("p_val == nullptr in  WavFile::readUInt16_LE");
                return false;
            }

            mFileStream->read((char *)p_val, 2);
            return true;
        }

        std::ifstream *mFileStream;
        std::string mFileName;
    };


    class DSoundWrapper
    {
        LPDIRECTSOUND       m_pDSound;
        LPDIRECTSOUNDBUFFER m_pBuffer;

        unsigned int m_bps, m_sizeOfChunk, m_frameSize, m_bufferLen, m_bufferOffset, m_cacheLen, m_timeouts;
        bool m_running, m_inited, m_isDirty;

    public:
        DSoundWrapper()
            :m_pBuffer(nullptr)
            , m_pDSound(nullptr)
            , m_bps(0u)
            , m_sizeOfChunk(0u)
            , m_frameSize(0u)
            , m_bufferLen(0u)
            , m_cacheLen(0u)
            , m_inited(false)
            , m_timeouts(0u)
            , m_bufferOffset(0u)
            , m_isDirty(false)
        {
        }

        ~DSoundWrapper()
        {
            DeInit();
        }

        bool Init(unsigned int sampleRate, unsigned int nbChannels)
        {
            if (m_inited) {
                return false;
            }

            // use default Device
            if (FAILED(DirectSoundCreate(nullptr, &m_pDSound, nullptr))) {
                return false;
            }

            if (FAILED(m_pDSound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY))) {
                m_pDSound->Release();
                return false;
            }

            WAVEFORMATEXTENSIBLE waveFormatEx = { 0 };

            ZeroMemory(&waveFormatEx, sizeof(WAVEFORMATEXTENSIBLE));
            waveFormatEx.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
            waveFormatEx.Format.nChannels = nbChannels;
            waveFormatEx.Format.nSamplesPerSec = sampleRate;
            waveFormatEx.Format.wBitsPerSample = 16;
            waveFormatEx.Format.wFormatTag = WAVE_FORMAT_PCM;
            waveFormatEx.Samples.wValidBitsPerSample = waveFormatEx.Format.wBitsPerSample;
            waveFormatEx.Format.nBlockAlign = waveFormatEx.Format.nChannels * (waveFormatEx.Format.wBitsPerSample >> 3);
            waveFormatEx.Format.nAvgBytesPerSec = waveFormatEx.Format.nSamplesPerSec * waveFormatEx.Format.nBlockAlign;

            m_bps = waveFormatEx.Format.nAvgBytesPerSec;

            auto frameCount = (unsigned int)(sampleRate * 0.010); // 10ms chunks
            m_frameSize = waveFormatEx.Format.nBlockAlign;
            m_sizeOfChunk = m_frameSize * frameCount;
            m_bufferLen = m_sizeOfChunk * 12; // 12 chunks by default

            DSBUFFERDESC dsBufferDescription;
            memset(&dsBufferDescription, 0, sizeof(DSBUFFERDESC));
            dsBufferDescription.dwSize = sizeof(DSBUFFERDESC);
            dsBufferDescription.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_TRUEPLAYPOSITION | DSBCAPS_GLOBALFOCUS;
            dsBufferDescription.dwBufferBytes = m_bufferLen;
            dsBufferDescription.lpwfxFormat = (WAVEFORMATEX *)&waveFormatEx;

            auto res = IDirectSound_CreateSoundBuffer(m_pDSound, &dsBufferDescription, &m_pBuffer, nullptr);
            if (res != DS_OK) {
                if (dsBufferDescription.dwFlags & DSBCAPS_LOCHARDWARE) {
                    SafeRelease(m_pBuffer);
                    dsBufferDescription.dwFlags &= ~DSBCAPS_LOCHARDWARE;
                    res = IDirectSound_CreateSoundBuffer(m_pDSound, &dsBufferDescription, &m_pBuffer, nullptr);
                }
                if (res != DS_OK) {
                    SafeRelease(m_pBuffer);
                    return false;
                }
            }

            m_pBuffer->Stop();

            m_frameSize = (waveFormatEx.Format.wBitsPerSample >> 3) * nbChannels;

            m_bufferOffset = 0u;
            m_cacheLen = 0u;
            m_inited = true;
            m_isDirty = false;
        }

        unsigned int AddPackets(char *pData, unsigned int frames, unsigned int offset)
        {
            if (!m_inited) {
                return 0u;
            }

            DWORD total = m_frameSize * frames;
            auto len = total;
            unsigned char* pBuffer = (unsigned char*)(pData) + offset* m_frameSize;

            DWORD bufferStatus = 0u;
            if (m_pBuffer->GetStatus(&bufferStatus) != DS_OK) {
                return 0u;
            }
            if (bufferStatus & DSBSTATUS_BUFFERLOST) {
                m_pBuffer->Restore();
            }

            while (GetSpace() < total) {
                if (m_isDirty) {
                    return INT_MAX;
                }
                else {
                    Sleep(total * 1000 / m_bps);
                }
            }

            for (; len != 0;) {
                LPVOID start = nullptr, startWrap = nullptr;
                DWORD size = 0, sizeWrap = 0;
                if (m_bufferOffset >= m_bufferLen) {
                    m_bufferOffset = 0u;
                }
                DWORD dwWriteBytes = m_sizeOfChunk < len ? m_sizeOfChunk : len;
                HRESULT res = m_pBuffer->Lock(m_bufferOffset, dwWriteBytes, &start, &size, &startWrap, &sizeWrap, 0);
                if (DS_OK != res) {
                    m_isDirty = true;
                    return INT_MAX;
                }

                memcpy(start, pBuffer, size);

                pBuffer += size;
                len -= size;

                m_bufferOffset += size;
                if (startWrap) {
                    memcpy(startWrap, pBuffer, sizeWrap);
                    m_bufferOffset = sizeWrap;

                    pBuffer += sizeWrap;
                    len -= sizeWrap;
                }

                m_cacheLen += size + sizeWrap; // This data is now in the cache
                m_pBuffer->Unlock(start, size, startWrap, sizeWrap);
            }

            DWORD status = 0;
            if (m_pBuffer->GetStatus(&status) != DS_OK) {
                return 0u;
            }

            if (!(status & DSBSTATUS_PLAYING) && m_cacheLen != 0) {
                m_pBuffer->Play(0, 0, DSBPLAY_LOOPING);
            }

            return (total - len) / m_frameSize; // Frames used
        }

    private:
        void DeInit()
        {
            if (!m_inited) {
                return;
            }

            if (m_pBuffer) {
                m_pBuffer->Stop();
                SafeRelease(m_pBuffer);
            }

            if (m_pDSound) {
                m_pDSound->Release();
            }

            m_inited = false;
            m_pBuffer = nullptr;
            m_pDSound = nullptr;
            m_bufferOffset = 0u;
            m_cacheLen = 0u;
            m_sizeOfChunk = 0u;
            m_bufferLen = 0u;
        }


        unsigned int GetSpace()
        {
            DWORD posOfPlay = 0;
            DWORD posForWrite = 0;
            if (DS_OK != m_pBuffer->GetCurrentPosition(&posOfPlay, &posForWrite)) {
                m_isDirty = true;
                unsigned int space = m_bufferLen - m_cacheLen;

                if (space > m_sizeOfChunk) {
                    return space - m_sizeOfChunk;
                }
                else {
                    return 0;
                }
            }

            if ((posOfPlay > posForWrite && m_bufferOffset < posForWrite) ||
                (posOfPlay > posForWrite && posOfPlay < m_bufferOffset) ||
                (posOfPlay < m_bufferOffset && m_bufferOffset < posForWrite))
            {
                m_bufferOffset = posForWrite;
                m_timeouts++;
                if (m_timeouts > 10) {
                    m_isDirty = true;
                    unsigned int space = m_bufferLen - m_cacheLen;

                    if (space > m_sizeOfChunk) {
                        return space - m_sizeOfChunk;
                    }
                    else {
                        return 0;
                    }
                }
            }
            else {
                m_timeouts = 0;
            }

            if (posOfPlay == m_bufferOffset && m_bufferOffset == posForWrite) {
                m_cacheLen = 0;
            }
            else if (m_bufferOffset > posOfPlay) {
                m_cacheLen = m_bufferOffset - posOfPlay;
            }
            else {
                m_cacheLen = m_bufferLen - (posOfPlay - m_bufferOffset);
            }

            unsigned int space = m_bufferLen - m_cacheLen;

            if (space > m_sizeOfChunk) {
                return space - m_sizeOfChunk;
            }
            else {
                return 0;
            }
        }
    };

    unsigned int frameCnt;
    bool m_newDataComing;
    bool m_isGoingToDie;
    float *m_pData(nullptr);

    void PlotingThread()
    {
        m_isGoingToDie = false;

        m_pData = (float *)malloc(frameCnt * sizeof(float));

        // "127.0.0.1": localhost
        if (WaaCaa::Client::Instance().Connect("127.0.0.1", 6666)) {
            WaaCaa::Client::Instance().CloseAllChart();
            auto pChart = WaaCaa::Client::Instance().CreateOneChart(WaaCaa::Chart::MainChartType::Cartesian);

            pChart->SetMaxNumbersOfDataset(1u);
            pChart->SetViewport(-1, 1, 0, frameCnt);
            pChart->ViewportHoldOn();
            bool isHoldViewport(false);

            for (; pChart != nullptr; ) {
                while (!m_newDataComing);
                pChart->Line(m_pData, frameCnt, "");

                if (m_isGoingToDie) break;
                m_newDataComing = false;
            }
            WaaCaa::Client::Instance().CloseChart(*pChart);
        }

        free(m_pData);
    }

    void OnNewData(short *charData)
    {
        for (auto i = 0u; i < frameCnt; i++) {
            m_pData[i] = (float)charData[i * 2] / 32767;
        }
        m_newDataComing = true;
    }
}


int main(int argc, char **argv)
{
    using namespace Example02;

    if (argc != 2) {
        std::cout << "Usage: example_02 xxx.wav" << std::endl;
        return -1;
    }

    WavFile wavFile;
    if (wavFile.openWavFile(argv[1])) {
        WavInfo wavInfo;
        if (wavFile.getWavHeaderInfo(wavInfo)) {
            Buffer_int16 inputSignal;
            if (wavFile.readAllPayload(wavInfo, inputSignal)) {
                DSoundWrapper ds;
                ds.Init(wavInfo.sampleRate, wavInfo.nbChannel);

                frameCnt = wavInfo.sampleRate * 0.01; // 0.01 second buffer

                std::thread plotingThread(PlotingThread);

                unsigned int lenOffset(0);
                for (; ; ) {
                    auto lenOut = ds.AddPackets((char*)(inputSignal.get()), frameCnt, lenOffset);
                    OnNewData((short *)((inputSignal.get()) + lenOffset * wavInfo.nbChannel));
                    lenOffset += lenOut;
                    if (lenOut < frameCnt) {
                        break;
                    }
                    if ((lenOffset + frameCnt) * wavInfo.nbChannel * wavInfo.sampleBits / 8 >= inputSignal.size()) {
                        break;
                    }
                }

                m_isGoingToDie = true;
                plotingThread.join();
            }

            inputSignal.deallocate();
        }
    }

    return 0;
}
