#include <opendaq/search_params_builder_impl.h>
#include <opendaq/search_params_factory.h>
#include <coretypes/validation.h>
#include <coretypes/impl.h>
#include <opendaq/component.h>

BEGIN_NAMESPACE_OPENDAQ

SearchParamsBuilderImpl::SearchParamsBuilderImpl()
    : recursive(false)
    , visibleOnly(true)
    , requiredTags(List<IString>())
    , excludedTags(List<IString>())
    , id(IComponent::Id)
{
}

SearchParamsBuilderImpl::SearchParamsBuilderImpl(const SearchParamsPtr& searchParams)
    : recursive(searchParams.getRecursive())
    , visibleOnly(searchParams.getVisibleOnly())
    , requiredTags(searchParams.getRequiredTags())
    , excludedTags(searchParams.getExcludedTags())
    , id(searchParams.getSearchId())
{
}

    ErrCode SearchParamsBuilderImpl::setRecursive(Bool recursive)
{
    this->recursive = recursive;
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsBuilderImpl::getRecursive(Bool* recursive)
{
    OPENDAQ_PARAM_NOT_NULL(recursive);

    *recursive = this->recursive;
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsBuilderImpl::setVisibleOnly(Bool visibleOnly)
{
    this->visibleOnly = visibleOnly;
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsBuilderImpl::getVisibleOnly(Bool* visibleOnly)
{
    OPENDAQ_PARAM_NOT_NULL(visibleOnly);

    *visibleOnly = this->visibleOnly;
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsBuilderImpl::setRequiredTags(IList* tags)
{
    this->requiredTags = tags;
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsBuilderImpl::getRequiredTags(IList** tags)
{
    OPENDAQ_PARAM_NOT_NULL(tags);

    *tags = this->requiredTags.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsBuilderImpl::setExcludedTags(IList* tags)
{
    this->excludedTags = tags;
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsBuilderImpl::getExcludedTags(IList** tags)
{
    OPENDAQ_PARAM_NOT_NULL(tags);

    *tags = this->excludedTags.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsBuilderImpl::setSearchId(IntfID id)
{
    this->id = id;
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsBuilderImpl::getSearchId(IntfID* id)
{
    OPENDAQ_PARAM_NOT_NULL(id);

    *id = this->id;
    return OPENDAQ_SUCCESS;
}

ErrCode SearchParamsBuilderImpl::build(ISearchParams** searchParams)
{
    OPENDAQ_PARAM_NOT_NULL(searchParams);

    return daqTry([&]()
    {
        const auto thisPtr = this->borrowPtr<SearchParamsBuilderPtr>();
        *searchParams = SearchParams(thisPtr).detach();
        return OPENDAQ_SUCCESS;
    });
}

OPENDAQ_DEFINE_CLASS_FACTORY(LIBRARY_FACTORY, SearchParamsBuilder)
OPENDAQ_DEFINE_CLASS_FACTORY_WITH_INTERFACE_AND_CREATEFUNC(LIBRARY_FACTORY,
                                                           SearchParamsBuilder,
                                                           ISearchParamsBuilder,
                                                           createSearchParamsBuilderCopy,
                                                           ISearchParams*,
                                                           searchParams)

END_NAMESPACE_OPENDAQ
