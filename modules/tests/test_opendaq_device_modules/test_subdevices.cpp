#include <testutils/testutils.h>
#include <opendaq/opendaq.h>
#include "testutils/memcheck_listener.h"

#include <thread>

// MAC CI issue
#if !defined(SKIP_TEST_MAC_CI)
#if defined(__clang__) && !defined(__RESHARPER__)
#define SKIP_TEST_MAC_CI return
#else
#define SKIP_TEST_MAC_CI
#endif
#endif
using namespace daq;

enum class StreamingType
{
    WebsocketStreaming = 0,
    NativeStreaming
};

// first param: leaf device streaming type
// second param: gateway device streaming type
class SubDevicesTest : public testing::TestWithParam<std::pair<StreamingType, StreamingType>>
{
public:
    const uint16_t NATIVE_STREAMING_PORT = 7420;
    const uint16_t WEBSOCKET_STREAMING_PORT = 7414;
    const uint16_t OPCUA_PORT = 4840;
    const uint16_t WEBSOCKET_CONTROL_PORT = 7438;

    const uint16_t MIN_CONNECTIONS = 0;
    const uint16_t MIN_HOPS = 1;

    StringPtr createConnectionString(uint16_t port)
    {
        return String(fmt::format("daq.opcua://127.0.0.1:{}/", port));
    }

    InstancePtr CreateSubdeviceInstance(uint16_t index)
    {
        auto logger = Logger();
        auto scheduler = Scheduler(logger);
        auto moduleManager = ModuleManager("");
        auto typeManager = TypeManager();
        auto context = Context(scheduler, logger, typeManager, moduleManager);
        auto instance = InstanceCustom(context, fmt::format("subdevice{}", index));
        const auto refDevice = instance.addDevice("daqref://device0");

        auto subdeviceStreamingType = GetParam().first;

        if (subdeviceStreamingType == StreamingType::WebsocketStreaming)
        {
            auto ws_config = instance.getAvailableServerTypes().get("openDAQ WebsocketTcp Streaming").createDefaultConfig();
            ws_config.setPropertyValue("WebsocketStreamingPort", WEBSOCKET_STREAMING_PORT + index);
            ws_config.setPropertyValue("WebsocketControlPort", WEBSOCKET_CONTROL_PORT + index);
            instance.addServer("openDAQ WebsocketTcp Streaming", ws_config);
        }
        else if (subdeviceStreamingType == StreamingType::NativeStreaming)
        {
            auto ns_config = instance.getAvailableServerTypes().get("openDAQ Native Streaming").createDefaultConfig();
            ns_config.setPropertyValue("NativeStreamingPort", NATIVE_STREAMING_PORT + index);
            instance.addServer("openDAQ Native Streaming", ns_config);
        }

        auto ua_config = instance.getAvailableServerTypes().get("openDAQ OpcUa").createDefaultConfig();
        ua_config.setPropertyValue("Port", OPCUA_PORT + index);
        instance.addServer("openDAQ OpcUa", ua_config);

        return instance;
    }

    InstancePtr CreateGatewayInstance()
    {
        auto logger = Logger();
        auto scheduler = Scheduler(logger);
        auto moduleManager = ModuleManager("");
        auto typeManager = TypeManager();
        auto context = Context(scheduler, logger, typeManager, moduleManager);

        auto instance = InstanceCustom(context, "gateway");

        auto subdeviceStreamingType = GetParam().first;
        for (auto index = 1; index <= 2; index++)
        {
            auto config = instance.getAvailableDeviceTypes().get("daq.opcua").createDefaultConfig();
            config.setPropertyValue("StreamingConnectionHeuristic", MIN_CONNECTIONS);
            config.setPropertyValue("AllowedStreamingProtocols", List<IString>("daq.ns", "daq.wss"));
            if (subdeviceStreamingType == StreamingType::WebsocketStreaming)
                config.setPropertyValue("PrimaryStreamingProtocol", "daq.wss");
            else if (subdeviceStreamingType == StreamingType::NativeStreaming)
                config.setPropertyValue("PrimaryStreamingProtocol", "daq.ns");
            const auto subDevice = instance.addDevice(createConnectionString(OPCUA_PORT+index), config);
        }

        auto gatewayStreamingType = GetParam().second;

        if (gatewayStreamingType == StreamingType::WebsocketStreaming)
        {
            auto ws_config = instance.getAvailableServerTypes().get("openDAQ WebsocketTcp Streaming").createDefaultConfig();
            ws_config.setPropertyValue("WebsocketStreamingPort", WEBSOCKET_STREAMING_PORT);
            ws_config.setPropertyValue("WebsocketControlPort", WEBSOCKET_CONTROL_PORT);
            instance.addServer("openDAQ WebsocketTcp Streaming", ws_config);
        }
        else if (gatewayStreamingType == StreamingType::NativeStreaming)
        {
            auto ns_config = instance.getAvailableServerTypes().get("openDAQ Native Streaming").createDefaultConfig();
            ns_config.setPropertyValue("NativeStreamingPort", NATIVE_STREAMING_PORT);
            instance.addServer("openDAQ Native Streaming", ns_config);
        }

        auto ua_config = instance.getAvailableServerTypes().get("openDAQ OpcUa").createDefaultConfig();
        ua_config.setPropertyValue("Port", OPCUA_PORT);
        instance.addServer("openDAQ OpcUa", ua_config);

        return instance;
    }

    InstancePtr CreateClientInstance(const IntegerPtr& heuristicValue)
    {
        auto logger = Logger();
        auto scheduler = Scheduler(logger);
        auto moduleManager = ModuleManager("");
        auto typeManager = TypeManager();
        auto context = Context(scheduler, logger, typeManager, moduleManager);
        auto instance = InstanceCustom(context, "client");

        auto clientStreamingType = (heuristicValue == MIN_HOPS) ?  GetParam().first : GetParam().second;
        auto config = instance.getAvailableDeviceTypes().get("daq.opcua").createDefaultConfig();
        config.setPropertyValue("StreamingConnectionHeuristic", heuristicValue);
        config.setPropertyValue("AllowedStreamingProtocols", List<IString>("daq.ns", "daq.wss"));
        if (clientStreamingType == StreamingType::WebsocketStreaming)
            config.setPropertyValue("PrimaryStreamingProtocol", "daq.wss");
        else if (clientStreamingType == StreamingType::NativeStreaming)
            config.setPropertyValue("PrimaryStreamingProtocol", "daq.ns");
        auto refDevice = instance.addDevice(createConnectionString(OPCUA_PORT), config);

        return instance;
    }
};

TEST_P(SubDevicesTest, RootStreamingActive)
{
    SKIP_TEST_MAC_CI;
    auto subdevice1 = CreateSubdeviceInstance(1u);
    auto subdevice2 = CreateSubdeviceInstance(2u);
    auto gateway = CreateGatewayInstance();
    auto client = CreateClientInstance(MIN_CONNECTIONS);

    auto clientSignals = client.getSignals(SearchParams(true, true));
    auto gatewaySignals = gateway.getSignals(SearchParams(true, true));
    ASSERT_EQ(clientSignals.getCount(), gatewaySignals.getCount());

    for (size_t index = 0; index < clientSignals.getCount(); ++index)
    {
        auto mirroredSignalConfigPtr = clientSignals[index].template asPtr<IMirroredSignalConfig>();
        ASSERT_EQ(mirroredSignalConfigPtr.getStreamingSources().getCount(), 1u);
        ASSERT_TRUE(mirroredSignalConfigPtr.getActiveStreamingSource().assigned());
        ASSERT_NE(mirroredSignalConfigPtr.getActiveStreamingSource(),
                  gatewaySignals[index].template asPtr<IMirroredSignalConfig>().getActiveStreamingSource());
    }

    using namespace std::chrono_literals;
    StreamReaderPtr reader = daq::StreamReader<double, uint64_t>(client.getSignals(SearchParams(true, true))[0]);

    std::this_thread::sleep_for(1000ms);
    double samples[100];
    for (int i = 0; i < 10; ++i)
    {
        std::this_thread::sleep_for(100ms);
        daq::SizeT count = 100;
        reader.read(samples, &count);
        EXPECT_GT(count, 0) << "iteration " << i;
    }
}

TEST_P(SubDevicesTest, LeafStreamingsActive)
{
    SKIP_TEST_MAC_CI;
    auto subdevice1 = CreateSubdeviceInstance(1u);
    auto subdevice2 = CreateSubdeviceInstance(2u);
    auto gateway = CreateGatewayInstance();
    auto client = CreateClientInstance(MIN_HOPS);

    auto clientSignals = client.getSignals(SearchParams(true, true));
    auto gatewaySignals = gateway.getSignals(SearchParams(true, true));
    ASSERT_EQ(clientSignals.getCount(), gatewaySignals.getCount());

    for (size_t index = 0; index < clientSignals.getCount(); ++index)
    {
        auto mirroredSignalConfigPtr = clientSignals[index].template asPtr<IMirroredSignalConfig>();
        ASSERT_EQ(mirroredSignalConfigPtr.getStreamingSources().getCount(), 2u);
        ASSERT_TRUE(mirroredSignalConfigPtr.getActiveStreamingSource().assigned());

        ASSERT_EQ(mirroredSignalConfigPtr.getActiveStreamingSource(),
                  gatewaySignals[index].template asPtr<IMirroredSignalConfig>().getActiveStreamingSource());
    }

    using namespace std::chrono_literals;
    StreamReaderPtr reader = daq::StreamReader<double, uint64_t>(client.getSignals(SearchParams(true, true))[0]);
    
    std::this_thread::sleep_for(1000ms);
    double samples[100];
    for (int i = 0; i < 10; ++i)
    {
        std::this_thread::sleep_for(100ms);
        daq::SizeT count = 100;
        reader.read(samples, &count);
        EXPECT_GT(count, 0) << "iteration " << i;
    }
}

#if defined(OPENDAQ_ENABLE_NATIVE_STREAMING) && !defined(OPENDAQ_ENABLE_WEBSOCKET_STREAMING)
INSTANTIATE_TEST_SUITE_P(
    SubDevicesTestGroup,
    SubDevicesTest,
    testing::Values(
        std::pair(StreamingType::NativeStreaming, StreamingType::NativeStreaming)
    )
);
#elif !defined(OPENDAQ_ENABLE_NATIVE_STREAMING) && defined(OPENDAQ_ENABLE_WEBSOCKET_STREAMING)
INSTANTIATE_TEST_SUITE_P(
    SubDevicesTestGroup,
    SubDevicesTest,
    testing::Values(
        std::pair(StreamingType::WebsocketStreaming, StreamingType::WebsocketStreaming)
    )
);
#elif defined(OPENDAQ_ENABLE_NATIVE_STREAMING) && defined(OPENDAQ_ENABLE_WEBSOCKET_STREAMING)
INSTANTIATE_TEST_SUITE_P(
    SubDevicesTestGroup,
    SubDevicesTest,
    testing::Values(
        std::pair(StreamingType::NativeStreaming, StreamingType::NativeStreaming),
        std::pair(StreamingType::WebsocketStreaming, StreamingType::WebsocketStreaming),
        std::pair(StreamingType::NativeStreaming, StreamingType::WebsocketStreaming)
        /// note: next one does not work because websocket streaming does not stream domain signal packets
        /// if subdevice has enabled websocket streaming - the gateway device will not be able to stream-forward
        /// signals thru native streaming which requires domain packets to be streamed explicitly
        // std::pair(StreamingType::WebsocketStreaming, StreamingType::NativeStreaming)
    )
);
#endif
