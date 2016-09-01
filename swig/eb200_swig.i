/* -*- c++ -*- */

#define EB200_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "eb200_swig_doc.i"

%{
#include "eb200/if_stream_decode.h"
%}


%include "eb200/if_stream_decode.h"
GR_SWIG_BLOCK_MAGIC2(eb200, if_stream_decode);
