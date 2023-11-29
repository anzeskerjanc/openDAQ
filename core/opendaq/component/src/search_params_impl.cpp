#include <opendaq/search_params_impl.h>
#include <opendaq/component.h>
#include <coretypes/validation.h>
#include <coretypes/impl.h>

BEGIN_NAMESPACE_OPENDAQ

SearchParamsImpl::SearchParamsImpl(const SearchParamsBuilderPtr& builder)
    : recursive(builder.getRecursive())
    , visibleOnly(builder.getVisibleOnly())
    , requiredTags(builder.getRequiredTags())
    , excludedTags(builder.getExcludedTags())
    , id(builder.getSearchId())
{
}

SearchParamsImpl::SearchParamsImpl(Bool visibleOnly, Bool recursive)
    : recursive(recursive)
    , visibleOnly(visibleOnly)
    , requiredTags(List<IString>())
    , excludedTags(List<IString>())
    , id(IComponent::Id)
{
}

ErrCode SearchParamsImpl::getRecursive(Bool* recursive)
{
    OPENDAQ_PARAM_NOT_NULL(recursive);

    *recursive = this->recursive;
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsImpl::getVisibleOnly(Bool* visibleOnly)
{
    OPENDAQ_PARAM_NOT_NULL(visibleOnly);

    *visibleOnly = this->visibleOnly;
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsImpl::getRequiredTags(IList** tags)
{
    OPENDAQ_PARAM_NOT_NULL(tags);

    *tags = this->requiredTags.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsImpl::getExcludedTags(IList** tags)
{
    OPENDAQ_PARAM_NOT_NULL(tags);

    *tags = this->excludedTags.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsImpl::getSearchId(IntfID* id)
{
    OPENDAQ_PARAM_NOT_NULL(id);

    *id = this->id;
    return OPENDAQ_SUCCESS;
}

OPENDAQ_DEFINE_CLASS_FACTORY_WITH_INTERFACE_AND_CREATEFUNC(LIBRARY_FACTORY, SearchParams, ISearchParams, createSearchParamsFromBuilder, ISearchParamsBuilder*, builder)
OPENDAQ_DEFINE_CLASS_FACTORY(LIBRARY_FACTORY, SearchParams, Bool, visible, Bool, recursive)

END_NAMESPACE_OPENDAQ
