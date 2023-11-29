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

#include <coretypes/baseobject.h>
#include <coretypes/listobject.h>

BEGIN_NAMESPACE_OPENDAQ

struct ISearchParamsBuilder;

/*!
 * @ingroup opendaq_components
 * @addtogroup opendaq_search_params Search Params
 * @{
 */

/*!
 * @brief Parameter object that can be passed to component getters/search methods to modify the search pattern.
 *
 * By default, any getter/search method that accepts the Search Params as an optional argument uses the default
 * configuration of the params - with recursive set to false, and visibleOnly set to true.
 *
 * The following parameters can be used to customize the search pattern:
 *
 * - Recursive: Recursively searches the subtree of the given folder if true. Re-uses the Search Params object when
 *   traversing the subtree.
 * - VisibleOnly: If true, searches only visible children. On recursive searches, if VisibleOnly is set to true, the
 *   search will not recurse into invisible folders.
 * - RequiredTags: List of tags that a component must have for it to be found.
 * - ExcludedTags: List of tags that a component must not have.
 * - SearchId: The interface ID that a component must implement for it to be found.
 */
DECLARE_OPENDAQ_INTERFACE(ISearchParams, IBaseObject)
{
    /*!
     * @brief Gets the Recursive search parameter
     * @param[out] recursive The Recursive search parameter.
     *
     * Recursively searches the subtree of the given folder if true. Re-uses the Search Params object when
     * traversing the subtree.
     */
    virtual ErrCode INTERFACE_FUNC getRecursive(Bool* recursive) = 0;

    /*!
     * @brief Gets the VisibleOnly search parameter
     * @param[out] visibleOnly The VisibleOnly search parameter.
     *
     * If true, searches only visible children. On recursive searches, if VisibleOnly is set to true, the
     * search will not recurse into invisible folders.
     */
    virtual ErrCode INTERFACE_FUNC getVisibleOnly(Bool* visibleOnly) = 0;

    // [elementType(tags, IString)]
    /*!
     * @brief Gets the RequiredTags search parameter
     * @param[out] tags The RequiredTags search parameter.
     *
     * List of tags that a component must have for it to be found.
     */
    virtual ErrCode INTERFACE_FUNC getRequiredTags(IList** tags) = 0;

    // [elementType(tags, IString)]
    /*!
     * @brief Gets the ExcludedTags search parameter
     * @param[out] tags The ExcludedTags search parameter.
     *
     * List of tags that a component must not have.
     */
    virtual ErrCode INTERFACE_FUNC getExcludedTags(IList** tags) = 0;

    /*!
     * @brief Gets the SearchID search parameter
     * @param[out] id The SearchID search parameter.
     *
     * The interface ID that a component must implement for it to be found.
     */
    virtual ErrCode INTERFACE_FUNC getSearchId(IntfID* id) = 0;
};

/*!@}*/

/*!
 * @ingroup opendaq_components
 * @addtogroup opendaq_search_params Component Search Params Factories
 * @{
 */

/*!
 * @brief Creates a Search Params object with overridden defaults for VisibleOnly and Recursive
 * @param visibleOnly If true, searches only visible children. On recursive searches, if VisibleOnly is set to true, the
 *                    search will not recurse into invisible folders.
 * @param recursive Recursively searches the subtree of the given folder if true. Re-uses the Search Params object when
 *                  traversing the subtree.
 */
OPENDAQ_DECLARE_CLASS_FACTORY(LIBRARY_FACTORY, SearchParams, Bool, visibleOnly, Bool, Recursive)

/*!
 * @brief Creates a SearchParams object from the builder, using the builder's current configuration.
 * @param builder Builder used for construction of the object.
 */
OPENDAQ_DECLARE_CLASS_FACTORY_WITH_INTERFACE(LIBRARY_FACTORY, SearchParamsFromBuilder, ISearchParams, ISearchParamsBuilder*, builder)

/*!@}*/

END_NAMESPACE_OPENDAQ
