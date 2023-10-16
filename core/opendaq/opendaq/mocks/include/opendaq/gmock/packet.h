/*
 * Copyright 2022-2023 Blueberry d.o.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <opendaq/packet.h>
#include <opendaq/packet_destruct_callback.h>
#include <coretypes/intfs.h>
#include <gmock/gmock.h>
#include <coretypes/gmock/mock_ptr.h>

struct MockPacket : daq::ImplementationOf<daq::IPacket>
{
    typedef MockPtr<
        daq::IPacket,
        daq::PacketPtr,
        testing::StrictMock<MockPacket>
    > Strict;

    MOCK_METHOD(daq::ErrCode, getType, (daq::PacketType* type), (override MOCK_CALL));
    MOCK_METHOD(daq::ErrCode, subscribeForDestructNotification, (daq::IPacketDestructCallback* packetDestructCallback), (override MOCK_CALL));
    MOCK_METHOD(daq::ErrCode, getRefCount, (daq::SizeT* refCount), (override MOCK_CALL));
};
