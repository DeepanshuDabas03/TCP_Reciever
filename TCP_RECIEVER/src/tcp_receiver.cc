#include "tcp_receiver.hh"

#include <algorithm>

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg)
{
    const TCPHeader head = seg.header();
    if (!_synReceived && !head.syn)
    {
        return;
    }
    // if input is not received and we have not recieved any segment for trasmission, return

    else if (!_synReceived)
    {
        _isn = head.seqno;
        _synReceived = true;
    }
    // input is recieved but _synReceived is not updated, implies this is a first segment, therefore update _synReceived and isn

    // ...
    // --- Hint ------
    // convert the seqno into absolute seqno
    uint64_t checkpoint = _reassembler.stream_out().bytes_written() + 1;
    uint64_t abs_seqno = unwrap(head.seqno, _isn, checkpoint);
    uint64_t stream_idx = abs_seqno - 1 + head.syn;
    // --- Hint ------
    // ...
    _finReceived = head.fin;
    _reassembler.push_substring(seg.payload().copy(), stream_idx, _finReceived);
    // push substring to reassembler at index stream_idx which is the seqno of the segment
}

optional<WrappingInt32> TCPReceiver::ackno() const
{
    if (!_synReceived)
        return {};
    return wrap(_reassembler.stream_out().bytes_written() + 1 + _reassembler.stream_out().input_ended(), _isn);
    ;
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }