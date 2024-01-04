#include "byte_stream.hh"

#include <algorithm>

// You will need to add private members to the class declaration in `byte_stream.hh`

/* Replace all the dummy definitions inside the methods in this file. */

using namespace std;

ByteStream::ByteStream(const size_t capa)
    : streamCapacity(capa), bufferRead(0), bufferWrite(0), streamBuffer(deque<char>()),
      inputEnd(0), _error(0)
{
}

size_t ByteStream::write(const string &data)
{
  if (input_ended())
  {
    return 0;
  }
  size_t i = streamBuffer.size(), j = 0;
  // adding data into buffer queue till its capacity
  while (j < data.length() && i < streamCapacity)
  {
    // add data into queue
    streamBuffer.push_back(data[j]);
    // update i variable to point latest size of buffer.
    i = streamBuffer.size();
    j++;
  }
  // update value of buffer written till now
  bufferWrite = bufferWrite + j;
  return j;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const
{
  string ans = "";
  // Copy data upto least of buffer size and given length
  for (int i = 0; i < min(buffer_size(), len); i++)
  {
    ans += streamBuffer[i];
  }
  return ans;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len)
{
  size_t bufferSize = min(bufferSize, len);
  for (int i = 0; i < bufferSize; i++)
  {
    streamBuffer.pop_front();
  }
  bufferRead = bufferRead+bufferSize;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len)
{
  string ans = peek_output(len);
  pop_output(len);
  return ans;
}

void ByteStream::end_input() { inputEnd = 1; }

bool ByteStream::input_ended() const { return inputEnd; }

size_t ByteStream::buffer_size() const { return (size_t)streamBuffer.size(); }

bool ByteStream::buffer_empty() const { return streamBuffer.empty(); }

bool ByteStream::eof() const { return (buffer_empty() && input_ended()); }

size_t ByteStream::bytes_written() const { return bufferWrite; }

size_t ByteStream::bytes_read() const { return bufferRead; }

size_t ByteStream::remaining_capacity() const { return (streamCapacity-buffer_size()); }
