#include "stream_reassembler.hh"
// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity),
                                                              _capacity(capacity),
                                                              _start_index(0),
                                                              _end_index(-1),
                                                              _unassembled_bytes(0),
                                                              _buffer(capacity),
                                                              _is_pushed(capacity) {}
                                                              // initialize all private members

void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof)
{
    if (eof)
    {
        _end_index = index + data.length();
    }

    size_t dataIndex = index;
    for (auto &i : data)
    {
        size_t idx = dataIndex - _start_index;
        if (idx < 0)
        {
            ++dataIndex;
        }
        else
        {
            if (dataIndex >= _output.bytes_read() + _capacity)
            // if data is out of range, break, since we can write no more data
            {
                break;
            }
            if (!_is_pushed[idx])
            {
                _is_pushed[idx] = true;
                ++_unassembled_bytes;
                // if data is not added in buffer , we get free space in buffer for that index
                // mark this space as used
            }
            _buffer[idx] = i;
            // add data to buffer at index
            ++dataIndex;
        }
    }

    string assembledData;
    while (_buffer.size() > 0 && _is_pushed[0])
    {
        assembledData += _buffer[0];
        _buffer.pop_front(), _is_pushed.pop_front();
        // delete assembled data from buffer
        ++_start_index;
        // update start index since 1 byte is added to assembled data
        --_unassembled_bytes;
        // byte has been assembled
        _buffer.push_back(0), _is_pushed.push_back(0);
        // add free space in buffer again at the end
    }
    _output.write(assembledData);
    // write assembled data to output

    if (_start_index == _end_index)
    {
        // if all data is assembled, and we reach at the end of data , end input
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const
{
    return _unassembled_bytes;
}

bool StreamReassembler::empty() const
{
    if (_unassembled_bytes == 0)
        return true;
    else
        return false;
}

size_t StreamReassembler::ack_index() const
{
    return _output.bytes_written() + 1;
}
