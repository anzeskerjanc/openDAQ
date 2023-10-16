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
#include <thread>
#include "websocket_streaming/websocket_streaming.h"
#include <opendaq/device_ptr.h>
#include <opendaq/reader_factory.h>

BEGIN_NAMESPACE_OPENDAQ_WEBSOCKET_STREAMING

class AsyncPacketReader
{
public:
    using OnPacketCallback = std::function<void(const SignalPtr& signal, const ListPtr<IPacket>& packets)>;

    AsyncPacketReader();
    ~AsyncPacketReader();

    void startReading(const DevicePtr& device, const ContextPtr& context);
    void stopReading();
    void onPacket(const OnPacketCallback& callback);
    void setLoopFrequency(uint32_t freqency);

protected:
    void startReadThread();
    void createReaders();

    DevicePtr device;
    ContextPtr context;
    OnPacketCallback onPacketCallback;
    std::thread readThread;
    bool readThreadStarted = false;
    std::chrono::milliseconds sleepTime;
    std::vector<std::pair<SignalPtr, PacketReaderPtr>> signalReaders;
};

END_NAMESPACE_OPENDAQ_WEBSOCKET_STREAMING
