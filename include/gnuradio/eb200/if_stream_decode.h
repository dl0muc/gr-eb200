/* -*- c++ -*- */
/*
 * Copyright 2023 gr-eb200 author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_EB200_IF_STREAM_DECODE_H
#define INCLUDED_EB200_IF_STREAM_DECODE_H

#include <gnuradio/eb200/api.h>
#include <gnuradio/block.h>

namespace gr {
namespace eb200 {

/*!
 * \brief <+description of block+>
 * \ingroup eb200
 *
 */
class EB200_API if_stream_decode : virtual public gr::block {
public:
  typedef std::shared_ptr<if_stream_decode> sptr;

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
