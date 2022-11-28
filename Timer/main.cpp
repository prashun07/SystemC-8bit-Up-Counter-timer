#include<systemc.h>
#include "timer.h"
#include "testbench.h"
int sc_main(int argc, char* argv[]) {
	sc_clock clk("clk", 20, SC_NS);
	sc_signal<bool>rst;
	sc_signal<bool>read_en_sig, write_en_sig, intr0_sig, intr1_sig;
	sc_signal<sc_uint<8>>addr_sig, data_in_sig, data_out_sig;;

	TimerCounter* timer = new TimerCounter("timer");
	timer->addr(addr_sig);
	timer->clk(clk);
	timer->data_in(data_in_sig);
	timer->data_out(data_out_sig);
	timer->intr0(intr0_sig);
	timer->intr1(intr1_sig);
	timer->read_en(read_en_sig);
	timer->write_en(write_en_sig);
	timer->reset(rst);

	testbench *Tb=new testbench("testbench");
	Tb->addr(addr_sig);
	Tb->clk(clk);
	Tb->d_in(data_in_sig);
	Tb->d_out(data_out_sig);
	Tb->intr0(intr0_sig);
	Tb->intr1(intr1_sig);
	Tb->read_en(read_en_sig);
	Tb->write_en(write_en_sig);
	Tb->reset(rst);

	sc_start(100,SC_MS);//run for 100ms
	
	return 0;
}