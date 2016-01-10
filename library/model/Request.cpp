#include "Request.h"



Request::Request(unsigned char majorType, unsigned char subType) :
    m_buffer()
{
    m_buffer.Append("_WCBEGIN", 8);
    m_buffer.AppendOneByte(1); // ver 1.0 
    m_buffer.AppendOneByte(0);
    m_buffer.Append("", 8); // reserved buffer
    m_buffer.AppendOneByte(majorType);
    m_buffer.AppendOneByte(subType);
    m_bodyLengthPointer = m_buffer.Length();
    m_buffer.Append("", 4); // message length
    m_headerLength = m_buffer.Length();
}

void Request::AppendBody(const char *srcBuffer, unsigned int srcBufferLength)
{
    m_buffer.Append(srcBuffer, srcBufferLength);
}

void Request::AppendOneByte(char oneByte)
{
    m_buffer.AppendOneByte(oneByte);
}

void Request::AppendOneByte(unsigned char oneByte)
{
    m_buffer.AppendOneByte((const char)oneByte);
}

void Request::Generate()
{
    m_bodyLength = m_buffer.Length() - m_headerLength;
    auto p = const_cast<char *>(m_buffer.Buffer() + m_bodyLengthPointer);
    *(p++) = (unsigned char)(m_bodyLength >> (3 * 8u));
    *(p++) = (unsigned char)((m_bodyLength << 1 * 8u) >> (3 * 8u));
    *(p++) = (unsigned char)((m_bodyLength << 2 * 8u) >> (3 * 8u));
    *(p++) = (unsigned char)(m_bodyLength & 0xff);
    m_buffer.Append("_WCEND", 6);
}
