#ifndef GLOBALS_H
#define GLOBALS_H

// Global Constants used in the project.
namespace GC
{
    extern size_t hits;
    extern size_t misses;
    extern size_t numRequests;
    extern size_t cycle_counter;
    extern Request *current_request;
    extern int max_cycles;
    extern int tlb_line_length;
    extern unsigned tlbSize;
    extern unsigned blocksize;
    extern unsigned tlbsLatency;
    extern unsigned memoryLatency;
    extern unsigned v2bBlockOffset;
    extern unsigned number_of_tagBits;
    extern unsigned number_of_offsetBits;
    extern unsigned number_of_tlb_indexBits;
}

#endif // GLOBALS_H