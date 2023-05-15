#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif


#include "filter_def.h"

class SobelFilter : public sc_module
{
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
#ifndef NATIVE_SYSTEMC
  cynw_p2p< sc_dt::sc_uint<24> >::in i_rgb;
  cynw_p2p< sc_dt::sc_uint<24> >::out o_avg;
#else
	sc_fifo_in< sc_dt::sc_uint<24> > i_rgb;
	sc_fifo_out< sc_dt::sc_uint<24> > o_avg;
#endif

/*
  sc_fifo_in<unsigned char> i_r;
  sc_fifo_in<unsigned char> i_g;
  sc_fifo_in<unsigned char> i_b;
  sc_fifo_in<unsigned char> i_img_width;

  sc_fifo_out<int> o_avg_r;
  sc_fifo_out<int> o_avg_g;
  sc_fifo_out<int> o_avg_b;
*/
  SC_HAS_PROCESS(SobelFilter);
  SobelFilter(sc_module_name n);
  ~SobelFilter();

private:
  void do_filter();
  sc_uint<14> avg_r;
  sc_uint<14> avg_g;
  sc_uint<14> avg_b;
};
#endif
