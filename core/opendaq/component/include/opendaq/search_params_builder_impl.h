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
#include <opendaq/search_params.h>
#include <opendaq/search_params_builder_ptr.h>
#include <coretypes/intfs.h>
#include <coretypes/listobject_factory.h>

BEGIN_NAMESPACE_OPENDAQ

class SearchParamsBuilderImpl final : public ImplementationOf<ISearchParamsBuilder>
{
public:
    explicit SearchParamsBuilderImpl();
    explicit SearchParamsBuilderImpl(const SearchParamsPtr& searchParams);

    ErrCode INTERFACE_FUNC build(ISearchParams** searchParams) override;

    ErrCode INTERFACE_FUNC setRecursive(Bool recursive) override;
    ErrCode INTERFACE_FUNC getRecursive(Bool* recursive) override;

    ErrCode INTERFACE_FUNC setVisibleOnly(Bool visibleOnly) override;
    ErrCode INTERFACE_FUNC getVisibleOnly(Bool* visibleOnly) override;

    ErrCode INTERFACE_FUNC setRequiredTags(IList* tags) override;
    ErrCode INTERFACE_FUNC getRequiredTags(IList** tags) override;

    ErrCode INTERFACE_FUNC setExcludedTags(IList* tags) override;
    ErrCode INTERFACE_FUNC getExcludedTags(IList** tags) override;

    ErrCode INTERFACE_FUNC setSearchId(IntfID id) override;
    ErrCode INTERFACE_FUNC getSearchId(IntfID* id) override;

private:
    Bool recursive;
    Bool visibleOnly;
    ListPtr<IString> requiredTags;
    ListPtr<IString> excludedTags;
    IntfID id;
};

END_NAMESPACE_OPENDAQ
