/* -*- c++ -*- */
/*
 * Copyright 2023 gr-eb200 author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_EB200_IF_STREAM_DECODE_IMPL_H
#define INCLUDED_EB200_IF_STREAM_DECODE_IMPL_H

#include <gnuradio/eb200/if_stream_decode.h>
#include "eb200header.h"

namespace gr {
namespace eb200 {

class if_stream_decode_impl : public if_stream_decode {
private:
      bool m_pDebugEnabled;
      bool m_pSynced;
      int  previousSeqNumber;
      // Expected DataSize of the received packets, read from Common Header
      unsigned long m_pDataSize;

      void decode_header(const unsigned char *in,
                         EB200_HEADER_TYPE &,
                         UDP_DATAGRAM_ATTRIBUTE_TYPE&);

      void print_eb200_header(EB200_HEADER_TYPE&);
      void print_udp_datagram_attribute(UDP_DATAGRAM_ATTRIBUTE_TYPE&);

public:
  if_stream_decode_impl(bool debug);
  ~if_stream_decode_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
};

} // namespace eb200
} // namespace gr

#endif /* INCLUDED_EB200_IF_STREAM_DECODE_IMPL_H */
