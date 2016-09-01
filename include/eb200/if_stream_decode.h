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


#ifndef INCLUDED_EB200_IF_STREAM_DECODE_H
#define INCLUDED_EB200_IF_STREAM_DECODE_H

#include <eb200/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace eb200 {

    /*!
     * \brief <+description of block+>
     * \ingroup eb200
     *
     */
    class EB200_API if_stream_decode : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<if_stream_decode> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of eb200::if_stream_decode.
       *
       * To avoid accidental use of raw pointers, eb200::if_stream_decode's
       * constructor is in a private implementation
       * class. eb200::if_stream_decode::make is the public interface for
       * creating new instances.
       */
      static sptr make(bool debug=false);
    };

  } // namespace eb200
} // namespace gr

#endif /* INCLUDED_EB200_IF_STREAM_DECODE_H */

