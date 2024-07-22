#ifndef LOOKUP_TABLE_HPP
#define LOOKUP_TABLE_HPP

#include <systemc>
#include <systemc.h>
#include <cmath>
#include "global_constants.hpp"
using namespace sc_core;

// Look up table for converting virtual address to physical address
SC_MODULE(LOOKUP_TABLE)
{
    sc_in<sc_bv<32>> virtual_Address;

    sc_event choose_written_event;

    sc_out<sc_bv<32>> calculated_physical_Address;

    SC_CTOR(LOOKUP_TABLE)
    {
        SC_THREAD(behaviour);
        sensitive << choose_written_event;

    }
    void behaviour()
    {
        while (true)
        {
            calculated_physical_Address = virtual_Address.read().to_uint() + GC::blocksize * GC::v2bBlockOffset;
            wait();
        }
    }
};
#endif