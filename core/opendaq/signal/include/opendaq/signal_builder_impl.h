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
#include <opendaq/signal_builder.h>
#include <opendaq/signal_factory.h>
#include <coretypes/intfs.h>

BEGIN_NAMESPACE_OPENDAQ

class SignalBuilderImpl : public ImplementationOf<ISignalBuilder>
{
public:
    explicit SignalBuilderImpl();
    explicit SignalBuilderImpl(const ContextPtr& context,
                               const ComponentPtr& parent,
                               const StringPtr& localId);

    ErrCode INTERFACE_FUNC build(ISignal** signal) override;
    ErrCode INTERFACE_FUNC getLocalId(IString** localId) override;
    ErrCode INTERFACE_FUNC setLocalId(IString* localId) override;
    ErrCode INTERFACE_FUNC getParent(IComponent** parent) override;
    ErrCode INTERFACE_FUNC setParent(IComponent* parent) override;
    ErrCode INTERFACE_FUNC getContext(IContext** context) override;
    ErrCode INTERFACE_FUNC setContext(IContext* context) override;
    ErrCode INTERFACE_FUNC getDescriptor(IDataDescriptor** descriptor) override;
    ErrCode INTERFACE_FUNC setDescriptor(IDataDescriptor* descriptor) override;
    ErrCode INTERFACE_FUNC getDomainSignal(ISignal** signal) override;
    ErrCode INTERFACE_FUNC setDomainSignal(ISignal* signal) override;
    ErrCode INTERFACE_FUNC getComponentStandardPropsMode(ComponentStandardProps* standardProps) override;
    ErrCode INTERFACE_FUNC setComponentStandardPropsMode(ComponentStandardProps standardProps) override;
    ErrCode INTERFACE_FUNC getName(IString** name) override;
    ErrCode INTERFACE_FUNC setName(IString* name) override;
    ErrCode INTERFACE_FUNC getDescription(IString** description) override;
    ErrCode INTERFACE_FUNC setDescription(IString* description) override;
    ErrCode INTERFACE_FUNC getTags(ITagsConfig** tags) override;
    ErrCode INTERFACE_FUNC setTags(ITagsConfig* tags) override;
    ErrCode INTERFACE_FUNC getVisible(Bool* visible) override;
    ErrCode INTERFACE_FUNC setVisible(Bool visible) override;
    ErrCode INTERFACE_FUNC getActive(Bool* active) override;
    ErrCode INTERFACE_FUNC setActive(Bool active) override;
    ErrCode INTERFACE_FUNC getPublic(Bool* isPublic) override;
    ErrCode INTERFACE_FUNC setPublic(Bool isPublic) override;
    ErrCode INTERFACE_FUNC getRelatedSignals(IList** signals) override;
    ErrCode INTERFACE_FUNC setRelatedSignals(IList* signals) override;
    ErrCode INTERFACE_FUNC getClassName(IString** className) override;
    ErrCode INTERFACE_FUNC setClassName(IString* className) override;

private:
    ContextPtr context;
    ComponentPtr parent;
    StringPtr localId;
    DataDescriptorPtr dataDescriptor;
    SignalPtr domainSignal;
    ComponentStandardProps componentStandardProps;
    StringPtr name;
    StringPtr description;
    TagsConfigPtr tags;
    Bool visible;
    Bool active;
    Bool _public;
    ListPtr<ISignal> relatedSignals;
    StringPtr className;
};

END_NAMESPACE_OPENDAQ
