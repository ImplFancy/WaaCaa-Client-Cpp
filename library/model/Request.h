#pragma once

#include "utils/LinearBuffer.h"

class Request
{
public:
    Request(unsigned char majorType, unsigned char subType);

    void AppendBody(const char *srcBuffer, unsigned int srcBufferLength);

    void AppendOneByte(unsigned char oneByte);

    void AppendOneByte(char oneByte);

    void Generate();

    const char *GetBuffer() { return m_buffer.Buffer(); }

    unsigned int GetBufferLen() { return m_buffer.Length(); }

private:
    LinearBuffer m_buffer;

    unsigned int m_headerLength, m_bodyLengthPointer, m_bodyLength;
};