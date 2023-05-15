#include <cmath>
#include <vector>

#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "SobelFilter.h"
#include <algorithm>
#include <iostream>

using namespace std;
/*11*/
SobelFilter::SobelFilter(sc_module_name n) : sc_module(n)
{
  #ifndef NATIVE_SYSTEMC
	//HLS_FLATTEN_ARRAY(data_buffer);
	//HLS_FLATTEN_ARRAY(box);
	//HLS_FLATTEN_ARRAY(center);
	//HLS_FLATTEN_ARRAY(result);
#endif
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
#ifndef NATIVE_SYSTEMC
	  i_rgb.clk_rst(i_clk, i_rst);
  	o_avg.clk_rst(i_clk, i_rst);
#endif
}
/*111*/

SobelFilter::~SobelFilter() {}

/*222*/
void SobelFilter::do_filter()
{
  {
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_rgb.reset();
		o_avg.reset();
#endif
		wait();
	}




  sc_uint<9> check = 0;
  //int width = i_img_width.read() * 4;
  //cout << "Now at width " << width << endl;
  sc_uint<8> temp_r[9] = {0};
  sc_uint<8> temp_g[9] = {0};
  sc_uint<8> temp_b[9] = {0};

  /*3333*/
  while (true)
  {

    sc_uint<8> buffer_r[6] = {0};
    sc_uint<8> buffer_g[6] = {0};
    sc_uint<8> buffer_b[6] = {0};
/*444*/
    avg_r = 0;
    avg_g = 0;
    avg_b = 0;
/*444*/
    // buffer move

    for (sc_uint<2> v = 0; v < MASK_Y; ++v)
    {
      for (sc_uint<2> u = 0; u < MASK_X; ++u)
      {

        if((check == 0) || (u==2 && check != 0))
        {
        /*read*/
				  sc_dt::sc_uint<24> rgb;
#ifndef NATIVE_SYSTEMC
				  {
				  	HLS_DEFINE_PROTOCOL("input");
				  	rgb = i_rgb.get();
				  	wait();
				  }
#else
				  rgb = i_rgb.read();
#endif    

          unsigned char i_r;
          unsigned char i_g;
          unsigned char i_b;
          i_r = rgb.range(7,0);
          i_g = rgb.range(15,8);
          i_b = rgb.range(23, 16);
/*read*/  
          temp_r[u + 3 * v] = i_r;
          temp_g[u + 3 * v] = i_g;
          temp_b[u + 3 * v] = i_b;
        }
      


        // if it is center we dont read it still 0
        if (u == 1 && v == 1)
        {
          avg_r = avg_r;
          avg_g = avg_g;
          avg_b = avg_b;
        }
        else // sum
        {
          avg_r += temp_r[u + 3 * v] ;
          avg_g += temp_g[u + 3 * v] ;
          avg_b += temp_b[u + 3 * v] ;
        }
      }
    }
    //  update check
    check ++ ;
    if (check == 256) // if already jump to next row then check==0;
    {
      check = 0;
    }
    if (check != 0)
    {
      buffer_r[0] = temp_r[1];
      buffer_g[0] = temp_g[1];
      buffer_b[0] = temp_b[1];

      buffer_r[1] = temp_r[2];
      buffer_g[1] = temp_g[2];
      buffer_b[1] = temp_b[2];

      buffer_r[2] = temp_r[4];
      buffer_g[2] = temp_g[4];
      buffer_b[2] = temp_b[4];

      buffer_r[3] = temp_r[5];
      buffer_g[3] = temp_g[5];
      buffer_b[3] = temp_b[5];

      buffer_r[4] = temp_r[7];
      buffer_g[4] = temp_g[7];
      buffer_b[4] = temp_b[7];

      buffer_r[5] = temp_r[8];
      buffer_g[5] = temp_g[8];
      buffer_b[5] = temp_b[8];
    }

    sort(temp_r, temp_r + 9);
    sort(temp_g, temp_g + 9);
    sort(temp_b, temp_b + 9);

    avg_r += temp_r[4]<<1;
    avg_g += temp_g[4]<<1;
    avg_b += temp_b[4]<<1;
    avg_r /= 10;
    avg_g /= 10;
    avg_b /= 10;

    if (check != 0)
    {
      temp_r[0] = buffer_r[0];
      temp_g[0] = buffer_g[0];
      temp_b[0] = buffer_b[0];

      temp_r[1] = buffer_r[1];
      temp_g[1] = buffer_g[1];
      temp_b[1] = buffer_b[1];

      temp_r[3] = buffer_r[2];
      temp_g[3] = buffer_g[2];
      temp_b[3] = buffer_b[2];

      temp_r[4] = buffer_r[3];
      temp_g[4] = buffer_g[3];
      temp_b[4] = buffer_b[3];

      temp_r[6] = buffer_r[4];
      temp_g[6] = buffer_g[4];
      temp_b[6] = buffer_b[4];

      temp_r[7] = buffer_r[5];
      temp_g[7] = buffer_g[5];
      temp_b[7] = buffer_b[5];
    }

    /*
    o_avg_r.write(avg_r);
    o_avg_g.write(avg_g);
    o_avg_b.write(avg_b);
    */
    sc_dt::sc_uint<24> out_avg_all;
    out_avg_all.range(7, 0)   = avg_r;
    out_avg_all.range(15, 8)  = avg_g;
    out_avg_all.range(23, 16) = avg_b;

    #ifndef NATIVE_SYSTEMC
    		{
    			HLS_DEFINE_PROTOCOL("output");
    			o_avg.put(out_avg_all);
    			wait();
    		}
    #else
    		o_avg.write(out_avg_all);
    #endif
  }
}
