#include<systemc.h>
//bit position for timer control register
#define EN 0
#define CMP 1
#define OV 2
//bit position for interrupt status register
#define OV_INTR 1
#define CMP_INTR 0

SC_MODULE(TimerCounter) {
	//input

	sc_in<bool> clk;
	sc_in<bool> reset;
	sc_in<bool>read_en, write_en;
	sc_in<sc_uint<8> >addr, data_in;

	//output
	sc_out<bool>intr0, intr1;
	sc_out<sc_uint<8> >data_out;

	//variables
	sc_uint<8>t_cntrl;
	sc_uint<8>t_val;
	sc_uint<8>t_cmp;
	sc_uint<8>t_intr_status;
	void reset_reg() {
		if (reset.read() == 1) //reset enabled
		{
			t_cntrl = 0;
			t_cmp = 0;
			t_val = 0;
			t_intr_status = 0;
		}
	}
	void timer_counter() {
		if (t_cntrl[EN] == 1) //timer enabled
		{
			if ( t_val == 255) { // overflow interrupt
				if (t_cntrl[OV] == 1) {
					t_intr_status[OV_INTR] = 1; //raise interrupt
				}
				t_val = 0;
			}
			else {
				t_val = t_val + 1;
				t_intr_status[OV_INTR] = 0;
			}
			if (t_cntrl[CMP] == 1 && t_val == t_cmp) {//comparator enabled
				t_intr_status[CMP_INTR] = 1;//assert interrupt for comparator
			}
			else {
				t_intr_status[CMP_INTR] = 0;
			}
		}
		intr0.write(t_intr_status[CMP_INTR]);
		intr1.write(t_intr_status[OV_INTR]);
	}

	void write_timer() {
		if (read_en.read() == 1) {
			switch (addr.read())
			{
			case 0x0: data_out.write(t_cntrl); break;
			case 0x4: data_out.write(t_val); break;
			case 0x8: data_out.write(t_cmp); break;
			default:
				break;
			}
		}
		else if (write_en.read() == 1) {
			sc_uint<8>data = data_in.read();
			switch (addr.read())
			{
			case 0x0: t_cntrl = data; break;
			case 0x8: t_cmp = data; break;
			case 0xC: t_intr_status = data; 
				break;
			default:
				break;
			}
		}
	}
	SC_CTOR(TimerCounter) {
		SC_METHOD(reset_reg);
		sensitive << reset;

		SC_METHOD(write_timer);
		sensitive << write_en << read_en;

		SC_METHOD(timer_counter);
		sensitive << clk.pos();

	}
};
