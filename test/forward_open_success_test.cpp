/**
Software License Agreement (BSD)

\file      forward_open_success_test.cpp
\authors   Kareem Shehata <kareem@shehata.ca>
\copyright Copyright (c) 2015, Clearpath Robotics, Inc., All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that
the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the
   following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
   following disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of Clearpath Robotics nor the names of its contributors may be used to endorse or promote
   products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WAR-
RANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, IN-
DIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <gtest/gtest.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>

#include "odva_ethernetip/forward_open_success.h"
#include "odva_ethernetip/serialization/serializable_buffer.h"
#include "odva_ethernetip/serialization/buffer_reader.h"
#include "odva_ethernetip/serialization/buffer_writer.h"

using boost::shared_ptr;
using boost::make_shared;
using namespace boost::asio;
using eip::ForwardOpenSuccess;
using eip::serialization::SerializableBuffer;
using eip::serialization::BufferReader;
using eip::serialization::BufferWriter;

class ForwardOpenSuccessTest : public :: testing :: Test
{

};

TEST_F(ForwardOpenSuccessTest, test_deserialize_no_data)
{
  EIP_BYTE d[] = {
    // O->T connection ID
    0xAD, 0xDE, 0xEF, 0xBE,
    // T->O connection ID
    0xEF, 0xCD, 0xAB, 0xA9,
    // connection serial number
    0x55, 0xAA,
    // originator vendor ID
    0xA9, 0xCB,
    // originator serial number
    0x12, 0x34, 0x55, 0xAA,
    // O->T Actual Packet Interval
    0x20, 0xA1, 0x07, 0x00,
    // T->O Actual Packet Interval
    0x40, 0x9C, 0x00, 0x00,
    // reply size
    0,
    // reserved byte
    0xFF,
  };

  ForwardOpenSuccess data;
  EXPECT_EQ(sizeof(d), data.getLength());
  BufferReader reader(buffer(d));
  data.deserialize(reader, sizeof(d));
  EXPECT_EQ(sizeof(d), reader.getByteCount());

  EXPECT_EQ(0xBEEFDEAD, data.o_to_t_connection_id);
  EXPECT_EQ(0xA9ABCDEF, data.t_to_o_connection_id);
  EXPECT_EQ(0xAA55, data.connection_sn);
  EXPECT_EQ(0xCBA9, data.originator_vendor_id);
  EXPECT_EQ(0xAA553412, data.originator_sn);
  EXPECT_EQ(500000, data.o_to_t_api);
  EXPECT_EQ(40000, data.t_to_o_api);
  EXPECT_FALSE(data.getResponseData());
}

TEST_F(ForwardOpenSuccessTest, test_deserialize_with_data)
{
  EIP_BYTE d[] = {
    // O->T connection ID
    0xAD, 0xDE, 0xEF, 0xBE,
    // T->O connection ID
    0xEF, 0xCD, 0xAB, 0xA9,
    // connection serial number
    0x55, 0xAA,
    // originator vendor ID
    0xA9, 0xCB,
    // originator serial number
    0x12, 0x34, 0x55, 0xAA,
    // O->T Actual Packet Interval
    0x20, 0xA1, 0x07, 0x00,
    // T->O Actual Packet Interval
    0x40, 0x9C, 0x00, 0x00,
    // reply size
    4,
    // reserved byte
    0xFF,
    // additional data
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xEF,
  };

  ForwardOpenSuccess data;
  BufferReader reader(buffer(d));
  data.deserialize(reader, sizeof(d));
  EXPECT_EQ(sizeof(d), reader.getByteCount());

  EXPECT_EQ(0xBEEFDEAD, data.o_to_t_connection_id);
  EXPECT_EQ(0xA9ABCDEF, data.t_to_o_connection_id);
  EXPECT_EQ(0xAA55, data.connection_sn);
  EXPECT_EQ(0xCBA9, data.originator_vendor_id);
  EXPECT_EQ(0xAA553412, data.originator_sn);
  EXPECT_EQ(500000, data.o_to_t_api);
  EXPECT_EQ(40000, data.t_to_o_api);

  ASSERT_TRUE(data.getResponseData());
  shared_ptr<SerializableBuffer> sb = boost::dynamic_pointer_cast<SerializableBuffer>
    (data.getResponseData());
  ASSERT_TRUE(sb);
  EXPECT_EQ(8, sb->getLength());
  EXPECT_EQ(d + 26, buffer_cast<EIP_BYTE*>(sb->getData()));
}
