#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP
#include <systemc>
using namespace sc_core;

struct Request
{
    uint32_t addr;
    uint32_t data;
    int we;
};

// Processor module that sends requests every clock cycle
SC_MODULE(PROCESSOR)
{
    sc_in<bool> clk;

    // sc_out<Request> did not work. As a workaround, I used three sc_outs
    sc_out<sc_bv<32>> addr;
    sc_out<sc_bv<32>> data;
    sc_out<int> we;

    std::vector<Request> requests;

    SC_CTOR(PROCESSOR);
    PROCESSOR(sc_module_name name, std::vector<Request> requests) : sc_module(name), requests(requests)
    {
        SC_CTHREAD(behaviour, clk.pos());
    }

    // Remove this comment
    void behaviour()
    {
        for (size_t i = 0; i < GC::numRequests; i++)
        {
            GC::current_request = &requests[i];
            addr = requests[i].addr;
            data = requests[i].data;
            we = requests[i].we;

            std::cout << "\nRequest #" << i << ": " << (requests[i].we == 1 ? "W" : "R") << " " << requests[i].addr << requests[i].data << std::endl;

            // COUNTERS UPDATE

            if (GC::cycle_counter + GC::tlbsLatency + GC::memoryLatency >= GC::max_cycles)
            { // check if we could afford a TLB access and a memory access to read or write in the memory
                sc_stop();
            }
            else
            {
                GC::cycle_counter = GC::cycle_counter + GC::tlbsLatency + GC::memoryLatency; // add tlb latency and memory latency to the cycle counter
            }

            wait();
        }

        // FINAL CALCULATIONS HERE

        sc_stop();
    }
};

#endif