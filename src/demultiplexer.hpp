#ifndef DEMULTIPLEXER_HPP
#define DEMULTIPLEXER_HPP

#include <systemc>
#include "global_constants.hpp"

#include <systemc.h>
using namespace sc_core;

// Decides which TLB line to write the new TLB entry to
SC_MODULE(DEMULTIPLEXER)
{
    sc_in<bool> input;
    sc_in<sc_bv<32>> choose;
    sc_vector<sc_out<bool>> output;
    sc_event choose_written_event;


    SC_CTOR(DEMULTIPLEXER) : output("output", GC::tlbSize)
    {
        SC_THREAD(behaviour);
        sensitive << choose_written_event;
        dont_initialize();
    }
    void behaviour()
    {
        while (true)
        {
            for (size_t i = 0; i < GC::tlbSize; i++)
            {
                output[i] = false;
            }
            output[choose.read().to_uint()] = input.read();
            wait();
        }
    }
};
#endif