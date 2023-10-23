#include <testutils/testutils.h>
#include <testutils/memcheck_listener.h>

#include <gtest/gtest.h>
#include <thread>

#include <opendaq/opendaq.h>
#include <opendaq/event_packet_params.h>
#include <opendaq/mock/mock_device_module.h>

using namespace daq;
using namespace std::chrono_literals;

// first param: streaming server type
// second param: primary streaming protocol
// third param: client device connection string
class StreamingTest : public testing::TestWithParam<std::tuple<std::string, std::string, std::string>>
{
public:
    void SetUp() override
    {
        serverInstance = CreateServerInstance();
        clientInstance = CreateClientInstance();
    }

    void TearDown() override
    {
    }

    void generatePackets(size_t packetCount)
    {
        auto devices = serverInstance.getDevices();

        for (const auto& device : devices)
        {
            auto name = device.getInfo().getName();
            if (name == "MockPhysicalDevice")
                device.setPropertyValue("GeneratePackets", packetCount);
        }
    }

    SignalPtr getSignal(const DevicePtr& device, const std::string& signalName)
    {
        auto signals = device.getSignalsRecursive();

        for (const auto& signal : signals)
        {
            const auto descriptor = signal.getDescriptor();
            if (descriptor.assigned() && descriptor.getName() == signalName)
            {
                return signal;
            }
        }

        throw NotFoundException();
    }

    ListPtr<IPacket> tryReadPackets(const PacketReaderPtr& reader, size_t packetCount, uint64_t timeoutMs = 500)
    {
        auto allPackets = List<IPacket>();
        auto lastPacketReceived = std::chrono::system_clock::now();

        while (allPackets.getCount() < packetCount)
        {
            if (reader.getAvailableCount() == 0)
            {
                auto now = std::chrono::system_clock::now();
                uint64_t diffMs = (uint64_t) std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPacketReceived).count();
                if (diffMs > timeoutMs)
                    break;

                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                continue;
            }

            auto packets = reader.readAll();

            for (const auto& packet : packets)
                allPackets.pushBack(packet);

            lastPacketReceived = std::chrono::system_clock::now();
        }

        return allPackets;
    }

    bool packetsEqual(const ListPtr<IPacket>& listA, const ListPtr<IPacket>& listB, bool skipEventPackets = false)
    {
        if (listA.getCount() != listB.getCount())
            return false;

        for (SizeT i = 0; i < listA.getCount(); i++)
        {
            if (skipEventPackets &&
                listA.getItemAt(i).getType() == PacketType::Event &&
                listB.getItemAt(i).getType() == PacketType::Event)
                continue;
            if (!BaseObjectPtr::Equals(listA.getItemAt(i), listB.getItemAt(i)))
                return false;
        }

        return true;
    }

protected:
    virtual InstancePtr CreateServerInstance()
    {
        auto logger = Logger();
        auto scheduler = Scheduler(logger);
        auto moduleManager = ModuleManager("");
        auto context = Context(scheduler, logger, nullptr, moduleManager);

        const ModulePtr deviceModule(MockDeviceModule_Create(context));
        moduleManager.addModule(deviceModule);

        auto instance = InstanceCustom(context, "local");

        const auto mockDevice = instance.addDevice("mock_phys_device");

        auto streamingServer = std::get<0>(GetParam());
        instance.addServer(streamingServer, nullptr);
        // streaming server added first, so registered device streaming options is published over opcua
        instance.addServer("openDAQ OpcUa", nullptr);

        return instance;
    }

    InstancePtr CreateClientInstance()
    {
        auto instance = Instance();
        auto connectionString = std::get<2>(GetParam());
        if (connectionString.find("daq.opcua") == 0)
        {
            auto config = instance.getAvailableDeviceTypes().get("daq.opcua").createDefaultConfig();
            config.setPropertyValue("AllowedStreamingProtocols", List<IString>("daq.ns", "daq.wss"));
            config.setPropertyValue("PrimaryStreamingProtocol", std::get<1>(GetParam()));
            auto device = instance.addDevice(connectionString, config);
        }
        else
        {
            auto device = instance.addDevice(connectionString);
        }
        return instance;
    }

    InstancePtr serverInstance;
    InstancePtr clientInstance;
};

TEST_P(StreamingTest, SignalDescriptorEvents)
{
    const size_t packetsToGenerate = 5;
    const size_t initialEventPackets = 1;
    const size_t packetsPerChange = 2;  // one triggered by data signal and one trigegred by domain signal
    const size_t packetsToRead = initialEventPackets + packetsToGenerate + (packetsToGenerate - 1) * packetsPerChange;

    auto serverSignal = getSignal(serverInstance, "ChangingSignal");
    auto clientSignal = getSignal(clientInstance, "ChangingSignal");
    auto serverReader = PacketReader(serverSignal);
    auto clientReader = PacketReader(clientSignal);

    generatePackets(packetsToGenerate);

    auto serverReceivedPackets = tryReadPackets(serverReader, packetsToRead);
    auto clientReceivedPackets = tryReadPackets(clientReader, packetsToRead);

    ASSERT_EQ(serverReceivedPackets.getCount(), packetsToRead);
    ASSERT_EQ(clientReceivedPackets.getCount(), packetsToRead);

    // TODO event packets for websocket streaming are not compared
    // websocket streaming does not recreate half assigned data descriptor changed event packet on client side
    // both: value and domain descriptors are always assigned in event packet
    // while on server side one descriptor can be assigned only
    ASSERT_TRUE(packetsEqual(serverReceivedPackets, clientReceivedPackets, std::get<0>(GetParam()) == "openDAQ WebsocketTcp Streaming"));


    // recreate client reader and test initial event packet
    clientReader = PacketReader(clientSignal);
    clientReceivedPackets = tryReadPackets(clientReader, 1);

    ASSERT_EQ(clientReceivedPackets.getCount(), 1);

    EventPacketPtr eventPacket = clientReceivedPackets[0].asPtrOrNull<IEventPacket>();
    ASSERT_TRUE(eventPacket.assigned());
    ASSERT_EQ(eventPacket.getEventId(), event_packet_id::DATA_DESCRIPTOR_CHANGED);

    DataDescriptorPtr dataDescriptor = eventPacket.getParameters().get(event_packet_param::DATA_DESCRIPTOR);
    DataDescriptorPtr domainDescriptor = eventPacket.getParameters().get(event_packet_param::DOMAIN_DATA_DESCRIPTOR);

    ASSERT_TRUE(dataDescriptor.assigned());
    ASSERT_TRUE(domainDescriptor.assigned());

    EXPECT_EQ(dataDescriptor, serverSignal.getDescriptor());
    EXPECT_EQ(dataDescriptor, clientSignal.getDescriptor());
    EXPECT_EQ(domainDescriptor, serverSignal.getDomainSignal().getDescriptor());
    EXPECT_EQ(domainDescriptor, clientSignal.getDomainSignal().getDescriptor());
}

TEST_P(StreamingTest, DataPackets)
{
    const size_t packetsToGenerate = 10;

    // Expect to receive all data packets,
    // +1 signal initial descriptor changed event packet
    const size_t packetsToRead = packetsToGenerate + 1;

    auto serverReader = PacketReader(getSignal(serverInstance, "ByteStep"));
    auto clientReader = PacketReader(getSignal(clientInstance, "ByteStep"));

    generatePackets(packetsToGenerate);

    auto serverReceivedPackets = tryReadPackets(serverReader, packetsToRead);
    auto clientReceivedPackets = tryReadPackets(clientReader, packetsToRead);

    ASSERT_EQ(serverReceivedPackets.getCount(), packetsToRead);
    ASSERT_EQ(clientReceivedPackets.getCount(), packetsToRead);
    ASSERT_TRUE(packetsEqual(serverReceivedPackets, clientReceivedPackets));
}

TEST_P(StreamingTest, SignalPropertyEvents)
{
    const size_t packetsToRead = 10;

    // Expect to receive all data packets,
    // +1 signal initial descriptor changed event packet
    // +2 signal property changed event packet
    const size_t packetsToReceive = packetsToRead + 1 + 2;

    SignalConfigPtr serverSignal = getSignal(serverInstance, "ByteStep");
    SignalPtr clientSignal = getSignal(clientInstance, "ByteStep");

    ASSERT_EQ(clientSignal.getName(), serverSignal.getName());
    ASSERT_EQ(clientSignal.getDescription(), serverSignal.getDescription());

    auto serverReader = PacketReader(serverSignal);
    auto clientReader = PacketReader(clientSignal);

    serverSignal.setName("ByteStepChanged");
    serverSignal.setDescription("DescriptionChanged");

    generatePackets(packetsToRead);

    auto serverReceivedPackets = tryReadPackets(serverReader, packetsToReceive);
    auto clientReceivedPackets = tryReadPackets(clientReader, packetsToReceive);

    ASSERT_EQ(clientSignal.getName(), serverSignal.getName());
    ASSERT_EQ(clientSignal.getDescription(), serverSignal.getDescription());

    // TODO
    // packet comparing for web-socket streaming is skipped since it does not recreate "PROPERTY_CHANGED"
    // event packet but create "DATA_DESCRIPTOR_CHANGED" event packet instead on client side
    // when property changed on server side
    if (std::get<0>(GetParam()) == "openDAQ WebsocketTcp Streaming")
        return;

    ASSERT_EQ(serverReceivedPackets.getCount(), packetsToReceive);
    ASSERT_EQ(clientReceivedPackets.getCount(), packetsToReceive);
    ASSERT_TRUE(packetsEqual(serverReceivedPackets, clientReceivedPackets));
}

#if defined(OPENDAQ_ENABLE_NATIVE_STREAMING) && defined(OPENDAQ_ENABLE_WEBSOCKET_STREAMING)
INSTANTIATE_TEST_SUITE_P(
    StreamingTestGroup,
    StreamingTest,
    testing::Values(
        std::make_tuple("openDAQ Native Streaming", "daq.ns", "daq.nsd://127.0.0.1/"),
        std::make_tuple("openDAQ Native Streaming", "daq.ns", "daq.opcua://127.0.0.1/"),
        std::make_tuple("openDAQ WebsocketTcp Streaming", "daq.wss", "daq.ws://127.0.0.1/"),
        std::make_tuple("openDAQ WebsocketTcp Streaming", "daq.wss", "daq.opcua://127.0.0.1/")
    )
);
#elif defined(OPENDAQ_ENABLE_NATIVE_STREAMING) && !defined(OPENDAQ_ENABLE_WEBSOCKET_STREAMING)
INSTANTIATE_TEST_SUITE_P(
    StreamingTestGroup,
    StreamingTest,
    testing::Values(
        std::make_tuple("openDAQ Native Streaming", "daq.ns", "daq.nsd://127.0.0.1/"),
        std::make_tuple("openDAQ Native Streaming", "daq.ns", "daq.opcua://127.0.0.1/")
    )
);
#elif !defined(OPENDAQ_ENABLE_NATIVE_STREAMING) && defined(OPENDAQ_ENABLE_WEBSOCKET_STREAMING)
INSTANTIATE_TEST_SUITE_P(
    StreamingTestGroup,
    StreamingTest,
    testing::Values(
        std::make_tuple("openDAQ WebsocketTcp Streaming", "daq.wss", "daq.ws://127.0.0.1/"),
        std::make_tuple("openDAQ WebsocketTcp Streaming", "daq.wss", "daq.opcua://127.0.0.1/")
    )
);
#endif

#if defined(OPENDAQ_ENABLE_NATIVE_STREAMING)
class StreamingAsyncSignalTest : public StreamingTest
{
protected:
    InstancePtr CreateServerInstance() override
    {
        auto logger = Logger();
        auto scheduler = Scheduler(logger);
        auto moduleManager = ModuleManager("");
        auto context = Context(scheduler, logger, nullptr, moduleManager);

        const ModulePtr deviceModule(MockDeviceModule_Create(context));
        moduleManager.addModule(deviceModule);

        auto instance = InstanceCustom(context, "local");

        const auto mockDevice = instance.addDevice("mock_phys_device");

        const auto statisticsFb = instance.addFunctionBlock("ref_fb_module_statistics");
        statisticsFb.setPropertyValue("DomainSignalType", 1);  // 1 - Explicit
        statisticsFb.getInputPorts()[0].connect(getSignal(instance, "ByteStep"));

        auto streamingServer = std::get<0>(GetParam());
        instance.addServer(streamingServer, nullptr);
        // streaming server added first, so registered device streaming options is published over opcua
        instance.addServer("openDAQ OpcUa", nullptr);

        return instance;
    }
};

TEST_P(StreamingAsyncSignalTest, SigWithExplicitDomain)
{
    const size_t packetsToRead = 10;

    auto serverReader = PacketReader(getSignal(serverInstance, "ByteStep/Avg"));
    auto clientReader = PacketReader(getSignal(clientInstance, "ByteStep/Avg"));

    generatePackets(packetsToRead);

    auto serverReceivedPackets = tryReadPackets(serverReader, packetsToRead + 1);
    auto clientReceivedPackets = tryReadPackets(clientReader, packetsToRead + 1);

    ASSERT_EQ(serverReceivedPackets.getCount(), packetsToRead + 1);
    ASSERT_EQ(clientReceivedPackets.getCount(), packetsToRead + 1);
    ASSERT_TRUE(packetsEqual(serverReceivedPackets, clientReceivedPackets));
}

INSTANTIATE_TEST_SUITE_P(
    StreamingAsyncSignalTestGroup,
    StreamingAsyncSignalTest,
    testing::Values(
        std::make_tuple("openDAQ Native Streaming", "daq.ns", "daq.nsd://127.0.0.1/"),
        std::make_tuple("openDAQ Native Streaming", "daq.ns", "daq.opcua://127.0.0.1/")
    )
);
#endif
