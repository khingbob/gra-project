#ifndef COMPARATOR_HPP
#define COMPARATOR_HPP

#include <systemc>
#include <systemc.h>

#include "global_constants.hpp"
using namespace sc_core;

// A TLB entry consists of a Virtual Adress tag and the physical number.
// The index of the requested VA tells which entry to extract.
// The comparator checks if the requested VA is the same as the extracted VA by comparing their tags.
SC_MODULE(COMPARATOR)
{
    sc_in<sc_bv<32>> tagBits_from_newVA;
    sc_in<sc_bv<32>> tagBits_from_oldVA;
    sc_out<bool> out;

    SC_CTOR(COMPARATOR)
    {
        SC_THREAD(behaviour);
        sensitive << tagBits_from_newVA << tagBits_from_oldVA;
        dont_initialize();
    }
    void behaviour()
    {
        while (true)
        {
            const bool isEqual = (tagBits_from_newVA.read().to_uint() == tagBits_from_oldVA.read().to_uint());

            out = isEqual;
            wait(SC_ZERO_TIME);
            wait();
        }
    }
};

#endif