#ifndef TLB_HPP
#define TLB_HPP

#include <systemc>
#include "TLB_Line.hpp"
#include "demultiplexer.hpp"
#include "multiplexer.hpp"
#include "lookup_table.hpp"
#include "comparator.hpp"
#include "final_mux.hpp"
#include "global_constants.hpp"
#include <systemc.h>
#include "and.hpp"
#include "not.hpp"
using namespace sc_core;

SC_MODULE(TLB)
{
    sc_in<sc_bv<32>> virtualAddress;
    sc_in<bool> clk;
    sc_signal<unsigned> VAoffsetBits;
    sc_signal<sc_bv<32>> VATlbIndexBits;
    sc_signal<sc_bv<32>> VATagBits;
    COMPARATOR comp;
    sc_signal<sc_bv<32>> oldVATagBits;
    sc_signal<bool> comp_output;

    MULTIPLEXER mux;
    sc_vector<sc_signal<bool>> mux_output; // old tlbline
    sc_bv<65> mux_output_temp;

    sc_vector<TLB_LINE> tlb_lines;
    sc_vector<sc_signal<bool>> tlb_lines_input;
    sc_vector<sc_vector<sc_signal<bool>>> tlb_lines_output;
    sc_bv<65> tlb_lines_input_temp;

    LOOKUP_TABLE lt; // lookup_table
    sc_signal<sc_bv<32>> ltOutput;

    FINAL_MUX mux_2;
    sc_signal<bool> comp_output_advanced; // comp_output && validBit;
    sc_signal<sc_bv<32>> old_physical_address;
    sc_signal<bool> valid_bit;
    sc_bv<32> oldVATagBits_temp;

    DEMULTIPLEXER demux;
    sc_vector<sc_signal<bool>> demux_output;
    sc_signal<bool> demux_input;
    NOT_GATE not_gate;

    sc_out<sc_bv<32>> physicalAddress;
    sc_bv<32> physialNumber;

    SC_CTOR(TLB) : comp("comparator"),
                   mux_output("mux_output", GC::tlb_line_length),
                   mux("multiplexer"),
                   lt("lookup_table"),
                   demux_output("demux_output", GC::tlbSize),
                   tlb_lines_output("tlb_lines_output", GC::tlbSize),
                   tlb_lines_input("tlb_lines_input", GC::tlb_line_length),
                   tlb_lines("tlb_lines", GC::tlbSize),
                   mux_2("final_mux"),
                   demux("demultiplexer"),
                   not_gate("not")
    {
        for (unsigned i = 0; i < GC::tlbSize; i++)
        {
            tlb_lines_output[i].init(GC::tlb_line_length);
            for (int j = 0; j < GC::tlb_line_length; j++)
            {
                tlb_lines_output[i][j] = false; // inisialise tlb_lines with 0's
            }
        }
        mux.choose(VATlbIndexBits);
        for (unsigned i = 0; i < GC::tlb_line_length; i++)
        {
            mux.out[i](mux_output[i]);
        }

        lt.virtual_Address(virtualAddress);
        lt.calculated_physical_Address(ltOutput); // check if physical address or ltOutput is functioning

        for (unsigned i = 0; i < GC::tlbSize; i++)
        {
            for (int j = 0; j < GC::tlb_line_length; j++)
            {
                mux.inputs[i][j](tlb_lines_output[i][j]);
            }
        }

        comp.out(comp_output);
        comp.tagBits_from_oldVA(oldVATagBits);
        comp.tagBits_from_newVA(VATagBits);

        for (unsigned i = 0; i < GC::tlbSize; i++)
        {
            tlb_lines[i].clk(clk);
            tlb_lines[i].write_signal(demux_output[i]);
            for (int j = 0; j < GC::tlb_line_length; j++)
            {
                tlb_lines[i].d_signals[j](tlb_lines_input[j]);
                tlb_lines[i].q_signals[j](tlb_lines_output[i][j]);
            }
        }

        not_gate.input(comp_output_advanced);
        not_gate.output(demux_input);

        demux.input(demux_input);
        for (unsigned i = 0; i < GC::tlbSize; i++)
        {
            demux.output[i](demux_output[i]);
        }
        demux.choose(VATlbIndexBits);

        mux_2.choose(comp_output_advanced);
        mux_2.input1(old_physical_address);
        mux_2.input2(ltOutput);
        mux_2.out(physicalAddress);
        SC_METHOD(print_tlb_lines); // Printing method for every Cycle
        sensitive << clk.pos();
        dont_initialize();

        SC_THREAD(UpdatetheSecondhalfcycle);
        sensitive << clk.neg();
        dont_initialize();

        SC_THREAD(UpdatetheFirsthalfcycle);
        sensitive << clk.pos();
        dont_initialize();
    }

    void UpdatetheFirsthalfcycle()
    {
        while (true)
        {
            wait(SC_ZERO_TIME);

            // VATBIndexBits are mux/demux  choose
            VAoffsetBits.write(virtualAddress.read().range(GC::number_of_offsetBits - 1, 0).to_uint());                                                                                                    // extracts the VA offsetbits
            VATlbIndexBits.write((GC::number_of_tlb_indexBits == 0) ? sc_bv<32>("0") : virtualAddress.read().range(GC::number_of_offsetBits + GC::number_of_tlb_indexBits - 1, GC::number_of_offsetBits)); // extracts the VA tlb_index_bits
            VATagBits.write(virtualAddress.read().range(31, GC::number_of_offsetBits + GC::number_of_tlb_indexBits));                                                                                      // extracts the VA tagBits
            wait(SC_ZERO_TIME);                                                                                                                                                                            // this is very important all the written signals are written to sc signals and they need time to update the values

            // lookuptable start
            lt.choose_written_event.notify();

            // multiplexer start
            mux.choose_written_event.notify();

            // demultiplexer start
            demux.choose_written_event.notify();
            wait(SC_ZERO_TIME);

            // Some Printing Messages
            std::cout << "Virtual address is : " << virtualAddress.read() << std::endl;         // virtual address
            std::cout << "physical address is :" << ltOutput << std::endl;                      // lookup table ready
            std::cout << "VATlbIndexBits is : " << VATlbIndexBits.read().to_int() << std::endl; // mux/demux choose signal

            for (int i = 0; i < GC::tlb_line_length; i++)
            {
                mux_output_temp[i] = mux_output[i];
            }

            wait();
        }
    }

    void UpdatetheSecondhalfcycle()
    {
        while (true)
        {

            // read from lookuptable output
            //  Extract the physical number from ltOutput
            sc_bv<32> physicalNumber = ltOutput.read().range(31, GC::number_of_offsetBits);

            // Write the physical number to the leftmost bits of tlb_lines_input_temp
            tlb_lines_input_temp.range(GC::number_of_tagBits + GC::number_of_tlb_indexBits - 1, 0) = physicalNumber.range(GC::number_of_tagBits + GC::number_of_tlb_indexBits - 1, 0);

            // Write the tag bits after the physical number
            tlb_lines_input_temp.range(GC::number_of_tlb_indexBits + 2 * GC::number_of_tagBits - 1, GC::number_of_tlb_indexBits + GC::number_of_tagBits) = VATagBits.read().range(31 - (GC::number_of_offsetBits + GC::number_of_tlb_indexBits), 0);

            // Write the valid bit to the leftmost bit after the tag bits
            tlb_lines_input_temp.range(2 * GC::number_of_tagBits + GC::number_of_tlb_indexBits, 2 * GC::number_of_tagBits + GC::number_of_tlb_indexBits) = 1;

            wait(SC_ZERO_TIME);

            // the bits are in reverse order so we need to reverse them
            for (int i = 0; i < GC::tlb_line_length; i++)
            {
                tlb_lines_input[GC::tlb_line_length - 1 - i] = tlb_lines_input_temp.get_bit(i);
            }

            wait(SC_ZERO_TIME); // for lookuptable

            valid_bit.write(mux_output[0].read());

            // writing the comparator input OldVATagBits
            int index = 0;
            for (int i = 0; i < GC::number_of_tagBits; i++)
            {
                oldVATagBits_temp[GC::number_of_tagBits - 1 - i] = mux_output[i + 1];
            }

            oldVATagBits.write(oldVATagBits_temp);
            wait(SC_ZERO_TIME); // for comparator and and gate

            mux_2.choose_written_event.notify();

            // writing the last mux input
            sc_bv<32> old_physical_address_temp;
            old_physical_address_temp.range(GC::number_of_offsetBits - 1, 0) = VAoffsetBits;
            old_physical_address_temp.range(31, GC::number_of_offsetBits) = mux_output_temp.range(31 - GC::number_of_offsetBits, 0);
            old_physical_address.write(old_physical_address_temp);
            wait(SC_ZERO_TIME); // wait for comparator

            comp_output_advanced.write(comp_output.read() && valid_bit.read()); // writing the the choose signal of mux2 and it's the not gate input

            wait(SC_ZERO_TIME); // two wait to make sure the signal propagates in the second mux input and the NOT gate
            wait(SC_ZERO_TIME);

            demux.choose_written_event.notify(); // update the values in the demux after the input is ready

            wait(SC_ZERO_TIME); // signals propagation after demux work

            if (comp_output_advanced) // HITS AND MISSES COUNTER HERE
            {

                GC::hits = GC::hits + 1;
            }
            else
            {
                GC::misses = GC::misses + 1;

                if (GC::cycle_counter + GC::memoryLatency >= GC::max_cycles)
                { // check if we could afford a the misspenalty
                    GC::cycle_counter = SIZE_MAX;
                    sc_stop();
                }
                else
                {
                    GC::cycle_counter = GC::cycle_counter + GC::memoryLatency; // add tlb latency and memory latency to the cycle counter
                }
            }

            wait(SC_ZERO_TIME); // making sure the signals are updated so that the dfliplops can get updated values

            wait();
        }
    }

    void print_tlb_lines()
    {
        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << "IN SIMULATION TIME :" << sc_time_stamp() << std::endl;

        std::cout << "TLB Lines Content:" << std::endl;
        for (unsigned i = 0; i < GC::tlbSize; i++)
        {
            std::cout << "TLB Line " << i << ": ";
            for (unsigned j = 0; j < GC::tlb_line_length; j++)
            {
                std::cout << tlb_lines_output[i][j].read() << "";
            }
            std::cout << std::endl;
        }
    }
};

#endif