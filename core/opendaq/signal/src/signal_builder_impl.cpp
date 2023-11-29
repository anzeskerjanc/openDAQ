#include <opendaq/signal_builder_impl.h>
#include <opendaq/signal_factory.h>
#include <coretypes/validation.h>
#include <opendaq/tags_factory.h>

BEGIN_NAMESPACE_OPENDAQ
    SignalBuilderImpl::SignalBuilderImpl()
    : componentStandardProps(ComponentStandardProps::Add)
    , tags(Tags())
    , visible(true)
    , active(true)
    , _public(true)
    , relatedSignals(List<ISignal>())
    , className("")
{
}

SignalBuilderImpl::SignalBuilderImpl(const ContextPtr& context,
                                     const ComponentPtr& parent,
                                     const StringPtr& localId)
    : SignalBuilderImpl()
{
    this->context = context;
    this->parent = parent;
    this->localId = localId;
    this->name = localId;
}

ErrCode SignalBuilderImpl::build(ISignal** signal)
{
    OPENDAQ_PARAM_NOT_NULL(signal);

    return daqTry([&]
    {
        *signal = Signal(this->borrowPtr<SignalBuilderPtr>()).detach();
        return OPENDAQ_SUCCESS;
    });;
}

ErrCode SignalBuilderImpl::getLocalId(IString** localId)
{
    OPENDAQ_PARAM_NOT_NULL(localId);

    *localId = this->localId.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setLocalId(IString* localId)
{
    this->localId = localId;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getParent(IComponent** parent)
{
    OPENDAQ_PARAM_NOT_NULL(parent);
    
    *parent = this->parent.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setParent(IComponent* parent)
{
    this->parent = parent;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getContext(IContext** context)
{
    OPENDAQ_PARAM_NOT_NULL(context);
    
    *context = this->context.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setContext(IContext* context)
{
    this->context = context;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getDescriptor(IDataDescriptor** descriptor)
{
    OPENDAQ_PARAM_NOT_NULL(descriptor);
    
    *descriptor = this->dataDescriptor.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setDescriptor(IDataDescriptor* descriptor)
{
    this->dataDescriptor = descriptor;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getDomainSignal(ISignal** signal)
{
    OPENDAQ_PARAM_NOT_NULL(signal);
    
    *signal = this->domainSignal.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setDomainSignal(ISignal* signal)
{
    this->domainSignal = signal;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getComponentStandardPropsMode(ComponentStandardProps* standardProps)
{
    OPENDAQ_PARAM_NOT_NULL(standardProps);
    
    *standardProps = this->componentStandardProps;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setComponentStandardPropsMode(ComponentStandardProps standardProps)
{
    this->componentStandardProps = standardProps;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getName(IString** name)
{
    OPENDAQ_PARAM_NOT_NULL(name);
    
    *name = this->name.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setName(IString* name)
{
    this->name = name;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getDescription(IString** description)
{
    OPENDAQ_PARAM_NOT_NULL(description);
    
    *description = this->description.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setDescription(IString* description)
{
    this->description = description;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getTags(ITagsConfig** tags)
{
    OPENDAQ_PARAM_NOT_NULL(tags);

    *tags = this->tags.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setTags(ITagsConfig* tags)
{
    this->tags = tags;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getVisible(Bool* visible)
{
    OPENDAQ_PARAM_NOT_NULL(visible);
    
    *visible = this->visible;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setVisible(Bool visible)
{
    this->visible = visible;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getActive(Bool* active)
{
    OPENDAQ_PARAM_NOT_NULL(active);

    *active = this->active;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setActive(Bool active)
{
    this->active = active;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getPublic(Bool* isPublic)
{
    OPENDAQ_PARAM_NOT_NULL(isPublic);

    *isPublic = this->_public;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setPublic(Bool isPublic)
{
    this->_public = isPublic;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getRelatedSignals(IList** signals)
{
    OPENDAQ_PARAM_NOT_NULL(signals);

    *signals = this->relatedSignals.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setRelatedSignals(IList* signals)
{
    this->relatedSignals = signals;
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::getClassName(IString** className)
{
    OPENDAQ_PARAM_NOT_NULL(className);

    *className = this->className.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SignalBuilderImpl::setClassName(IString* className)
{
    this->className = className;
    return OPENDAQ_SUCCESS;
}

OPENDAQ_DEFINE_CLASS_FACTORY(
    LIBRARY_FACTORY, SignalBuilder,
    IContext*, context,
    IComponent*, parent,
    IString*, localId
)

OPENDAQ_DEFINE_CLASS_FACTORY_WITH_INTERFACE_AND_CREATEFUNC(
    LIBRARY_FACTORY, SignalBuilder, ISignalBuilder, createSignalBuilderEmpty)

END_NAMESPACE_OPENDAQ
