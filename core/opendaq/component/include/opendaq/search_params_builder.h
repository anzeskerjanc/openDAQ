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

BEGIN_NAMESPACE_OPENDAQ

/*!
 * @ingroup opendaq_components
 * @addtogroup opendaq_search_params Search Params Builder
 * @{
 */
    
/*!
 * @brief Builder component of Search Params objects. Contains setter methods to configure the Search Params parameters, and a
 * `build` method that builds the Search Params object.
 */
DECLARE_OPENDAQ_INTERFACE(ISearchParamsBuilder, IBaseObject)
{
    /*!
     * @brief Builds and returns the Search Params.
     * @param[out] searchParams The built Search Params.
     */
    virtual ErrCode INTERFACE_FUNC build(ISearchParams** searchParams) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the Recursive search parameter
     * @param recursive The Recursive search parameter.
     *
     * Recursively searches the subtree of the given folder if true. Re-uses the Search Params object when
     * traversing the subtree.
     */
    virtual ErrCode INTERFACE_FUNC setRecursive(Bool recursive) = 0;
    /*!
     * @brief Gets the Recursive search parameter
     * @param[out] recursive The Recursive search parameter.
     *
     * Recursively searches the subtree of the given folder if true. Re-uses the Search Params object when
     * traversing the subtree.
     */
    virtual ErrCode INTERFACE_FUNC getRecursive(Bool* recursive) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the VisibleOnly search parameter
     * @param visibleOnly The VisibleOnly search parameter.
     *
     * If true, searches only visible children. On recursive searches, if VisibleOnly is set to true, the
     * search will not recurse into invisible folders.
     */
    virtual ErrCode INTERFACE_FUNC setVisibleOnly(Bool visibleOnly) = 0;
    /*!
     * @brief Gets the VisibleOnly search parameter
     * @param[out] visibleOnly The VisibleOnly search parameter.
     *
     * If true, searches only visible children. On recursive searches, if VisibleOnly is set to true, the
     * search will not recurse into invisible folders.
     */
    virtual ErrCode INTERFACE_FUNC getVisibleOnly(Bool* visibleOnly) = 0;

    // [elementType(tags, IString), returnSelf]   
    /*!
     * @brief Sets the RequiredTags search parameter
     * @param tags The RequiredTags search parameter.
     *
     * List of tags that a component must have for it to be found.
     */
    virtual ErrCode INTERFACE_FUNC setRequiredTags(IList* tags) = 0;
    // [elementType(tags, IString)]
    /*!
     * @brief Gets the RequiredTags search parameter
     * @param[out] tags The RequiredTags search parameter.
     *
     * List of tags that a component must have for it to be found.
     */
    virtual ErrCode INTERFACE_FUNC getRequiredTags(IList** tags) = 0;

    // [elementType(tags, IString), returnSelf]
    /*!
     * @brief Sets the ExcludedTags search parameter
     * @param tags The ExcludedTags search parameter.
     *
     * List of tags that a component must not have.
     */
    virtual ErrCode INTERFACE_FUNC setExcludedTags(IList* tags) = 0;
    // [elementType(tags, IString)]
    /*!
     * @brief Gets the ExcludedTags search parameter
     * @param[out] tags The ExcludedTags search parameter.
     *
     * List of tags that a component must not have.
     */
    virtual ErrCode INTERFACE_FUNC getExcludedTags(IList** tags) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the SearchID search parameter
     * @param id The SearchID search parameter.
     *
     * The interface ID that a component must implement for it to be found.
     */
    virtual ErrCode INTERFACE_FUNC setSearchId(IntfID id) = 0;
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
 * @brief Creates a Search Params Builder with all fields initialized to default.
 *
 * Defaults are:
 *   - Recursive = false
 *   - VisibleOnly = true
 *   - RequiredTags = empty list
 *   - ExcludedTags = empty list
 *   - SearchID = IComponent::Id
 */
OPENDAQ_DECLARE_CLASS_FACTORY(LIBRARY_FACTORY, SearchParamsBuilder)

/*!
 * @brief Creates a Search Params Builder, copying the configuration of the Search Params passed as argument
 * @param searchParams the Search Params to be copied.
 */
OPENDAQ_DECLARE_CLASS_FACTORY_WITH_INTERFACE(LIBRARY_FACTORY, SearchParamsBuilderCopy, ISearchParamsBuilder, ISearchParams*, searchParams)

/*!@}*/

END_NAMESPACE_OPENDAQ
