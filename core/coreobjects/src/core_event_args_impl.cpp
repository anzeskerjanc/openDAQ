#include <coreobjects/core_event_args_impl.h>
#include <coreobjects/property_ptr.h>

BEGIN_NAMESPACE_OPENDAQ

OPENDAQ_DEFINE_CLASS_FACTORY(LIBRARY_FACTORY, CoreEventArgs, Int, eventId, IDict*, parameters)

#if !defined(BUILDING_STATIC_LIBRARY)

extern "C"
ErrCode PUBLIC_EXPORT createCoreEventArgsPropertyValueChanged(ICoreEventArgs** objTmp, IString* propName, IBaseObject* value)
{
    const auto dict = Dict<IString, IBaseObject>({{"Name", propName}, {"Value", value}});
    return daq::createObject<ICoreEventArgs, CoreEventArgsImpl, Int, IDict*>(objTmp, core_event_ids::PropertyValueChanged,dict);
}

extern "C"
ErrCode PUBLIC_EXPORT createCoreEventArgsUpdateEnd(ICoreEventArgs** objTmp, IDict* updatedProperties)
{
    const auto dict = Dict<IString, IBaseObject>({{"UpdatedProperties", updatedProperties}});
    return daq::createObject<ICoreEventArgs, CoreEventArgsImpl, Int, IDict*>(objTmp, core_event_ids::UpdateEnd,dict);
}

extern "C"
ErrCode PUBLIC_EXPORT createCoreEventArgsPropertyAdded(ICoreEventArgs** objTmp, IProperty* prop)
{
    const auto dict = Dict<IString, IBaseObject>({{"Property", prop}});
    return daq::createObject<ICoreEventArgs, CoreEventArgsImpl, Int, IDict*>(objTmp, core_event_ids::PropertyAdded,dict);
}

extern "C"
ErrCode PUBLIC_EXPORT createCoreEventArgsPropertyRemoved(ICoreEventArgs** objTmp, IString* propName)
{
    const auto dict = Dict<IString, IBaseObject>({{"Name", propName}});
    return daq::createObject<ICoreEventArgs, CoreEventArgsImpl, Int, IDict*>(objTmp, core_event_ids::PropertyRemoved,dict);
}

#endif

END_NAMESPACE_OPENDAQ
