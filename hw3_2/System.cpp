#include "System.h"
System::System( sc_module_name n, string input_bmp, string output_bmp ): sc_module( n ), 
	tb("tb"), sobelfilter("sobelfilter"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst"), _output_bmp(output_bmp)
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	sobelfilter.i_clk(clk);
	sobelfilter.i_rst(rst);
	tb.o_rgb(rgb);
	tb.i_avg_rgb(result);
	sobelfilter.i_rgb(rgb);
	sobelfilter.o_avg(result);

  tb.read_bmp(input_bmp);
}

System::~System() {
  tb.write_bmp(_output_bmp);
}
