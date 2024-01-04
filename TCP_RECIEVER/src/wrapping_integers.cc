#include "wrapping_integers.hh"

#include <iostream>

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { return isn + uint32_t(n); }

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint)
{
    uint32_t asqn = n - isn;
    // the absolute sequence number i.ie relative sequence number - initial sequence number
    if (checkpoint <= asqn)
    {
        // the absolute sequence number is larger than the checkpoint, so we don't need to unwrap
        return asqn;
    }
    // 2^32 using left shift operator
    uint64_t two_pow = uint64_t(1) << 32;
    uint64_t a = asqn + (((checkpoint - asqn) / (two_pow)) * two_pow);
    // number of times we have sequence number counts up to 2^32-1 from 0 is given by (checkpoint - asqn) / (2^32) and then we multiply it by 2^32 to get number of bytes and then add it to absolute sequence number of current to get the  of this checkpoint
    uint64_t b = a + two_pow;
    // if we add 2^32 to a, we can directly get a number larger than checkpoint wihtout affecting sequence number (since a and a+2^32 is simply a if we take mod with 2^32 which is done for wrap around in 32 bit integer )and we can compare it with checkpoint to see which one is closer to checkpoint that is whether a or a+2^32 is closer to checkpoint, since we know we need to return 64 bit sequence number that wraps to n and is closest to 'checkpoint'
    if (checkpoint - a >= b - checkpoint)
    {
        return b;
    }
    else
    {
        return a;
    }
    // we need to check whether the a or b is closer to checkpoint, we can do by comparing the distance between checkpoint and a and b which is checkpoint- a and b - checkpoint respectively
}
