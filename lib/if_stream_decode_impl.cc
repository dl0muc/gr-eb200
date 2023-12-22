/* -*- c++ -*- */
/*
 * Copyright 2023 gr-eb200 author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "if_stream_decode_impl.h"
#include <gnuradio/io_signature.h>
#include <netinet/in.h>

#define dout m_pDebugEnabled && std::cout

#define EB200_DEFAULT_UDP_PACKET_SIZE 32796 // Byte
#define EB200_SAMPLE_SIZE_SHORT 4
#define EB200_SAMPLE_SIZE_LONG 8

namespace gr {
namespace eb200 {

using input_type = char;
using output_type = gr_complex;
if_stream_decode::sptr if_stream_decode::make(bool debug) {
  return gnuradio::make_block_sptr<if_stream_decode_impl>(debug);
}

/*
 * The private constructor
 */
if_stream_decode_impl::if_stream_decode_impl(bool debug)
    : gr::block(
          "if_stream_decode",
          gr::io_signature::make(1, 1, sizeof(input_type)),
          gr::io_signature::make(1, 1, sizeof(output_type))),
    m_pDebugEnabled(debug),
    m_pSynced(false),
    previousSeqNumber(0),
    m_pDataSize(EB200_DEFAULT_UDP_PACKET_SIZE)
    {}


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
    
/*
 * Our virtual destructor.
 */
if_stream_decode_impl::~if_stream_decode_impl() {}

    int
    if_stream_decode_impl::general_work (int noutput_items,
                                         gr_vector_int &ninput_items,
                                         gr_vector_const_void_star &input_items,
                                         gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      // Keep this data across several runs of the worker method
      static EB200_HEADER_TYPE eb200_header;
      static UDP_DATAGRAM_ATTRIBUTE_TYPE udp_datagram_attribute;
      static unsigned short producedSamples = 0;
      // This data is only relevant for the current run of the worker
      unsigned int consumedInputItems = 0;
      int producedOutputItems = 0;

      dout << "ninput_items: " << ninput_items[0] << std::endl;
      dout << "noutput_items: " << noutput_items << std::endl;

      // Not synced -> Looking for EB200 Header
      if(!m_pSynced)
      {
        // Check if we got enough input items to form a whole header
        if(ninput_items[0] >= UDP_HEADER_SIZE)
        {
          decode_header(in, eb200_header, udp_datagram_attribute);

          // Check for magic number and correct mode (901 = IF STREAM)
          if(eb200_header.MagicNumber == 0xeb200
             && eb200_header.VersionMajor == 2
             && udp_datagram_attribute.Tag == 901)
          {
            // Check for sequence errors
            if (eb200_header.SeqNumber != previousSeqNumber + 1) {
            	std::cout << "Packet Sequence error: " << (eb200_header.SeqNumber - previousSeqNumber) << std::endl;
            	previousSeqNumber = eb200_header.SeqNumber;
            } else {
                previousSeqNumber++;
            }
            	
            // Reset sample counter for a new packet
            producedSamples = 0;
            m_pDataSize = eb200_header.DataSize;
            // Header was read successfully
            consumedInputItems += UDP_HEADER_SIZE;
            m_pSynced = true;

            if(m_pDebugEnabled)
            {
              print_eb200_header(eb200_header);
              print_udp_datagram_attribute(udp_datagram_attribute);
            }
          }
        }
      }

      // Header was ok, working on the Payload now
      if(m_pSynced)
      {

        // special case: radio indicates that the packet has no samples
        if (udp_datagram_attribute.NumItems == 0)
        {
          std::cout << "Incomplete packet, skipping" << std::endl;
          m_pSynced = false;
          consumedInputItems=ninput_items[0];
          consume_each(consumedInputItems);
          producedSamples=0;
          return 0;
        }

        // Deciding about sample size (I+Q) by number of items in packet
        short sampleSize = udp_datagram_attribute.Length
                            /udp_datagram_attribute.NumItems;
                
        // Work as long as output buffer is not full and we still got enough
        // input data to form a IQ sample
        while((producedOutputItems < noutput_items)
              && (ninput_items[0]-consumedInputItems) >= sampleSize)
        {
          short real = 0x00;
          short imag = 0x00;
          long reall = 0x00;
          long imagl = 0x00;

          switch(sampleSize)
          {
            case EB200_SAMPLE_SIZE_SHORT:
              memcpy(&real, &in[consumedInputItems], sampleSize/2);
              memcpy(&imag, &in[consumedInputItems+(sampleSize/2)], sampleSize/2);

              break;
            case EB200_SAMPLE_SIZE_LONG:
              memcpy(&reall, &in[consumedInputItems], sampleSize/2);
              memcpy(&imagl, &in[consumedInputItems+(sampleSize/2)], sampleSize/2);

              // Apparently, "long" does not mean 32 bit of usable data per sample
              // Samples look like 00 13 82 00 00 00 4B 00,
              // where only the inner two Bytes contain useful data (1382, 004B)
              // Highest and lowest Byte are discarded for now and we
              // end up with a "short" resolution of 2*16 bit again
              // This behavior was seen e.g. on an EB500 device.
              if(eb200_header.VersionMinor == 97)
              {
                real = (reall >> 8) & 0xFFFF;
                imag = (imagl >> 8) & 0xFFFF;
              }
              // Using the EM100 receiver, the "long" format contains
              // 3 usable bytes, e.g. F4 CC DC 00 17 B3 D3 00,
              // so the least significant byte can be discarded.
              // This is applicable e.g. for EM100.
              else // VersionMinor == 64
              {
                real = (short)(reall >> 16);
                imag = (short)(imagl >> 16);
              }

              break;
            default:
              std::cout << "Unkown sample length, aborting" << std::endl;
              m_pSynced = false;
              consumedInputItems=ninput_items[0];
              consume_each(consumedInputItems);
              producedSamples=0;
              return 0;
              // return -1;
          }
          consumedInputItems += sampleSize;

          // If SWAP flag is not set, do NTOH conversion
          if(!(udp_datagram_attribute.SelectorFlags & 0x20000000))
          {
            real = ntohs(real);
            imag = ntohs(imag);
          }

          // Convert to float
          float realf, imagf;
          realf = (float)real;
          imagf = (float)imag;

          // Assign items to output stream
          out[producedOutputItems++] = gr_complex(realf, imagf);
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
        m_pSynced = false;
        consumedInputItems=ninput_items[0];
        consume_each(consumedInputItems);
        producedSamples=0;
        return 0;
        //return -1;
      }

      dout << "consumedInputItems: " << consumedInputItems << std::endl;
      dout << "producedOutputItems: " << producedOutputItems << std::endl;
      dout << std::endl;

      consume_each(consumedInputItems);
      return producedOutputItems;
    }

} /* namespace eb200 */
} /* namespace gr */
