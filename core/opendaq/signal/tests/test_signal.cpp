#include <opendaq/signal_exceptions.h>
#include <opendaq/signal_factory.h>
#include <opendaq/data_descriptor_factory.h>
#include <opendaq/signal_events.h>
#include <opendaq/signal_private_ptr.h>
#include <opendaq/context_factory.h>
#include <opendaq/removable_ptr.h>
#include <coreobjects/property_object_class_factory.h>
#include <gtest/gtest.h>

using SignalTest = testing::Test;

BEGIN_NAMESPACE_OPENDAQ

class ConnectionMockImpl: public ImplementationOf<IConnection>
{
public:
    bool packetEnqueued{ false };

    ErrCode INTERFACE_FUNC enqueue(IPacket* packet) override
    {
        packetEnqueued = true;
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC enqueueOnThisThread(IPacket* packet) override
    {
        packetEnqueued = true;
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC dequeue(IPacket** packet) override
    {
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC peek(IPacket** packet) override
    {
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC getPacketCount(SizeT* packetCount) override
    {
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC getSignal(ISignal** signal) override
    {
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC getInputPort(IInputPort** inputPort) override
    {
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC getAvailableSamples(SizeT* samples) override
    {
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC getSamplesUntilNextDescriptor(SizeT* samples) override
    {
        return OPENDAQ_SUCCESS;
    }
};

class PacketMockImpl : public ImplementationOf<IPacket>
{
public:
    ErrCode INTERFACE_FUNC getType(PacketType* type) override
    {
        *type = PacketType::Data;
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC subscribeForDestructNotification(IPacketDestructCallback* packetDestructCallback) override
    {
        return OPENDAQ_ERR_NOTIMPLEMENTED;
    }

    ErrCode INTERFACE_FUNC getRefCount(SizeT* refCount) override
    {
        return OPENDAQ_ERR_NOTIMPLEMENTED;
    }
};

ConnectionPtr ConnectionMock()
{
    IConnection* intf;
    checkErrorInfo(createObject<IConnection, ConnectionMockImpl>(&intf));
    return intf;
}

PacketPtr PacketMock()
{
    IPacket* intf;
    checkErrorInfo(createObject<IPacket, PacketMockImpl>(&intf));
    return intf;
}

class DataDescriptorMockImpl: public ImplementationOf<IDataDescriptor>
{
public:

    ErrCode INTERFACE_FUNC getName(IString** name) override
    {
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC getDimensions(IList** dimensions) override
    {
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC getSampleType(SampleType* sampleType) override
    {
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC getUnit(IUnit** unit) override
    {
        return OPENDAQ_SUCCESS;
    }

    virtual ErrCode INTERFACE_FUNC getValueRange(IRange** range) override
    {
        return OPENDAQ_SUCCESS;
    }

    virtual ErrCode INTERFACE_FUNC getRule(IDataRule** rule) override
    {
        return OPENDAQ_SUCCESS;
    }

    virtual ErrCode INTERFACE_FUNC getOrigin(IString** origin) override
    {
        return OPENDAQ_SUCCESS;
    }

    virtual ErrCode INTERFACE_FUNC getTickResolution(IRatio** tickResolution) override
    {
        return OPENDAQ_SUCCESS;
    }

    virtual ErrCode INTERFACE_FUNC getPostScaling(IScaling** scaling) override
    {
        return OPENDAQ_SUCCESS;
    }

    virtual ErrCode INTERFACE_FUNC getStructFields(IList** structFields) override
    {
        return OPENDAQ_SUCCESS;
    }

    ErrCode INTERFACE_FUNC getMetadata(IDict** metadata) override
    {
        return OPENDAQ_SUCCESS;
    }
};

TEST_F(SignalTest, IsComponent)
{
    auto signal = Signal(NullContext(), nullptr, "sig");
    ASSERT_NO_THROW(signal.asPtr<IComponent>());
}

TEST_F(SignalTest, SignalConnections)
{
    const auto signal = Signal(NullContext(), nullptr, "sig");
    auto connections = signal.getConnections();
    ASSERT_EQ(connections.getCount(), 0u);

    auto conn1 = ConnectionMock();
    checkErrorInfo(signal.asPtr<ISignalEvents>()->listenerConnected(conn1));
    connections = signal.getConnections();
    ASSERT_EQ(connections.getCount(), 1u);

    auto conn2 = ConnectionMock();
    checkErrorInfo(signal.asPtr<ISignalEvents>()->listenerConnected(conn2));
    connections = signal.getConnections();
    ASSERT_EQ(connections.getCount(), 2u);

    ASSERT_THROW(checkErrorInfo(signal.asPtr<ISignalEvents>()->listenerConnected(conn2)), DuplicateItemException);
    
    checkErrorInfo(signal.asPtr<ISignalEvents>()->listenerDisconnected(conn2));
    connections = signal.getConnections();
    ASSERT_EQ(connections.getCount(), 1u);

    ASSERT_THROW(checkErrorInfo(signal.asPtr<ISignalEvents>()->listenerDisconnected(conn2)), NotFoundException);
}

TEST_F(SignalTest, RelatedSignals)
{
    const auto signal = Signal(NullContext(), nullptr, "sig");
    auto relatedSignals = signal.getRelatedSignals();
    ASSERT_EQ(relatedSignals.getCount(), 0u);

    auto signal1 = Signal(NullContext(), nullptr, "sig");
    signal.addRelatedSignal(signal1);
    relatedSignals = signal.getRelatedSignals();
    ASSERT_EQ(relatedSignals.getCount(), 1u);
    ASSERT_EQ(relatedSignals[0], signal1);

    auto signal2 = Signal(NullContext(), nullptr, "sig");
    signal.addRelatedSignal(signal2);
    relatedSignals = signal.getRelatedSignals();
    ASSERT_EQ(relatedSignals.getCount(), 2u);

    ASSERT_THROW(signal.addRelatedSignal(signal2), DuplicateItemException);

    signal.removeRelatedSignal(signal2);
    relatedSignals = signal.getRelatedSignals();
    ASSERT_EQ(relatedSignals.getCount(), 1u);

    ASSERT_THROW(signal.removeRelatedSignal(signal2), NotFoundException);

    signal.clearRelatedSignals();
    relatedSignals = signal.getRelatedSignals();
    ASSERT_EQ(relatedSignals.getCount(), 0u);
}

TEST_F(SignalTest, EmptyClassName)
{
    auto signal = Signal(NullContext(), nullptr, "sig");
    ASSERT_EQ(signal.getClassName(), "");
}

// TODO: reintroduce className parameter when duck-typing is limited.

/*
TEST_F(SignalTest, ClassName)
{
    const auto context = Context(nullptr, Logger(), TypeManager(), nullptr);
    auto rangeItemClass = PropertyObjectClass("TestClass");
    context.getTypeManager().addType(rangeItemClass);

    auto signal = Signal(context, nullptr, "sig", "TestClass");
    ASSERT_EQ(signal.getClassName(), "TestClass");
}
*/

TEST_F(SignalTest, DomainSignal)
{
    const auto signal = Signal(NullContext(), nullptr, "sig");
    ASSERT_FALSE(signal.getDomainSignal().assigned());

    auto signal1 = Signal(NullContext(), nullptr, "sig");
    signal.setDomainSignal(signal1);
    ASSERT_EQ(signal.getDomainSignal(), signal1);

    signal.setDomainSignal(nullptr);
    ASSERT_FALSE(signal.getDomainSignal().assigned());
}

TEST_F(SignalTest, Active)
{
    const auto signal = Signal(NullContext(), nullptr, "sig");
    ASSERT_TRUE(signal.getActive());

    signal.setActive(False);
    ASSERT_FALSE(signal.getActive());
}

TEST_F(SignalTest, Public)
{
    const auto signal = Signal(NullContext(), nullptr, "sig");
    ASSERT_TRUE(signal.getPublic());

    signal.setPublic(False);
    ASSERT_FALSE(signal.getPublic());
}

TEST_F(SignalTest, Name)
{
    const auto signal = Signal(NullContext(), nullptr, "sig");
    ASSERT_EQ(signal.getName(), "sig");

    signal.setName("Signal");
    ASSERT_EQ(signal.getName(), "Signal");
}

TEST_F(SignalTest, Descriptor)
{
    const auto signal = Signal(NullContext(), nullptr, "sig");
    ASSERT_EQ(signal.getDescription(), "");

    signal.setDescription("Description");
    ASSERT_EQ(signal.getDescription(), "Description");
}

TEST_F(SignalTest, SignalDescriptor)
{
    auto dataDescriptorImpl = new DataDescriptorMockImpl();
    ObjectPtr<IDataDescriptor> dataDescriptor;
    checkErrorInfo(dataDescriptorImpl->queryInterface(IDataDescriptor::Id, reinterpret_cast<void**>(&dataDescriptor)));

    const auto signal = Signal(NullContext(), nullptr, "sig");
    ASSERT_FALSE(signal.getDescriptor().assigned());

    signal.setDescriptor(dataDescriptor);
    ASSERT_EQ(signal.getDescriptor(), dataDescriptor);
}

TEST_F(SignalTest, SendNullPacket)
{
    const auto signal = Signal(NullContext(), nullptr, "sig");
    ASSERT_THROW(signal.sendPacket(nullptr), ArgumentNullException);
}

TEST_F(SignalTest, SendPacket)
{
    const auto signal = Signal(NullContext(), nullptr, "sig");

    auto connImpl = new ConnectionMockImpl();
    ConnectionPtr conn;
    checkErrorInfo(connImpl->queryInterface(IConnection::Id, reinterpret_cast<void**>(&conn)));

    signal.asPtr<ISignalEvents>()->listenerConnected(conn);

    auto packet = PacketMock();
    signal.sendPacket(packet);

    ASSERT_TRUE(connImpl->packetEnqueued);
}

TEST_F(SignalTest, SetDescriptorWithConnection)
{
    const auto signal = Signal(NullContext(), nullptr, "sig");

    auto connImpl = new ConnectionMockImpl();
    ConnectionPtr conn;
    checkErrorInfo(connImpl->queryInterface(IConnection::Id, reinterpret_cast<void**>(&conn)));

    signal.asPtr<ISignalEvents>()->listenerConnected(conn);

    auto desc = DataDescriptorBuilder().setSampleType(SampleType::Float64).build();
    signal.setDescriptor(desc);

    ASSERT_TRUE(connImpl->packetEnqueued);
}

TEST_F(SignalTest, SetDomainDescriptorWithConnection)
{
    const auto signal = Signal(NullContext(), nullptr, "sig");
    const auto domainSignal = Signal(NullContext(), nullptr, "sig");
    signal.setDomainSignal(domainSignal);

    auto connImpl = new ConnectionMockImpl();
    ConnectionPtr conn;
    checkErrorInfo(connImpl->queryInterface(IConnection::Id, reinterpret_cast<void**>(&conn)));

    signal.asPtr<ISignalEvents>()->listenerConnected(conn);

    auto desc = DataDescriptorBuilder().setSampleType(SampleType::Float64).build();
    domainSignal.setDescriptor(desc);

    ASSERT_TRUE(connImpl->packetEnqueued);
}


TEST_F(SignalTest, SetSharedDomainDescriptor)
{
    auto signal1 = Signal(NullContext(), nullptr, "sig");
    {
        auto signal2 = Signal(NullContext(), nullptr, "sig");
        {
            auto signal3 = Signal(NullContext(), nullptr, "sig");
            auto domainSignal = Signal(NullContext(), nullptr, "sig");

            signal3.setDomainSignal(domainSignal);
            signal2.setDomainSignal(domainSignal);
            signal1.setDomainSignal(domainSignal);
        }
    }
}

TEST_F(SignalTest, Remove)
{
    auto signal = Signal(NullContext(), nullptr, "sig");

    auto removable = signal.asPtr<IRemovable>(true);
    ASSERT_FALSE(removable.isRemoved());

    removable.remove();

    ASSERT_TRUE(removable.isRemoved());
}

TEST_F(SignalTest, StreamingSourcesEmpty)
{
    auto signal = Signal(NullContext(), nullptr, "sig");

    auto sources = signal.getStreamingSources();
    ASSERT_EQ(sources.getCount(), 0u);

    auto activeSource = signal.getActiveStreamingSource();
    ASSERT_FALSE(activeSource.assigned());

    ASSERT_THROW(signal.setActiveStreamingSource("connectionString"), NotFoundException);
}

END_NAMESPACE_OPENDAQ
