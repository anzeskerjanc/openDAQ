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
#include <opendaq/signal.h>

BEGIN_NAMESPACE_OPENDAQ


/*!
 * @ingroup opendaq_signals
 * @addtogroup opendaq_signal Signal Builder
 * @{
 */

/*!
 * @brief Builder interface that allows for building and customizing construction parameters of a Signal.
 */
DECLARE_OPENDAQ_INTERFACE(ISignalBuilder, IBaseObject)
{
    /*!
     * @brief Builds and returns the signal.
     * @param[out] signal The built signal.
     */
    virtual ErrCode INTERFACE_FUNC build(ISignal** signal) = 0;
    
    /*!
     * @brief Gets the context object.
     * @param[out] context The context object.
     *
     * The builder should have the same context as all other components.
     */
    virtual ErrCode INTERFACE_FUNC getContext(IContext** context) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the context object.
     * @param context The context object.
     *
     * The builder should have the same context as all other components.
     */
    virtual ErrCode INTERFACE_FUNC setContext(IContext* context) = 0;

    /*!
     * @brief Gets the local ID of the signal.
     * @param[out] localId The local ID of the signal.
     *
     * Represents the identifier that is unique in a relation to the parent component. There is no
     * predefined format for local ID. It is a string defined by its parent component.
     */
    virtual ErrCode INTERFACE_FUNC getLocalId(IString** localId) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the local ID of the signal.
     * @param localId The local ID of the signal.
     *
     * Represents the identifier that is unique in a relation to the parent component. There is no
     * predefined format for local ID. It is a string defined by its parent component.
     */
    virtual ErrCode INTERFACE_FUNC setLocalId(IString* localId) = 0;
    
    /*!
     * @brief Gets the parent of the signal.
     * @param[out] parent The parent of the signal.
     *
     * Every openDAQ component has a parent, except for instance. Parent should be passed as
     * a parameter to the constructor/factory. Once the signal is built, the parent
     * cannot be changed.
     */
    virtual ErrCode INTERFACE_FUNC getParent(IComponent** parent) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the parent of the signal.
     * @param parent The parent of the signal.
     *
     * Every openDAQ component has a parent, except for instance. Parent should be passed as
     * a parameter to the constructor/factory. Once the signal is built, the parent
     * cannot be changed.
     */
    virtual ErrCode INTERFACE_FUNC setParent(IComponent* parent) = 0;
    
    /*!
     * @brief Gets the signal's data descriptor.
     * @param[out] descriptor The signal's data descriptor.
     *
     * The descriptor contains metadata about the signal, providing information about its name, description,...
     * and defines how the data in it's packet's buffers should be interpreted.
     */
    virtual ErrCode INTERFACE_FUNC getDescriptor(IDataDescriptor** descriptor) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the data descriptor.
     * @param descriptor The data descriptor.
     *
     * The descriptor contains metadata about the signal, providing information about its name, description,...
     * and defines how the data in it's packet's buffers should be interpreted.
     */
    virtual ErrCode INTERFACE_FUNC setDescriptor(IDataDescriptor* descriptor) = 0;

    /*!
     * @brief Gets the signal that carries its domain data.
     * @param[out] signal The domain signal.
     */
    virtual ErrCode INTERFACE_FUNC getDomainSignal(ISignal** signal) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the domain signal reference.
     * @param signal The domain signal.
     */
    virtual ErrCode INTERFACE_FUNC setDomainSignal(ISignal* signal) = 0;

    /*!
     * @brief Gets the name of the signal.
     * @param[out] name The name of the signal. Local ID if name is not configured.
     */
    virtual ErrCode INTERFACE_FUNC getName(IString** name) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the name of the signal.
     * @param name The name of the signal.
     */
    virtual ErrCode INTERFACE_FUNC setName(IString* name) = 0;

    /*!
     * @brief Gets the description of the signal.
     * @param[out] description The description of the signal. Empty if not configured.
     */
    virtual ErrCode INTERFACE_FUNC getDescription(IString** description) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the description of the signal.
     * @param description The description of the signal. Empty if not configured.
     */
    virtual ErrCode INTERFACE_FUNC setDescription(IString* description) = 0;
    
    /*!
     * @brief Gets the tags of the signal.
     * @param[out] tags The tags of the signal.
     *
     * Tags are user definable labels that can be attached to the signal.
     */
    virtual ErrCode INTERFACE_FUNC getTags(ITagsConfig** tags) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the tags of the signal.
     * @param tags The tags of the signal.
     *
     * Tags are user definable labels that can be attached to the signal.
     */
    virtual ErrCode INTERFACE_FUNC setTags(ITagsConfig* tags) = 0;

    /*!
     * @brief Gets `visible` metadata state of the signal
     * @param[out] visible True if the component is visible; False otherwise.
     *
     * Visible determines whether search/getter methods return find the signal by default.
     */
    virtual ErrCode INTERFACE_FUNC getVisible(Bool* visible) = 0;
    // [returnSelf]
    /*!
     * @brief Sets `visible` metadata state of the signal
     * @param visible True if the component is visible; False otherwise.
     *
     * Visible determines whether search/getter methods return find the signal by default.
     */
    virtual ErrCode INTERFACE_FUNC setVisible(Bool visible) = 0;

    /*!
     * @brief Returns true if the signal is active; false otherwise.
     * @param[out] active True if the signal is active; false otherwise.
     *
     * An active signal sends packets on the signal path.
     */
    virtual ErrCode INTERFACE_FUNC getActive(Bool* active) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the signal to be either active or inactive.
     * @param active True if the signal is active; false otherwise.
     *
     * An active signal sends packets on the signal path.
     */
    virtual ErrCode INTERFACE_FUNC setActive(Bool active) = 0;

    /*!
     * @brief Returns true if the signal is public; false otherwise.
     * @param[out] isPublic True if the signal is public; false otherwise.
     *
     * Public signals are visible to clients connected to the device, and are streamed.
     */
    virtual ErrCode INTERFACE_FUNC getPublic(Bool* isPublic) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the signal to be either public or private.
     * @param isPublic If false, the signal is set to private; if true, the signal is set to be public.
     *
     * Public signals are visible to clients connected to the device, and are streamed.
     */
    virtual ErrCode INTERFACE_FUNC setPublic(Bool isPublic) = 0;

    /*!
     * @brief Gets the standard property mode used to determine whether standard component properties should
     * be added, added as read-only, or skipped.
     * @param[out] standardProps the mode of handling standard component props on creation.
     */
    virtual ErrCode INTERFACE_FUNC getComponentStandardPropsMode(ComponentStandardProps* standardProps) = 0;
    // [returnSelf]
    /*!
     * @brief Sets the standard property mode used to determine whether standard component properties should
     * be added, added as read-only, or skipped.
     * @param standardProps the mode of handling standard component props on creation.
     */
    virtual ErrCode INTERFACE_FUNC setComponentStandardPropsMode(ComponentStandardProps standardProps) = 0;

    // [elementType(signals, ISignal)]
    /*!
     * @brief Gets a list of related signals.
     * @param[out] signals The list of related signals.
     *
     * Signals within the related signals list are facilitate the interpretation of a given signal's data, or
     * are otherwise relevant when working with the signal.
     */
    virtual ErrCode INTERFACE_FUNC getRelatedSignals(IList** signals) = 0;

    // [elementType(signals, ISignal), returnSelf]
    /*!
     * @brief Sets the list of related signals.
     * @param signals The list of related signals.
     *
     * Signals within the related signals list are facilitate the interpretation of a given signal's data, or
     * are otherwise relevant when working with the signal.
     */
    virtual ErrCode INTERFACE_FUNC setRelatedSignals(IList* signals) = 0;

    /*!
     * @brief Gets the class name. The signal will inherit the properties of a Property Object Class with the provided name.
     * @param[out] className The name of the Property Object Class.
     */
    virtual ErrCode INTERFACE_FUNC getClassName(IString** className) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the class name. The signal will inherit the properties of a Property Object Class with the provided name.
     * @param className The name of the Property Object Class.
     */
    virtual ErrCode INTERFACE_FUNC setClassName(IString* className) = 0;
};
/*!@}*/

/*!
 * @ingroup opendaq_signal
 * @addtogroup opendaq_signal_factories Factories
 * @{
 */

/*!
 * @brief Creates a Signal Builder with the context, parent, and localId configured.
 * @param context The openDAQ context. Should be the same as contained in all components in a given instance.
 * @param parent The parent of the Signal that is being built.
 * @param localId An ID of the signal. Should be unique amongst all children of the provided parent.
 *
 * Other builder fields are set to their default values.
 */
OPENDAQ_DECLARE_CLASS_FACTORY(
    LIBRARY_FACTORY, SignalBuilder,
    IContext*, context,
    IComponent*, parent,
    IString*, localId
)

/*!
 * @brief Creates a Signal Builder with all fields set to their defaults.
 *
 * The builder needs at least the context, parent, and localId configured before `build` is called.
 */
OPENDAQ_DECLARE_CLASS_FACTORY_WITH_INTERFACE(LIBRARY_FACTORY, SignalBuilderEmpty, ISignalBuilder)

/*!@}*/

END_NAMESPACE_OPENDAQ
