/* -*- c++ -*- */
/*
 * Copyright 2016 Christian Obersteiner, DL1COM.
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

#ifndef EB200HEADER_H
#define EB200HEADER_H

/*
* This file contains the EB200 Header structs, similar to the ones
* described in the Application Notes of EM100.
* As we cannot rely on struct __attribute__(packed), we account header
* lengths manually and assign the fields manually in the implementation.
*/

typedef struct Eb200Header
{
  unsigned long   MagicNumber;
  unsigned short  VersionMinor;
  unsigned short  VersionMajor;
  unsigned short  SeqNumber;
  unsigned short  Reserved;
  unsigned long   DataSize;
} EB200_HEADER_TYPE;
// Manual accounting of Header Size
#define EB200_HEADER_SIZE 16

typedef struct UdpDatagramAttribute
{
  unsigned short  Tag;
  unsigned short  Length;
  unsigned short  NumItems;
  unsigned char   Reserved;
  unsigned char   OptHeaderLength;
  unsigned long   SelectorFlags;
  /// \todo Optional Header is not evaluated by now
  //unsigned char   OptHeader[22];
} UDP_DATAGRAM_ATTRIBUTE_TYPE;
// Manual accounting of Header Size
#define ATTRIBUTE_HEADER_SIZE 12

#define UDP_HEADER_SIZE EB200_HEADER_SIZE+ATTRIBUTE_HEADER_SIZE

#endif //EB200HEADER_H
