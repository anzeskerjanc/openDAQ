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
#include <opendaq/search_params_ptr.h>
#include <opendaq/search_params_builder_ptr.h>

BEGIN_NAMESPACE_OPENDAQ

/*!
 * @ingroup opendaq_components
 * @addtogroup opendaq_search_params Component Search Params Factories
 * @{
 */

/*!
 * @brief Creates a SearchParams object from the builder, using the builder's current configuration.
 * @param builder Builder used for construction of the object.
 */
inline SearchParamsPtr SearchParams(const SearchParamsBuilderPtr& builder)
{
    SearchParamsPtr obj(SearchParamsFromBuilder_Create(builder));
    return obj;
}

/*!
 * @brief Creates a Search Params object with overridden defaults for VisibleOnly and Recursive
 * @param visibleOnly If true, searches only visible children. On recursive searches, if VisibleOnly is set to true, the
 *                    search will not recurse into invisible folders.
 * @param recursive Recursively searches the subtree of the given folder if true. Re-uses the Search Params object when
 *                  traversing the subtree.
 */
inline SearchParamsPtr SearchParams(Bool visibleOnly = true, Bool recursive = false)
{
    SearchParamsPtr obj(SearchParams_Create(visibleOnly, recursive));
    return obj;
}

/*!
 * @brief Creates a Search Params Builder with all fields initialized to default.
 *
 * Defaults are:
 *   - Recursive = false
 *   - VisibleOnly = true
 *   - RequiredTags = empty list
 *   - ExcludedTags = empty list
 *   - SearchID = IComponent::Id
 */
inline SearchParamsBuilderPtr SearchParamsBuilder()
{
    SearchParamsBuilderPtr obj(SearchParamsBuilder_Create());
    return obj;
}

/*!
 * @brief Creates a Search Params Builder, copying the configuration of the Search Params passed as argument
 * @param searchParams the Search Params to be copied.
 */
inline SearchParamsBuilderPtr SearchParamsBuilderCopy(const SearchParamsPtr& searchParams)
{
    SearchParamsBuilderPtr obj(SearchParamsBuilderCopy_Create(searchParams));
    return obj;
}


/*!@}*/

END_NAMESPACE_OPENDAQ
