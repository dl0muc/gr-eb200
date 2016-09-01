/* -*- c++ -*- */
/*
 * Copyright 2016 <+YOU OR YOUR COMPANY+>.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_EB200_IF_STREAM_DECODE_IMPL_H
#define INCLUDED_EB200_IF_STREAM_DECODE_IMPL_H

#include <eb200/if_stream_decode.h>

#include "eb200header.h"

namespace gr {
  namespace eb200 {

    class if_stream_decode_impl : public if_stream_decode
    {
     private:
      bool m_pDebugEnabled;
      bool m_pSynced;

      void decode_header(const unsigned char *in,
                         EB200_HEADER_TYPE &,
                         UDP_DATAGRAM_ATTRIBUTE_TYPE&);

      void print_eb200_header(EB200_HEADER_TYPE&);
      void print_udp_datagram_attribute(UDP_DATAGRAM_ATTRIBUTE_TYPE&);

     public:
      if_stream_decode_impl(bool debug);
      ~if_stream_decode_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace eb200
} // namespace gr

#endif /* INCLUDED_EB200_IF_STREAM_DECODE_IMPL_H */
