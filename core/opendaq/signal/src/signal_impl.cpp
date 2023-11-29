#include <opendaq/signal_impl.h>

BEGIN_NAMESPACE_OPENDAQ

#if !defined(BUILDING_STATIC_LIBRARY)

extern "C"
daq::ErrCode PUBLIC_EXPORT createSignalFromBuilder(ISignal** objTmp, ISignalBuilder* builder)
{
	return daq::createObject<ISignal, SignalImpl, ISignalBuilder*>(objTmp, builder);
}

extern "C"
daq::ErrCode PUBLIC_EXPORT createSignal(
    ISignalConfig** objTmp, IContext* context,
    IComponent* parent, IString* localId, 
    IString* className)
{
	return daq::createObject<ISignalConfig, SignalImpl, IContext*, IComponent*, IString*, IString*, ComponentStandardProps>(
		objTmp, context, parent, localId, className, ComponentStandardProps::Add);
}

using SignalWithDescriptorImpl = SignalImpl;

extern "C"
daq::ErrCode PUBLIC_EXPORT createSignalWithDescriptor(
	ISignalConfig** objTmp, IContext* context, 
	IDataDescriptor* descriptor, IComponent* parent, 
	IString* localId, IString* className)
{
	return daq::createObject<ISignalConfig, SignalWithDescriptorImpl, IContext*, IDataDescriptor*, IComponent*, IString*, IString*, ComponentStandardProps>(
		objTmp, context, descriptor, parent, localId, className, ComponentStandardProps::Add);
}

#endif

END_NAMESPACE_OPENDAQ
