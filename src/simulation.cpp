#include <systemc>
#include "dflipflop.hpp"
#include "TLB.hpp"
#include "processor.hpp"
#include "global_constants.hpp"
#include "main_memory.hpp"
using namespace sc_core;
// #include <sc_trace.h> // make sure this is the right header to include

// Declaring Global Constants to set in global_constants.hpp
namespace GC
{
    int max_cycles;
    unsigned tlbSize;
    unsigned tlbsLatency;
    unsigned blocksize;
    unsigned v2bBlockOffset;
    unsigned memoryLatency;
    unsigned number_of_offsetBits;
    unsigned number_of_tlb_indexBits;
    unsigned number_of_tagBits;
    int tlb_line_length;
    size_t numRequests;
    size_t cycle_counter = 0;
    size_t hits = 0;
    size_t misses = 0;
    Request *current_request = NULL;
}

struct Result
{
    size_t cycles;
    size_t misses;
    size_t hits;
    size_t primitiveGateCount;
};

extern "C" struct Result run_simulation(
    int cycles,
    unsigned tlbSize,
    unsigned tlbsLatency,
    unsigned blocksize,
    unsigned v2bBlockOffset,
    unsigned memoryLatency,
    size_t numRequests,
    struct Request requests[],
    const char *tracefile)
{

    // initialize the global constants
    GC::max_cycles = cycles;
    GC::tlbSize = tlbSize;
    GC::tlbsLatency = tlbsLatency;
    GC::blocksize = blocksize;
    GC::v2bBlockOffset = v2bBlockOffset;
    GC::memoryLatency = memoryLatency;
    GC::numRequests = numRequests;
    GC::number_of_offsetBits = log2(blocksize);
    GC::number_of_tlb_indexBits = log2(tlbSize);
    GC::number_of_tagBits = 32 - (log2(blocksize) + log2(tlbSize));
    GC::tlb_line_length = GC::number_of_tagBits + 32 - GC::number_of_offsetBits + 1; // VA tag + physical number + valid bit

    sc_signal<sc_bv<32>> current_request_address;
    sc_signal<sc_bv<32>> current_request_data;
    sc_signal<int> current_request_we;

    sc_signal<sc_bv<32>> addressOut;
    sc_signal<sc_bv<32>> ValueOut;

    const size_t primitiveGatesCount = 131 + 2 * log2(GC::tlbSize) + 70 * GC::tlbSize + 4 * GC::tlb_line_length;

    Result result = {
        GC::cycle_counter,
        GC::misses,
        GC::hits,
        primitiveGatesCount};

    // the lenght of the requests is dynamic (not known at compile time) so vector used.
    // it's not a signal vector, so standart c++ vector is used
    std::vector<Request> requests_vector(numRequests);
    for (size_t i = 0; i < numRequests; i++)
    {
        requests_vector[i] = requests[i];
    }

    sc_clock clk("clk", sc_time(10, SC_NS));
    PROCESSOR processor("processor", requests_vector);
    processor.clk(clk);
    processor.addr(current_request_address);
    processor.data(current_request_data);
    processor.we(current_request_we);

    sc_signal<sc_bv<32>> p_address;

    TLB tlb("tlb");
    tlb.clk(clk);
    tlb.virtualAddress(current_request_address);
    tlb.physicalAddress(p_address);

    MAIN_MEMORY main_memory("main_memory");
    main_memory.clk(clk);
    main_memory.address(p_address);
    main_memory.write_data(current_request_data);
    main_memory.write_enable(current_request_we);

    sc_trace_file *trace1;
    if (tracefile != NULL)
    {
        // creating the tracefile
        trace1 = sc_create_vcd_trace_file(tracefile);
        if (trace1 == nullptr)
        {
            std::cout << "Failed to create trace file" << std::endl;
            return result;
        }

        // adding the most important signals to the tracefile
        sc_trace(trace1, current_request_address, "current_request_address");
        sc_trace(trace1, current_request_data, "current_request_data");
        sc_trace(trace1, current_request_we, "current_request_we");

        sc_trace(trace1, tlb.VATlbIndexBits, "VATlbIndexBits");
        sc_trace(trace1, tlb.VATlbIndexBits, "VATlbIndexBits");

        sc_trace(trace1, tlb.demux_input, "demux_input");
        for (unsigned j = 0; j < GC::tlbSize; j++) // tlb.demux.output is an sc_vector
        {

            sc_trace(trace1, tlb.demux.output[j], "demux_outputBit");
        }

        for (int i = 0; i < GC::tlb_line_length; i++) // tlb.mux.out is an sc_vector
        {
            sc_trace(trace1, tlb.mux.out[i], "mux_outputBit");
        }
        for (unsigned j = 0; j < GC::tlbSize; j++) // tlb.mux.inputs is an sc_vector of sc_vectors
        {
            for (int i = 0; i < GC::tlb_line_length; i++)
            {
                sc_trace(trace1, tlb.mux.inputs[j][i], "mux_inputBit");
            }
        }

        sc_trace(trace1, tlb.comp_output, "comp_output");

        sc_trace(trace1, tlb.mux_2.choose, "mux_2_choose");

        sc_trace(trace1, p_address, "p_address");

        sc_trace(trace1, addressOut, "addressOut");
        sc_trace(trace1, ValueOut, "ValueOut");
    }

    sc_start();

    if (tracefile != NULL)
    {
        // closing the tracefile only if it is necessary
        sc_close_vcd_trace_file(trace1);
    }

    result = {
        GC::cycle_counter,
        GC::misses,
        GC::hits,
        primitiveGatesCount};
    return result;
}

// This sc_main implementation is needed but does not add anything to the functionality
int sc_main(int argc, char *argv[])
{
    std::cout << "ERROR" << std::endl;
    return 1;
}