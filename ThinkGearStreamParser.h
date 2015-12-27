/*
 * @(#)ThinkGearStreamParser.h    2.0    Mar 04, 2008
 *
 * Copyright (c) 2008 NeuroSky, Inc. All Rights Reserved
 * NEUROSKY PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

/* Ensure this header is only included once */
#ifndef THINKGEAR_STREAM_PARSER_H_
#define THINKGEAR_STREAM_PARSER_H_

/**
 * @file ThinkGearStreamParser.h
 *
 * The ThinkGear Stream Parser is used to parse bytes of ThinkGear
 * data streamed from any source.  Each arriving byte is fed into
 * the Parser using the THINKGEAR_parseByte() function, and a user
 * supplied callback function is called whenever DataRows are decoded
 * from complete ThinkGear Packets.  Each Parser can be initialized
 * to recognize either: streams of ThinkGear Packets, or streams
 * of 2-byte raw values (the old 5V ThinkGear stream format).
 *
 * @author Kelvin Soo
 * @version 2.0 Mar 04, 2008 Kelvin Soo
 *   - Renamed to ThinkGearStreamParser from ThinkGearStreamDecoder.
 *   - Revised to call a callback function instead of stuffing arriving
 *     data values into a ThinkGearData object.
 *   - Renamed symbols according to updated Packet vocabulary.
 * @version 1.0 Nov 27, 2007 Kelvin Soo
 *   - Initial version (ThinkGearStreamDecoder).
 */

/* Include all external libraries required by this header */
//#include <stdlib.h>

/* Disable name-mangling when compiling as C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Data CODE definitions */
#define PARSER_CODE_POOR_QUALITY       0x02
#define PARSER_CODE_ATTENTION          0x04
#define PARSER_CODE_MEDITATION         0x05

#define PARSER_CODE_RAW_SIGNAL         0x80
#define PARSER_CODE_ASIC_EEG_POWER_INT 0x83

/**
 * The Parser is a state machine that manages the parsing state.
 */
typedef struct _ThinkGearStreamParser {

    unsigned char   state;
    unsigned char   lastByte;

    unsigned char   payloadLength;
    unsigned char   payloadBytesReceived;
    unsigned char   payload[256];
    unsigned char   payloadSum;
    unsigned char   chksum;

    void (*handleDataValue)( unsigned char extendedCodeLevel,
                             unsigned char code, unsigned char numBytes,
                             const unsigned char *value );
    void (*handleRawValue)( unsigned char numBytes,
                             const unsigned char *value );

} ThinkGearStreamParser;

/**
 * @param parser              Pointer to a ThinkGearStreamParser object.
 * @param parserType          One of the PARSER_TYPE_* constants defined above:
 *                            PARSER_TYPE_PACKETS or PARSER_TYPE_2BYTERAW.
 * @param handleDataValueFunc A user-defined callback function that will
 *                            be called whenever a data value is parsed
 *                            from a Packet.
 * @param customData          A pointer to any arbitrary data that will
 *                            also be passed to the handleDataValueFunc
 *                            whenever a data value is parsed from a
 *                            Packet.
 *
 * @return -1 if @c parser is NULL.
 * @return -2 if @c parserType is invalid.
 * @return 0 on success.
 */
int
THINKGEAR_initParser( ThinkGearStreamParser *parser, 
                      void (*handleDataValueFunc)(
                          unsigned char extendedCodeLevel,
                          unsigned char code, unsigned char numBytes,
                          const unsigned char *value),
                      void (*handleRawValueFunc)( unsigned char numBytes,
                             const unsigned char *value) );

/**
 * Feeds the @c byte into the @c parser.  If the @c byte completes a
 * complete, valid parser, then the @c parser's handleDataValue()
 * function is automatically called on each DataRow in the Packet.
 * The return value provides an indication of the state of the
 * @c parser after parsing the byte.
 *
 * @param parser Pointer to an initialized ThinkGearDataParser object.
 * @param byte   The next byte of the data stream.
 *
 * @return -1 if @c parser is NULL.
 * @return -2 if a complete Packet was received, but the checksum failed.
 * @return -3 if an invalid Packet with PLENGTH > 170 was detected.
 * @return -4 if an invalid Packet with PLENGTH == 170 was detected.
 * @return -5 if the @c parser is somehow in an unrecognized state.
 * @return 0 if the @c byte did not yet complete a Packet.
 * @return 1 if a Packet was received and parsed successfully.
 *
 */
int
THINKGEAR_parseByte( ThinkGearStreamParser *parser, unsigned char byte );


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* THINKGEAR_STREAM_DECODER_H_ */


