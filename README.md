# gr-eb200

## General Information

This repository contains blocks for decoding data in the R&S EB200 protocol format.

**Disclaimer: This repository is not created or maintained
by Rohde & Schwarz GmbH & Co. KG.
It is just an open source implementation following the freely accessible
specification of the EB200 protocol.**

The EB200 protocol specification offers the possibility to stream a range
of different data streams.
Each type of data stream has the same header structure,
which defines the type of the data stream in the payload section.

## Installation

```
cd gr-eb200
mkdir build
cd build
cmake ../
sudo make install
sudo ldconfig
```

## Available GNU Radio Blocks

### eb200 IF Stream Decode

#### Usage

For a usage example, see *./examples/eb200_if_stream_udp_example.grc*

To enable IF streaming on the receiver, send the following command using telnet,
where IP is address of the computer running GNU Radio (e.g. 192.168.1.100)

```
trace:udp:tag:on "192.168.1.100",60340,if
```

The sample rate of the IF stream depends on the bandwidth set up in the
receiver. 500 kHz bandwidth correspond to 640 kSamples.

#### Known Limitations

* Supported protocol version: Major: 2, Minor: 64
* Currently, only the EB200 header is evaluated,
the optional header part (e.g. containing Rx frequency) is not supported by now
and has to be deactivated in the stream.
* Only SHORT IQ data streaming is supported (2* 2 Byte per sample).

## Possible Improvements

* Evaluate common and stream specific headers to tag the output stream,
e.g. with the current Rx frequency.
* Add SCPI functionality to the blocks (or create a separate block) to be
able to control the device using GNU Radio (set Rx frequency, bandwidth, AGC/MGC, ...).
Currently, it is necessary to do the configuration manually outside of GNU Radio (e.g. using telnet)

## Further Information

The specification of the EB200 protocol and some application code examples
can be found in the product brochures of R&S monitoring receivers
like ESMD, EB500, EM100, PR100.
