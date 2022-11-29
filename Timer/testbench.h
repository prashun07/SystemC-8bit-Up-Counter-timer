#include<systemc.h>

SC_MODULE(testbench) {
	//output
	sc_out<bool> reset;
	sc_out<bool>read_en, write_en;
	sc_out<sc_uint<8> >addr, d_in;

	//input
	sc_in<bool>clk;
	sc_in<bool>intr0, intr1;
	sc_in<sc_uint<8> >d_out;

	bool intr_message = false;

	void interrupt();
	void write_tb();
	void write_method(sc_uint<8>addr, sc_uint<8>val);
	void read_method(sc_uint<8>addr);


	SC_CTOR(testbench) {
		SC_THREAD(write_tb);
		SC_METHOD(interrupt);
		sensitive << intr1 << intr0;
	}

};


void testbench::interrupt() {
	if (intr_message) {
		if (intr1.read()) {
			cout << "Interrupt asserted:Intr1( " << sc_time_stamp() << " )" << endl;
		}
		if (intr0.read()) {
			cout << "Interrupt asserted:Intr0( " << sc_time_stamp() << " )" << endl;
		}
	}
}

void testbench::read_method(sc_uint<8>adr) {
	addr.write(adr);
	read_en.write(1);
	wait(SC_ZERO_TIME);
	cout << "At:: " << sc_time_stamp() << ": Register Value is : " << d_out.read() << endl;
	read_en.write(0);
	wait(SC_ZERO_TIME);
}

void testbench::write_method(sc_uint<8>adr, sc_uint<8>val) {
	addr.write(adr);
	d_in.write(val);
	write_en.write(1);
	wait(SC_ZERO_TIME);
	write_en.write(0);
	wait(SC_ZERO_TIME);
}

void testbench::write_tb() {
	reset.write(1);
	//Enable both interrupt
	cout << "Turn ON Interrupt message At : " << sc_time_stamp() << endl;
	intr_message = true;

	write_method(0x0, 7);

	write_method(0x8, 135); //write timer_cmp to 135

	wait(SC_ZERO_TIME);

	wait(18470, SC_NS);
	read_method(0x4);

	wait(1520, SC_NS);
	read_method(0x4);


	wait(10, SC_NS);//20k ns
	write_method(0x0, 6);//disable timer

	wait(500, SC_NS);//20500ns
	read_method(0x4);


	wait(500, SC_NS);//21k ns
	write_method(0x0, 7);//Enable timer

	wait(8990, SC_NS); //29990 ns
	read_method(0x4);


	wait(10, SC_NS); //At 30k ns 
	write_method(0x8, 500);//changed timer_cmp


	wait(10000, SC_NS); //At 40k ns

	cout << "Turn OFF Interrupt message At: " << sc_time_stamp() << endl;
	intr_message = false;// interrupt off message
	write_method(0x0, 1);
	wait(960000, SC_NS);//At 1 ms

	cout << "Turn ON Interrupt message At: " << sc_time_stamp() << endl;
	intr_message = true;//Enable interrupt message 
	write_method(0x0, 7);
	wait(750, SC_NS);
	read_method(0x4);

	wait(9250, SC_NS);

	cout << "Turn OFF Interrupt message At: " << sc_time_stamp() << endl;
	intr_message = false;
	write_method(0x0, 1);

}
