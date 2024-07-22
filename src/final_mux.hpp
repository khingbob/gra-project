#ifndef FINAL_MUX_HPP
#define FINAL_MUX_HPP

#include <systemc>
#include <systemc.h>
using namespace sc_core;

// MUX used for choosing between the result from the lookup table and the result from the TLB.
// Seperate multiplexer was creted to match the input types.
SC_MODULE(FINAL_MUX)
{
    sc_in<sc_bv<32>> input1; // old physicalAddress
    sc_in<sc_bv<32>> input2;
    sc_in<bool> choose;

    sc_event choose_written_event;

    sc_out<sc_bv<32>> out;

    SC_CTOR(FINAL_MUX)
    {
        SC_THREAD(behaviour);
        sensitive << choose_written_event;
    }
    void behaviour()
    {
        while (true)
        {
            out = choose ? input1.read() : input2.read();
            wait();
        }
    }
};
#endif