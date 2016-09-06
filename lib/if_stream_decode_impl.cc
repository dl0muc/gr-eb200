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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "if_stream_decode_impl.h"

// For printf
#include <stdio.h>
// For nothl/s functions
#include <netinet/in.h>

#define dout m_pDebugEnabled && std::cout

#define EB200_DEFAULT_UDP_PACKET_SIZE 32796 // Byte
#define EB200_NSAMPLES_SHORT 8192
#define EB200_NSAMPLES_LONG 4096
#define EB200_SAMPLE_SIZE_SHORT 4
#define EB200_SAMPLE_SIZE_LONG 8

namespace gr {
  namespace eb200 {

    if_stream_decode::sptr
    if_stream_decode::make(bool debug)
    {
      return gnuradio::get_initial_sptr
        (new if_stream_decode_impl(debug));
    }

    /*
     * The private constructor
     */
     if_stream_decode_impl::if_stream_decode_impl(bool debug)
       : gr::block("if_stream_decode",
                   gr::io_signature::make(1, 1, sizeof(unsigned char)),
                   gr::io_signature::make(1, 1, sizeof(short))),
         m_pDebugEnabled(debug),
         m_pSynced(false),
         m_pDataSize(EB200_DEFAULT_UDP_PACKET_SIZE)
     {}

    /*
     * Our virtual destructor.
     */
    if_stream_decode_impl::~if_stream_decode_impl()
    {
    }

    /**
    * \brief Get contents of EB200 Header from Input Stream
    */
    void
    if_stream_decode_impl::decode_header(const unsigned char *in,
                                         EB200_HEADER_TYPE &eb200_header,
                                         UDP_DATAGRAM_ATTRIBUTE_TYPE& udp_datagram_attribute)
    {
      memcpy(&eb200_header.MagicNumber, in, sizeof(float));
      memcpy(&eb200_header.VersionMinor, &in[4], sizeof(short));
      memcpy(&eb200_header.VersionMajor, &in[6], sizeof(short));
      memcpy(&eb200_header.SeqNumber, &in[8], sizeof(short));
      memcpy(&eb200_header.Reserved, &in[10], sizeof(short));
      memcpy(&eb200_header.DataSize, &in[12], sizeof(float));

      eb200_header.MagicNumber  = ntohl(eb200_header.MagicNumber);
      eb200_header.VersionMinor = ntohs(eb200_header.VersionMinor);
      eb200_header.VersionMajor = ntohs(eb200_header.VersionMajor);
      eb200_header.SeqNumber    = ntohs(eb200_header.SeqNumber);
      eb200_header.DataSize     = ntohl(eb200_header.DataSize);

      memcpy(&udp_datagram_attribute.Tag, &in[16], sizeof(short));
      memcpy(&udp_datagram_attribute.Length, &in[18], sizeof(short));
      memcpy(&udp_datagram_attribute.NumItems, &in[20], sizeof(short));
      memcpy(&udp_datagram_attribute.Reserved, &in[22], sizeof(char));
      memcpy(&udp_datagram_attribute.OptHeaderLength, &in[23], sizeof(char));
      memcpy(&udp_datagram_attribute.SelectorFlags, &in[24], sizeof(float));

      udp_datagram_attribute.Tag           = ntohs(udp_datagram_attribute.Tag);
      udp_datagram_attribute.Length        = ntohs(udp_datagram_attribute.Length);
      udp_datagram_attribute.NumItems      = ntohs(udp_datagram_attribute.NumItems);
      udp_datagram_attribute.SelectorFlags = ntohl(udp_datagram_attribute.SelectorFlags);

    }

    /**
    * \brief Print contents of EB200 Common Header
    */
    void
    if_stream_decode_impl::print_eb200_header(EB200_HEADER_TYPE &eb200_header)
    {
      printf("MagicNumber : 0x%.8lX\n\r", eb200_header.MagicNumber);
      printf("VersionMinor: %d\n\r", eb200_header.VersionMinor);
      printf("VersionMajor: %d\n\r", eb200_header.VersionMajor);
      printf("SeqNumber   : %d\n\r", eb200_header.SeqNumber);
      printf("Reserved    : 0x%.4X\n\r", eb200_header.Reserved);
      printf("DataSize    : %ld\n\r", eb200_header.DataSize);
    }

    /**
    * \brief Print contents of UDP_DATAGRAM_ATTRIBUTE Header
    */
    void
    if_stream_decode_impl::print_udp_datagram_attribute(
      UDP_DATAGRAM_ATTRIBUTE_TYPE &udp_datagram_attribute)
    {
      printf("AttributeTag   : %d\n\r", udp_datagram_attribute.Tag);
      printf("AttributeLength: %d\n\r", udp_datagram_attribute.Length);
      printf("NumItems       : %d\n\r", udp_datagram_attribute.NumItems);
      printf("Reserved       : 0x%.2X\n\r", udp_datagram_attribute.Reserved);
      printf("OptHeaderLength: %d\n\r", udp_datagram_attribute.OptHeaderLength);
      printf("SelectorFlags  : 0x%.8lX\n\r", udp_datagram_attribute.SelectorFlags);
    }

    /**
    * \brief We always expected to get a complete, reassembled EB200 UDP Packet
    */
    void
    if_stream_decode_impl::forecast (int noutput_items,
                                     gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = m_pDataSize;
    }

    int
    if_stream_decode_impl::general_work (int noutput_items,
                                         gr_vector_int &ninput_items,
                                         gr_vector_const_void_star &input_items,
                                         gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      short *out = (short *) output_items[0];

      static EB200_HEADER_TYPE eb200_header;
      static UDP_DATAGRAM_ATTRIBUTE_TYPE udp_datagram_attribute;
      static unsigned short producedSamples = 0;

      unsigned int consumedInputItems = 0;
      int producedOutputItems = 0;

      dout << "ninput_items: " << ninput_items[0] << std::endl;
      dout << "noutput_items: " << noutput_items << std::endl;

      // Only act if we got enough output buffer for the payload of a whole packet,
      // which is NUM_SAMPLES * 2 (because of interleaved I and Q output)
      if (noutput_items < EB200_NSAMPLES_SHORT*2)
      {
        // Output Buffer too small, do nothing
        return 0;
      }

      // Not synced -> Looking for EB200 Header
      if(!m_pSynced)
      {
        // Check if we got enough input items to form a whole header
        if(ninput_items[0] >= UDP_HEADER_SIZE)
        {
          decode_header(in, eb200_header, udp_datagram_attribute);

          // Check for magic number and correct mode (901 = IF STREAM)
          if(eb200_header.MagicNumber == 0xeb200
             && eb200_header.VersionMinor == 64
             && eb200_header.VersionMajor == 2
             && udp_datagram_attribute.Tag == 901)
          {
            producedSamples = 0;
            consumedInputItems += UDP_HEADER_SIZE;

            m_pDataSize = eb200_header.DataSize;
            m_pSynced = true;

            if(m_pDebugEnabled)
            {
              print_eb200_header(eb200_header);
              print_udp_datagram_attribute(udp_datagram_attribute);
            }
          }
        }
      }

      // Header as ok, working on the Payload now
      if(m_pSynced)
      {
        // Work as long as output buffer is not full and we still got enough
        // input data to form a IQ sample
        while((producedOutputItems < noutput_items)
             && (ninput_items[0]-consumedInputItems)>=4)
        {
          short real = 0x00;
          short imag = 0x00;

          // No marshalling necessary
          memcpy(&real, &in[consumedInputItems], sizeof(short));
          memcpy(&imag, &in[consumedInputItems+2], sizeof(short));
          consumedInputItems += 2*sizeof(short);

          // If SWAP flag is not set, do NOTH conversion
          if(!(udp_datagram_attribute.SelectorFlags & 0x20000000))
          {
            real = ntohs(real);
            imag = ntohs(imag);
          }

          // Assign items to output stream
          out[producedOutputItems++] = real;
          out[producedOutputItems++] = imag;
          producedSamples++;

          // Stop working on this packet when we reach
          // the given number of items (samples)
          if(producedSamples==udp_datagram_attribute.NumItems)
          {
            m_pSynced = false;
            break;
          }
        }
      }
      else
      {
        std::cout << "Sync loss, something went wrong in UDP communication" << std::endl;
        return -1;
      }

      dout << "consumedInputItems: " << consumedInputItems << std::endl;
      dout << "producedOutputItems: " << producedOutputItems << std::endl;
      dout << std::endl;

      consume_each(consumedInputItems);
      return producedOutputItems;
    }

  } /* namespace eb200 */
} /* namespace gr */
