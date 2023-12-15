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
#include <coreobjects/eval_value_ptr.h>
#include <coreobjects/exceptions.h>
#include <coreobjects/property_ptr.h>
#include <coreobjects/property_object_class_ptr.h>
#include <coreobjects/property_object_ptr.h>
#include <coreobjects/ownable_ptr.h>
#include <coreobjects/property_factory.h>
#include <coreobjects/property_object.h>
#include <coreobjects/property_object_protected.h>
#include <coretypes/type_manager_ptr.h>
#include <coreobjects/property_value_event_args_factory.h>
#include <coreobjects/end_update_event_args_factory.h>
#include <coreobjects/object_keys.h>
#include <coretypes/coretypes.h>
#include <coretypes/updatable.h>
#include <tsl/ordered_map.h>
#include <utility>
#include <map>
#include <coreobjects/property_internal_ptr.h>
#include <coreobjects/property_object_internal_ptr.h>

BEGIN_NAMESPACE_OPENDAQ

using PropertyOrderedMap = tsl::ordered_map<StringPtr, PropertyPtr, StringHash, StringEqualTo>;

struct PropertyNameInfo
{
    StringPtr name;
    Int index{};
};

template <typename PropObjInterface, typename... Interfaces>
class GenericPropertyObjectImpl : public ImplementationOfWeak<PropObjInterface,
                                                              IOwnable,
                                                              IFreezable,
                                                              ISerializable,
                                                              IUpdatable,
                                                              IPropertyObjectProtected,
                                                              IPropertyObjectInternal,
                                                              Interfaces...>
{
public:
    explicit GenericPropertyObjectImpl();
    explicit GenericPropertyObjectImpl(const TypeManagerPtr& manager, const StringPtr& className);

    virtual ErrCode INTERFACE_FUNC getClassName(IString** className) override;

    virtual ErrCode INTERFACE_FUNC setPropertyValue(IString* propertyName, IBaseObject* value) override;
    virtual ErrCode INTERFACE_FUNC getPropertyValue(IString* propertyName, IBaseObject** value) override;
    virtual ErrCode INTERFACE_FUNC getPropertySelectionValue(IString* propertyName, IBaseObject** value) override;
    virtual ErrCode INTERFACE_FUNC clearPropertyValue(IString* propertyName) override;

    virtual ErrCode INTERFACE_FUNC hasProperty(IString* propertyName, Bool* hasProperty) override;
    virtual ErrCode INTERFACE_FUNC getProperty(IString* propertyName, IProperty** property) override;
    virtual ErrCode INTERFACE_FUNC addProperty(IProperty* property) override;
    virtual ErrCode INTERFACE_FUNC removeProperty(IString* propertyName) override;

    virtual ErrCode INTERFACE_FUNC getVisibleProperties(IList** properties) override;
    virtual ErrCode INTERFACE_FUNC getAllProperties(IList** properties) override;

    virtual ErrCode INTERFACE_FUNC setPropertyOrder(IList* orderedPropertyNames) override;
    
    virtual ErrCode INTERFACE_FUNC getOnPropertyValueWrite(IString* propertyName, IEvent** event) override;
    virtual ErrCode INTERFACE_FUNC getOnPropertyValueRead(IString* propertyName, IEvent** event) override;

    virtual ErrCode INTERFACE_FUNC beginUpdate() override;
    virtual ErrCode INTERFACE_FUNC endUpdate() override;

    virtual ErrCode INTERFACE_FUNC getOnEndUpdate(IEvent** event) override;

    // IPropertyObjectInternal
    ErrCode INTERFACE_FUNC checkForReferences(IProperty* property, Bool* isReferenced) override;

    // IUpdatable
    virtual ErrCode INTERFACE_FUNC update(ISerializedObject* obj) override;

    // ISerializable
    virtual ErrCode INTERFACE_FUNC serialize(ISerializer* serializer) override;
    virtual ErrCode INTERFACE_FUNC getSerializeId(ConstCharPtr* id) const override;

    virtual ErrCode INTERFACE_FUNC toString(CharPtr* str) override;

    static ConstCharPtr SerializeId();
    static ErrCode Deserialize(ISerializedObject* serialized, IBaseObject* context, IFunction* factoryCallback, IBaseObject** obj);

    // IOwnable
    virtual ErrCode INTERFACE_FUNC setOwner(IPropertyObject* newOwner) override;

    [[nodiscard]]
    WeakRefPtr<IPropertyObject> getOwner() const;

    // IFreezable
    virtual ErrCode INTERFACE_FUNC freeze() override;
    virtual ErrCode INTERFACE_FUNC isFrozen(Bool* isFrozen) const override;

    // IPropertyObjectProtected
    virtual ErrCode INTERFACE_FUNC setProtectedPropertyValue(IString* propertyName, IBaseObject* value) override;
    virtual ErrCode INTERFACE_FUNC clearProtectedPropertyValue(IString* propertyName) override;

protected:
    struct UpdatingAction
    {
        bool setValue;
        BaseObjectPtr value;
    };

    using UpdatingActions = tsl::ordered_map<PropertyPtr, UpdatingAction, ObjectHash<IProperty>, ObjectEqualTo<IProperty>>;

    bool frozen;
    WeakRefPtr<IPropertyObject> owner;
    std::vector<StringPtr> customOrder;
    PropertyObjectPtr objPtr;
    int updateCount;
    UpdatingActions updatingPropsAndValues;

    void internalDispose(bool) override;
    ErrCode setPropertyValueInternal(IString* name, IBaseObject* value, bool triggerEvent, bool protectedAccess, bool isUpdating);
    ErrCode clearPropertyValueInternal(IString* name, bool protectedAccess, bool batch);

    // Serialization

    ErrCode serializeProperties(ISerializer* serializer);

    virtual ErrCode serializeCustomValues(ISerializer* serializer);
    virtual ErrCode serializeProperty(const StringPtr& name, const ObjectPtr<IBaseObject>& value, ISerializer* serializer);

    static ErrCode DeserializeProperties(ISerializedObject* serialized, IBaseObject* context, IFunction* factoryCallback, IPropertyObject* propObjPtr);


    // Child property handling - Used when a property is queried in the "parent.child" format
    bool isChildProperty(const StringPtr& name, StringPtr& childName, StringPtr& subName) const;

    // Update

    ErrCode setPropertyFromSerialized(const StringPtr& propName,
                                      const PropertyObjectPtr& propObj,
                                      const SerializedObjectPtr& serialized);

    virtual void updatingValuesWrite(const UpdatingActions& propsAndValues);
    virtual void applyUpdate();
private:
    using PropertyValueEventEmitter = EventEmitter<PropertyObjectPtr, PropertyValueEventArgsPtr>;
    using EndUpdateEventEmitter = EventEmitter<PropertyObjectPtr, EndUpdateEventArgsPtr>;

    StringPtr className;
    PropertyObjectClassPtr objectClass;
    std::unordered_map<StringPtr, PropertyValueEventEmitter> valueWriteEvents;
    std::unordered_map<StringPtr, PropertyValueEventEmitter> valueReadEvents;
    EndUpdateEventEmitter endUpdateEvent;

    PropertyOrderedMap localProperties;
    std::unordered_map<StringPtr, BaseObjectPtr, StringHash, StringEqualTo> propValues;

    // Gets the property, as well as its value. Gets the referenced property, if the property is a refProp
    ErrCode getPropertyAndValueInternal(const StringPtr& name, BaseObjectPtr& value, PropertyPtr& property);
    ErrCode getPropertiesInternal(Bool includeInvisible, Bool bind, IList** list);

    // Gets the property value, if stored in local value dictionary (propValues)
    // Parses brackets, if the property is a list
    ErrCode readLocalValue(const StringPtr& name, BaseObjectPtr& value) const;
    PropertyNameInfo getPropertyNameInfo(const StringPtr& name) const;

    // Adds the value to the local list of values (`propValues`)
    void writeLocalValue(const StringPtr& name, const BaseObjectPtr& value);

    // Checks if the value is a container type, or base `IPropertyObject`. Only such values can be set in `setProperty`
    ErrCode checkContainerType(const PropertyPtr& prop, const BaseObjectPtr& value);

    // Checks if the property is a struct type, and checks its fields for type/name compatibility
    ErrCode checkStructType(const PropertyPtr& prop, const BaseObjectPtr& value);

    // Checks if value is a correct key into the list/dictionary of selection values
    ErrCode checkSelectionValues(const PropertyPtr& prop, const BaseObjectPtr& value);

    // Called when `setPropertyValue` successfully sets a new value
    void callPropertyValueWrite(const PropertyPtr& prop, const BaseObjectPtr& newValue, PropertyEventType changeType, bool isUpdating);

    // Called at the end of `getPropertyValue`
    BaseObjectPtr callPropertyValueRead(const PropertyPtr& prop, const BaseObjectPtr& readValue);

    // Checks if property and value type match. If not, attempts to convert the value
    ErrCode checkPropertyTypeAndConvert(const PropertyPtr& prop, BaseObjectPtr& value);

    // Sets `this` as owner of `value`, if `value` is ownable
    void setOwnerToPropertyValue(const BaseObjectPtr& value);

    // Gets the index integer value between two square brackets
    int parseIndex(char const* lBracket) const;

    // Gets the property name without the index as the `propName` output parameter
    // Returns the index in the form of [index], eg. [0]
    ConstCharPtr getPropNameWithoutIndex(const StringPtr& name, StringPtr& propName) const;

    // Child property handling - Used when a property is queried in the "parent.child" format
    ErrCode getChildPropertyValue(const StringPtr& childName, const StringPtr& subName, BaseObjectPtr& value);

    // Does not bind property to object and does not look up reference property
    PropertyPtr getUnboundProperty(const StringPtr& name);
    PropertyPtr getUnboundPropertyOrNull(const StringPtr& name) const;
    
    PropertyPtr checkForRefPropAndGetBoundProp(PropertyPtr& prop, bool* isReferenced = nullptr) const;

    // Checks whether the property is a reference property that references an already referenced property
    bool hasDuplicateReferences(const PropertyPtr& prop);

    // Coercion/Validation
    void coercePropertyWrite(const PropertyPtr& prop, ObjectPtr<IBaseObject>& valuePtr) const;
    void validatePropertyWrite(const PropertyPtr& prop, ObjectPtr<IBaseObject>& valuePtr) const;
    void coerceMinMax(const PropertyPtr& prop, ObjectPtr<IBaseObject>& valuePtr);
    bool checkIsReferenced(const StringPtr& referencedPropName, const PropertyInternalPtr& prop);

    // update
    ErrCode updateObjectProperties(const PropertyObjectPtr& propObj,
                                   const SerializedObjectPtr& serialized,
                                   const ListPtr<IProperty>& props);
};

using PropertyObjectImpl = GenericPropertyObjectImpl<IPropertyObject>;

template <class PropObjInterface, class... Interfaces>
GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::GenericPropertyObjectImpl()
    : frozen(false)
    , updateCount(0)
    , className(nullptr)
    , objectClass(nullptr)
{
    this->internalAddRef();
    objPtr = this->template borrowPtr<PropertyObjectPtr>();
}


template <typename PropObjInterface, typename... Interfaces>
GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::GenericPropertyObjectImpl(const TypeManagerPtr& manager,
                                                                                      const StringPtr& className)
    : GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::GenericPropertyObjectImpl()
{
    if (className.assigned() && className != "")
    {
        this->className = className;
        if (!manager.assigned())
            throw ManagerNotAssignedException{};

        const TypePtr type = manager.getType(className);

        if (!type.assigned())
            throw NotFoundException{ "Class with name {} is not available in module manager", className };

        const auto objClass = type.asPtrOrNull<IPropertyObjectClass>();
        if (!objClass.assigned())
            throw InvalidTypeException{"Type with name {} is not a property object class", className};

        objectClass = objClass;
    }
}

template <class PropObjInterface, class... Interfaces>
void GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::internalDispose(bool)
{
    for (auto& item : propValues)
    {
        if (item.second.assigned())
        {
            OwnablePtr ownablePtr = item.second.template asPtrOrNull<IOwnable>(true);
            if (ownablePtr.assigned())
                ownablePtr.setOwner(nullptr);
        }
    }
    propValues.clear();

    owner.release();
    className.release();
    objectClass.release();
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getClassName(IString** className)
{
    if (className == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    if (this->className.assigned())
    {
        *className = this->className.addRefAndReturn();
    }
    else
    {
        *className = String("").detach();
    }

    return OPENDAQ_SUCCESS;
}

#if defined(__GNUC__) && __GNUC__ >= 12
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdangling-pointer"
#endif

template <class PropObjInterface, class... Interfaces>
bool GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::isChildProperty(const StringPtr& name,
                                                                                 StringPtr& childName,
                                                                                 StringPtr& subName) const
{
    auto strName = name.getCharPtr();
    auto propName = strchr(strName, '.');
    if (propName != nullptr)
    {
        childName = String(strName, propName - strName);
        subName = String(propName + 1);
        return true;
    }

    return false;
}

template <typename PropObjInterface, typename ... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getChildPropertyValue(const StringPtr& childName,
                                                                                          const StringPtr& subName,
                                                                                          BaseObjectPtr& value)
{
    PropertyPtr prop;
    StringPtr name;

    auto err = daqTry([&]() -> auto
        {
            prop = getUnboundProperty(childName);

            prop = checkForRefPropAndGetBoundProp(prop);
            name = prop.getName();
            return OPENDAQ_SUCCESS;
        });

    if (OPENDAQ_FAILED(err))
    {
        return err;
    }

    if (!prop.assigned())
    {
        return this->makeErrorInfo(OPENDAQ_ERR_NOTFOUND, fmt::format(R"(Property "{}" does not exist)", name));
    }

    BaseObjectPtr childProp;
    err = getPropertyValue(name, &childProp);
    if (OPENDAQ_FAILED(err))
    {
        return err;
    }

    return daqTry([&]() -> auto
        {
            const auto childPropAsPropertyObject = childProp.template asPtr<IPropertyObject, PropertyObjectPtr>(true);
            value = childPropAsPropertyObject.getPropertyValue(subName);
            return OPENDAQ_SUCCESS;
        });
}

#if defined(__GNUC__) && __GNUC__ >= 12
    #pragma GCC diagnostic pop
#endif

template <class PropObjInterface, class... Interfaces>
void GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::callPropertyValueWrite(const PropertyPtr& prop,
                                                                                        const BaseObjectPtr& newValue,
                                                                                        PropertyEventType changeType,
                                                                                        bool isUpdating)
{
    if (!prop.assigned())
    {
        return;
    }

    auto args = PropertyValueEventArgs(prop, newValue, changeType, isUpdating);

    if (prop.assigned())
    {
        PropertyValueEventEmitter propEvent{prop.getOnPropertyValueWrite()};
        if (propEvent.hasListeners())
        {
            propEvent(objPtr, args);
        }
    }

    const auto name = prop.getName();
    if (valueWriteEvents.find(name) != valueWriteEvents.end())
    {
        if (valueWriteEvents[name].hasListeners())
        {
            valueWriteEvents[name](objPtr, args);
        }
    }

    if (args.getValue() != newValue)
    {
        setPropertyValueInternal(name, args.getValue(), false, true, false);
    }
}

template <typename PropObjInterface, typename... Interfaces>
BaseObjectPtr GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::callPropertyValueRead(const PropertyPtr& prop,
                                                                                                const BaseObjectPtr& readValue)
{
    if (!prop.assigned())
    {
        return readValue;
    }

    auto args = PropertyValueEventArgs(prop, readValue, PropertyEventType::Read, False);
    PropertyValueEventEmitter propEvent{prop.getOnPropertyValueRead()};
    if (propEvent.hasListeners())
    {
        propEvent(objPtr, args);
    }

    const auto name = prop.getName();
    if (valueReadEvents.find(name) != valueReadEvents.end())
    {
        if (valueReadEvents[name].hasListeners())
        {
            valueReadEvents[name](objPtr, args);
        }
    }
    
    return args.getValue();
}

template <class PropObjInterface, class... Interfaces>
void GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::coercePropertyWrite(const PropertyPtr& prop,
                                                                                     ObjectPtr<IBaseObject>& valuePtr) const
{
    if (prop.assigned() && valuePtr.assigned())
    {
        const auto coercer = prop.getCoercer();
        if (coercer.assigned())
        {
            try
            {
                const auto* propObj = static_cast<const IPropertyObject*>(this);
                valuePtr = coercer.coerce(propObj, valuePtr);
            }
            catch (const DaqException&)
            {
                throw;
            }
            catch (...)
            {
                throw CoerceFailedException{};
            }
        }
    }
}

template <class PropObjInterface, class... Interfaces>
void GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::validatePropertyWrite(const PropertyPtr& prop,
                                                                                       ObjectPtr<IBaseObject>& valuePtr) const
{
    if (prop.assigned() && valuePtr.assigned())
    {
        const auto validator = prop.getValidator();
        if (validator.assigned())
        {
            try
            {
                const auto* propObj = static_cast<const IPropertyObject*>(this);
                validator.validate(propObj, valuePtr);
            }
            catch (const DaqException&)
            {
                throw;
            }
            catch (...)
            {
                throw ValidateFailedException{};
            }
        }
    }
}

template <class PropObjInterface, typename... Interfaces>
void GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::coerceMinMax(const PropertyPtr& prop, ObjectPtr<IBaseObject>& valuePtr)
{
    if (!prop.assigned() || !valuePtr.assigned())
        return;
    
    const auto min = prop.getMinValue();
    if (min.assigned())
    {
        try
        {
            if (valuePtr < min)
                valuePtr = min;
        }
        catch (...)
        {
        }
    }

    const auto max = prop.getMaxValue();
    if (max.assigned())
    {
        try
        {
            if (valuePtr > max)
                valuePtr = max;
        }
        catch (...)
        {
        }
    }
}

template <class PropObjInterface, typename... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::setProtectedPropertyValue(IString* propertyName, IBaseObject* value)
{
    return setPropertyValueInternal(propertyName, value, true, true, updateCount > 0);
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::setPropertyValue(IString* propertyName, IBaseObject* value)
{
    return setPropertyValueInternal(propertyName, value, true, false, updateCount > 0);
}

template <typename PropObjInterface, typename... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::checkContainerType(const PropertyPtr& prop, const BaseObjectPtr& value)
{
    if (!value.assigned())
        return OPENDAQ_SUCCESS;

    auto coreType = value.getCoreType();
    if (coreType == ctObject)
    {
        auto inspect = value.asPtrOrNull<IInspectable>();
        if (inspect.assigned() && !inspect.getInterfaceIds().empty())
        {
            return inspect.getInterfaceIds()[0] == IPropertyObject::Id;
        }

        return this->makeErrorInfo(OPENDAQ_ERR_INVALIDTYPE, "Only base Property Object object-type values are allowed");
    }

    auto iterate = [this](const IterablePtr<IBaseObject>& it, CoreType type)
    {
        for (const auto& key : it)
        {
            auto ct = key.getCoreType();
            
            if (ct != type)
            {
                return false;
            }

            if (ct == ctObject)
            {
                auto inspect = key.asPtrOrNull<IInspectable>();
                if (inspect.assigned() && !inspect.getInterfaceIds().empty())
                {
                    return inspect.getInterfaceIds()[0] == IPropertyObject::Id;
                }
            }
        }
        return true;
    };
    
    if (coreType == ctDict)
    {
        const auto dict = value.asPtrOrNull<IDict>();
        const auto keyType = prop.getKeyType();
        const auto itemType = prop.getItemType();
        IterablePtr<IBaseObject> it;
        dict->getKeys(&it);
        if (!iterate(it, keyType))
        {
            return this->makeErrorInfo(OPENDAQ_ERR_INVALIDTYPE, "Invalid dictionary key type");
        }

        dict->getValues(&it);
        if (!iterate(it, itemType))
        {
            return this->makeErrorInfo(OPENDAQ_ERR_INVALIDTYPE, "Invalid dictionary item type");
        }
    }

    
    if (coreType == ctList)
    {
        const auto itemType = prop.getItemType();
        if (!iterate(value, itemType))
        {
            return this->makeErrorInfo(OPENDAQ_ERR_INVALIDTYPE, "Invalid list item type");
        }
    }

    return OPENDAQ_SUCCESS;
}

template <typename PropObjInterface, typename ... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::checkStructType(const PropertyPtr& prop, const BaseObjectPtr& value)
{
    if (prop.getValueType() != ctStruct)
        return OPENDAQ_SUCCESS;

    auto structPtr = value.asPtrOrNull<IStruct>();
    if (!structPtr.assigned())
        return this->makeErrorInfo(OPENDAQ_ERR_INVALIDSTATE, "Set value is not a struct");
    
    StructTypePtr structType = prop.getStructType();
    StructTypePtr valueStructType = structPtr.getStructType();

    if (structType != valueStructType)
        return this->makeErrorInfo(OPENDAQ_ERR_INVALIDSTATE, "Set value StructureType is different from the default.");

    return OPENDAQ_SUCCESS;
}

template <typename PropObjInterface, typename... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::checkSelectionValues(const PropertyPtr& prop, const BaseObjectPtr& value)
{
    const auto selectionValues = prop.getSelectionValues();
    if (selectionValues.assigned())
    {
        const SizeT key = value;
        const auto list = selectionValues.asPtrOrNull<IList>();
        if (list.assigned() && key < list.getCount())
            return OPENDAQ_SUCCESS;

        const auto dict = selectionValues.asPtrOrNull<IDict>();
        if (dict.assigned() && dict.hasKey(value))
            return OPENDAQ_SUCCESS;

        return this->makeErrorInfo(OPENDAQ_ERR_NOTFOUND, "Value is not a key/index of selection values.");
    }

    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, typename... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::setPropertyValueInternal(IString* name,
                                                                                             IBaseObject* value,
                                                                                             bool triggerEvent, 
                                                                                             bool protectedAccess,
                                                                                             bool isUpdating)
{
    if (name == nullptr || value == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    if (frozen)
        return OPENDAQ_ERR_FROZEN;

    try
    {
        auto propName = StringPtr::Borrow(name);
        auto valuePtr = BaseObjectPtr::Borrow(value);

        StringPtr childName;
        StringPtr subName;
        const auto isChildProp = isChildProperty(propName, childName, subName);
        if (isChildProp)
        {
            propName = childName;
        }

        PropertyPtr prop = getUnboundProperty(propName);
        prop = checkForRefPropAndGetBoundProp(prop);

        if (!prop.assigned())
        {
            return this->makeErrorInfo(OPENDAQ_ERR_NOTFOUND, fmt::format(R"(Property "{}" not found.)", propName));
        }

        propName = prop.getName();

        if (!protectedAccess)
        {
            if (prop.getReadOnly())
            {
                return OPENDAQ_ERR_ACCESSDENIED;
            }
        }

        if (isChildProp)
        {
            BaseObjectPtr childProp;
            const ErrCode err = getPropertyValue(propName, &childProp);
            if (OPENDAQ_FAILED(err))
            {
                return err;
            }

            const auto childPropAsPropertyObject = childProp.template asPtr<IPropertyObject, PropertyObjectPtr>(true);
            childPropAsPropertyObject.setPropertyValue(subName, valuePtr);
        }
        else
        {
            // TODO: If function type, check if return value is correct type.

            ErrCode err = checkPropertyTypeAndConvert(prop, valuePtr);
            if (OPENDAQ_FAILED(err))
            {
                return err;
            }

            err = checkContainerType(prop, valuePtr);
            if (OPENDAQ_FAILED(err))
            {
                return err;
            }

            err = checkSelectionValues(prop, valuePtr);
            if (OPENDAQ_FAILED(err))
            {
                return err;
            }

            err = checkStructType(prop, valuePtr);
            if (OPENDAQ_FAILED(err))
            {
                return err;
            }

            coercePropertyWrite(prop, valuePtr);
            validatePropertyWrite(prop, valuePtr);
            coerceMinMax(prop, valuePtr);

            if (isUpdating)
            {
                updatingPropsAndValues.insert_or_assign(prop, UpdatingAction{true, valuePtr});
            }
            else
            {
                writeLocalValue(propName, valuePtr);
                setOwnerToPropertyValue(valuePtr);
                if (triggerEvent)
                {
                    callPropertyValueWrite(prop, valuePtr, PropertyEventType::Update, false);
                }
            }
        }

        return OPENDAQ_SUCCESS;
    }
    catch (const DaqException& e)
    {
        return this->makeErrorInfo(e.getErrCode(), e.what());
    }
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::checkPropertyTypeAndConvert(const PropertyPtr& prop,
                                                                                                BaseObjectPtr& value)
{
    if (!prop.assigned() || !value.assigned())
        return OPENDAQ_SUCCESS;

    if (value.supportsInterface<IEvalValue>())
    {
        return OPENDAQ_SUCCESS;
    }

    try
    {
        const auto propCoreType = prop.getValueType();
        const auto valueCoreType = value.getCoreType();

        if (propCoreType != valueCoreType)
            value = value.convertTo(propCoreType);
    }
    catch (const DaqException& e)
    {
        return this->makeErrorInfo(e.getErrCode(), "Value type is different than Property type and conversion failed");
    }

    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
void GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::writeLocalValue(const StringPtr& name, const BaseObjectPtr& value)
{
    auto it = propValues.find(name);
    if (it != propValues.end())
    {
        it->second = value;
    }
    else
    {
        propValues.emplace(name, value);
    }
}

template <class PropObjInterface, class... Interfaces>
void GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::setOwnerToPropertyValue(const BaseObjectPtr& value)
{
    if (!value.assigned())
        return;

    auto ownablePtr = value.asPtrOrNull<IOwnable>(true);
    if (ownablePtr.assigned())
    {
        try
        {
            ownablePtr.setOwner(this->template borrowThis<GenericPropertyObjectPtr, IPropertyObject>());
        }
        catch (const DaqException& e)
        {
            this->makeErrorInfo(e.getErrCode(), "Failed to set owner to property value");
            throw;
        }
    }
}


template <class PropObjInterface, class... Interfaces>
PropertyPtr GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getUnboundProperty(const StringPtr& name)
{
    const auto res = localProperties.find(name);
    if (res == localProperties.end())
    {
        if (objectClass == nullptr)
            throw NotFoundException(fmt::format(R"(Property with name {} does not exist.)", name));

        return objectClass.getProperty(name);
    }

    return res->second;
}

template <class PropObjInterface, class... Interfaces>
PropertyPtr GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getUnboundPropertyOrNull(const StringPtr& name) const
{
    const auto res = localProperties.find(name);
    if (res != localProperties.cend())
        return res->second;

    if (objectClass == nullptr)
        return nullptr;

    PropertyPtr property;
    const auto errCode = objectClass->getProperty(name, &property);
    if (errCode == OPENDAQ_ERR_NOTFOUND)
    {
        this->clearErrorInfo();
        return nullptr;
    }

    checkErrorInfo(errCode);
    return property;
}

template <class PropObjInterface, class... Interfaces>
PropertyPtr GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::checkForRefPropAndGetBoundProp(PropertyPtr& prop,
                                                                                                  bool* isReferenced) const
{
    if (!prop.assigned())
    {
        return prop;
    }

    auto boundProp = prop.asPtr<IPropertyInternal>().cloneWithOwner(objPtr);
    auto refProp = boundProp.getReferencedProperty();
    if (refProp.assigned())
    {
        CoreType ct = refProp.getCoreType();

        if (ct != ctObject)
            throw std::invalid_argument("Invalid reference to property");

        if (isReferenced)
            *isReferenced = true;

        return checkForRefPropAndGetBoundProp(refProp);
    }

    if (isReferenced)
        *isReferenced = false;
    return boundProp;
}

template <typename PropObjInterface, typename ... Interfaces>
bool GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::hasDuplicateReferences(const PropertyPtr& prop)
{
    auto refEval = prop.asPtr<IPropertyInternal>().getReferencedPropertyUnresolved();
    if (refEval.assigned())
    {
        auto refNames = refEval.getPropertyReferences();
        for (auto refPropName : refNames)
        {
            if (objPtr.hasProperty(refPropName) && objPtr.getProperty(refPropName).getIsReferenced())
                return true;
        }
    }

    return false;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::readLocalValue(const StringPtr& name, BaseObjectPtr& value) const
{
    PropertyNameInfo info = getPropertyNameInfo(name);

    const auto it = propValues.find(info.name);
    if (it != propValues.cend())
    {
        if (info.index != -1)
        {
            if (it->second.getCoreType() != ctList)
            {
                return this->makeErrorInfo(OPENDAQ_ERR_INVALIDPARAMETER, "Could not access the index as the value is not a list.");
            }

            ListPtr<IBaseObject> list = it->second;
            if (info.index >= (int) list.getCount())
            {
                return this->makeErrorInfo(OPENDAQ_ERR_OUTOFRANGE, "The index parameter is out of bounds of the list.");
            }
            value = list[std::size_t(info.index)];
        }
        else
        {
            value = it->second;
        }
        return OPENDAQ_SUCCESS;
    }

    return this->makeErrorInfo(OPENDAQ_ERR_NOTFOUND, fmt::format(R"(Property value "{}" not found)", name));
}

template <class PropObjInterface, class... Interfaces>
int GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::parseIndex(char const* lBracket) const
{
    auto last = strchr(lBracket, ']');
    if (last != nullptr)
    {
        char* end;
        int index = strtol(lBracket + 1, &end, 10);

        if (end != last)
        {
            throw InvalidParameterException("Could not parse the property index.");
        }

        return index;
    }
    throw InvalidParameterException("No matching ] found.");
}

#if defined(__GNUC__) && __GNUC__ >= 12
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdangling-pointer"
#endif

template <class PropObjInterface, class... Interfaces>
PropertyNameInfo GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getPropertyNameInfo(const StringPtr& name) const
{
    PropertyNameInfo nameInfo;

    auto propNameData = name.getCharPtr();
    auto first = strchr(propNameData, '[');
    if (first != nullptr)
    {
        nameInfo.index = parseIndex(first);
        nameInfo.name = String(propNameData, first - propNameData);
    }
    else
    {
        nameInfo.index = -1;
        nameInfo.name = name;
    }

    return nameInfo;
}

template <class PropObjInterface, class... Interfaces>
ConstCharPtr GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getPropNameWithoutIndex(const StringPtr& name,
                                                                                                 StringPtr& propName) const
{
    auto propNameData = name.getCharPtr();
    auto first = strchr(propNameData, '[');

    if (first == nullptr)
    {
        propName = String(propNameData);
    }
    else
    {
        propName = String(propNameData, first - propNameData);
    }
    return first;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getPropertyAndValueInternal(const StringPtr& name,
                                                                                                BaseObjectPtr& value,
                                                                                                PropertyPtr& property)
{
    StringPtr propName;
    ConstCharPtr bracket = getPropNameWithoutIndex(name, propName);

    property = getUnboundPropertyOrNull(propName);
    
    if (!property.assigned())
    {
        return this->makeErrorInfo(OPENDAQ_ERR_NOTFOUND, fmt::format(R"(Property "{}" does not exist)", propName));
    }

    bool isRef;
    property = checkForRefPropAndGetBoundProp(property, &isRef);

    // TODO: Extract this to own function
    if (bracket != nullptr)
    {
        if (isRef)
        {
            propName = property.getName() + std::string(bracket);
        }
        else
        {
            propName = name;
        }
    }
    else if (isRef)
    {
        propName = property.getName();
    }
    
    ErrCode res = readLocalValue(propName, value);

    if (res != OPENDAQ_ERR_NOTFOUND && OPENDAQ_FAILED(res))
    {
        return res;
    }
    daqClearErrorInfo();

    if (res == OPENDAQ_ERR_NOTFOUND)
    {
        this->clearErrorInfo();

        res = property->getDefaultValue(&value);

        if (OPENDAQ_FAILED(res) || !value.assigned())
        {
            value = nullptr;
            daqClearErrorInfo();
            return OPENDAQ_SUCCESS;
        }

        CoreType coreType = value.getCoreType();
        if (coreType == ctList && bracket != nullptr)
        {
            int index = parseIndex(bracket);
            ListPtr<IBaseObject> list = value;
            if (index >= static_cast<int>(list.getCount()))
            {
                return this->makeErrorInfo(OPENDAQ_ERR_OUTOFRANGE, "The index parameter is out of bounds of the list.");
            }
            value = list[std::size_t(index)];
        }
    }

    value = callPropertyValueRead(property, value);
    return OPENDAQ_SUCCESS;
}

#if defined(__GNUC__) && __GNUC__ >= 12
    #pragma GCC diagnostic pop
#endif

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getPropertyValue(IString* propertyName, IBaseObject** value)
{
    if (propertyName == nullptr || value == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    try
    {
        auto propName = StringPtr::Borrow(propertyName);
        BaseObjectPtr valuePtr;
        ErrCode err;

        StringPtr childName;
        StringPtr subName;

        if (isChildProperty(propName, childName, subName))
        {
            err = getChildPropertyValue(childName, subName, valuePtr);
        }
        else
        {
            PropertyPtr prop;
            err = getPropertyAndValueInternal(propName, valuePtr, prop);
        }

        if (OPENDAQ_SUCCEEDED(err))
            *value = valuePtr.detach();

        return err;
    }
    catch (const DaqException& e)
    {
        return errorFromException(e);
    }
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getPropertySelectionValue(IString* propertyName, IBaseObject** value)
{
    if (propertyName == nullptr || value == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    try
    {
        const auto propName = StringPtr::Borrow(propertyName);
        BaseObjectPtr valuePtr;
        PropertyPtr prop;

        getPropertyAndValueInternal(propName, valuePtr, prop);

        if (!prop.assigned())
            throw NotFoundException(R"(Selection property "{}" not found)", propName);

        auto values = prop.getSelectionValues();
        if (!values.assigned())
            throw InvalidPropertyException(R"(Selection property "{}" has no selection values assigned)", propName);

        auto valuesList = values.asPtrOrNull<IList, ListPtr<IBaseObject>>(true);
        if (!valuesList.assigned())
        {
            auto valuesDict = values.asPtrOrNull<IDict, DictPtr<IBaseObject, IBaseObject>>(true);
            if (!valuesDict.assigned())
            {
                throw InvalidPropertyException(R"(Selection property "{}" values is not a list or dictionary)", propName);
            }

            valuePtr = valuesDict.get(valuePtr);
        }
        else
        {
            valuePtr = valuesList.getItemAt(valuePtr);
        }

        if (prop.getItemType() != valuePtr.getCoreType())
        {
            return this->makeErrorInfo(OPENDAQ_ERR_INVALIDTYPE, "List item type mismatch");
        }

        *value = valuePtr.detach();
        return OPENDAQ_SUCCESS;
    }
    catch (const DaqException& e)
    {
        return errorFromException(e);
    }
    catch (const std::exception& e)
    {
        return errorFromException(e);
    }
    catch (...)
    {
        return OPENDAQ_ERR_GENERALERROR;
    }
}

template <class PropObjInterface, typename... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::clearProtectedPropertyValue(IString* propertyName)
{
    return clearPropertyValueInternal(propertyName, true, updateCount > 0);
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::clearPropertyValue(IString* propertyName)
{
    return clearPropertyValueInternal(propertyName, false, updateCount > 0);
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::clearPropertyValueInternal(IString* name, bool protectedAccess, bool batch)
{
    if (name == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    if (frozen)
        return OPENDAQ_ERR_FROZEN;

    try
    {
        auto propName = StringPtr::Borrow(name);

        StringPtr childName;
        StringPtr subName;
        const auto isChildProp = isChildProperty(propName, childName, subName);
        if (isChildProp)
        {
            propName = childName;
        }

        PropertyPtr prop = getUnboundPropertyOrNull(propName);
        prop = checkForRefPropAndGetBoundProp(prop);

        if (!prop.assigned())
        {
            return this->makeErrorInfo(OPENDAQ_ERR_NOTFOUND, fmt::format(R"(Property "{}" does not exist)", propName));
        }

        propName = prop.getName();

        if (!protectedAccess)
        {
            if (prop.getReadOnly())
            {
                return OPENDAQ_ERR_ACCESSDENIED;
            }
        }

        if (isChildProp)
        {
            BaseObjectPtr childProp;
            const ErrCode err = getPropertyValue(propName, &childProp);
            if (OPENDAQ_FAILED(err))
            {
                return err;
            }

            const auto childPropAsPropertyObject = childProp.template asPtr<IPropertyObject, PropertyObjectPtr>(true);
            childPropAsPropertyObject.clearPropertyValue(subName);
        }
        else
        {
            auto it = propValues.find(prop.getName());
            if (it == propValues.end())
            {
                return  OPENDAQ_IGNORED;
            }

            if (it->second.assigned())
            {
                const auto ownable = it->second.template asPtrOrNull<IOwnable>(true);
                if (ownable.assigned())
                    ownable.setOwner(nullptr);
            }

            if (batch)
            {
                updatingPropsAndValues.insert_or_assign(prop, UpdatingAction{false, nullptr});
            }
            else
            {
                propValues.erase(it);
                callPropertyValueWrite(prop, nullptr, PropertyEventType::Clear, false);
            }
        }
    }
    catch (const DaqException& e)
    {
        return errorFromException(e);
    }

    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getProperty(IString* propertyName, IProperty** property)
{
    if (propertyName == nullptr || property == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    return daqTry([&]() -> auto {
        PropertyPtr prop = getUnboundProperty(StringPtr::Borrow(propertyName));
        auto boundProp = prop.asPtr<IPropertyInternal>().cloneWithOwner(objPtr);

        auto freezable = boundProp.template asPtrOrNull<IFreezable>();
        if (freezable.assigned())
        {
            freezable.freeze();
        }

        *property = boundProp.detach();
        return OPENDAQ_SUCCESS;
    });
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::addProperty(IProperty* property)
{
    if (property == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    if (frozen)
        return OPENDAQ_ERR_FROZEN;

    return daqTry([&]() -> auto {
        const PropertyPtr propPtr = property;
        StringPtr propName = propPtr.getName();
        if (!propName.assigned())
            return this->makeErrorInfo(OPENDAQ_ERR_INVALIDVALUE, "Property does not have an assigned name.");

        if(hasDuplicateReferences(propPtr))
            return this->makeErrorInfo(OPENDAQ_ERR_INVALIDVALUE, "Reference property references a property that is already referenced by another.");

        propPtr.asPtr<IOwnable>().setOwner(objPtr);

        const auto res = localProperties.insert(std::make_pair(propName, propPtr));
        if (!res.second)
            return this->makeErrorInfo(OPENDAQ_ERR_ALREADYEXISTS, fmt::format(R"(Property with name {} already exists.)", propName));

        return OPENDAQ_SUCCESS;
    });
}

template <typename PropObjInterface, typename ... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::removeProperty(IString* propertyName)
{
    if (propertyName == nullptr)
    {
        return OPENDAQ_ERR_ARGUMENT_NULL;
    }

    if (frozen)
    {
        return OPENDAQ_ERR_FROZEN;
    }

    if(localProperties.find(propertyName) == localProperties.cend())
    {
        StringPtr namePtr = propertyName;
        return this->makeErrorInfo(OPENDAQ_ERR_NOTFOUND, fmt::format(R"(Property "{}" does not exist)", namePtr));
    }

    localProperties.erase(propertyName);
    if (propValues.find(propertyName) != propValues.cend())
    {
        propValues.erase(propertyName);
    }

    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getVisibleProperties(IList** properties)
{
    return getPropertiesInternal(false, true, properties);
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getAllProperties(IList** properties)
{
    return getPropertiesInternal(true, true, properties);
}

template <class PropObjInterface, typename... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::setPropertyOrder(IList* orderedPropertyNames)
{
    if (frozen)
        return OPENDAQ_ERR_FROZEN;

    customOrder.clear();
    if (orderedPropertyNames != nullptr)
    {
        for (auto&& propName : ListPtr<IString>::Borrow(orderedPropertyNames))
        {
            customOrder.emplace_back(propName);
        }
    }

    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getPropertiesInternal(Bool includeInvisible,
                                                                                          Bool bind,
                                                                                          IList** list)
{
    if (list == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    if (!includeInvisible && !bind)
        return OPENDAQ_ERR_INVALIDPARAMETER;

    std::vector<PropertyPtr> allProperties;
    if (objectClass.assigned())
    {
        auto propList = objectClass.getProperties(True);
        for (const auto& prop : propList)
            allProperties.push_back(prop);
    }

    for (const auto& prop : localProperties)
        allProperties.push_back(prop.second);

    PropertyOrderedMap lookup;
    for (auto& prop : allProperties)
    {
        if (!bind)
        {
            lookup.insert({prop.getName(), prop});
            continue;
        }

        auto boundProp = prop.asPtr<IPropertyInternal>().cloneWithOwner(objPtr);
        if (!includeInvisible && boundProp.getIsReferenced())
        {
            continue;
        }
        
        try
        {
            if (!includeInvisible && !boundProp.getVisible())
            {
                continue;
            }

            auto freezable = boundProp.template asPtrOrNull<IFreezable>();
            if (freezable.assigned())
            {
                freezable.freeze();
            }
            
            lookup.insert_or_assign(boundProp.getName(), boundProp);
        }
        catch (const NotFoundException&)
        {
            return OPENDAQ_ERR_NOTFOUND;
        }
        catch (const CalcFailedException&)
        {
        }
        catch (const NoInterfaceException&)
        {
        }
    }

    auto properties = List<IProperty>();
    if (!customOrder.empty())
    {
        // Add properties with explicit order
        for (auto& propName : customOrder)
        {
            const auto iter = lookup.find(propName);
            if (iter != lookup.cend())
            {
                properties.unsafePushBack(iter->second);
                lookup.erase(iter);
            }
        }

        // Add the rest of without set order
        for (auto& prop : lookup)
        {
            properties.unsafePushBack(prop.second);
        }
    }
    else
    {
        for (auto& prop : lookup)
        {
            properties.unsafePushBack(prop.second);
        }
    }

    *list = properties.detach();
    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getOnPropertyValueWrite(IString* propertyName, IEvent** event)
{
    if (event == nullptr || propertyName == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    Bool hasProp;
    StringPtr name = propertyName;

    ErrCode err = this->hasProperty(name, &hasProp);
    if (OPENDAQ_FAILED(err))
    {
        return err;
    }

    if (!hasProp)
    {
        return this->makeErrorInfo(OPENDAQ_ERR_NOTFOUND, fmt::format(R"(Property "{}" does not exist)", name));
    }


    if (valueWriteEvents.find(name) == valueWriteEvents.end())
    {
        PropertyValueEventEmitter emitter;
        valueWriteEvents.emplace(name, emitter);
    }

    *event = valueWriteEvents[name].addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

template <typename PropObjInterface, typename ... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getOnPropertyValueRead(IString* propertyName, IEvent** event)
{
    if (event == nullptr || propertyName == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    Bool hasProp;
    StringPtr name = propertyName;

    ErrCode err = this->hasProperty(name, &hasProp);
    if (OPENDAQ_FAILED(err))
    {
        return err;
    }

    if (!hasProp)
    {
        return this->makeErrorInfo(OPENDAQ_ERR_NOTFOUND, fmt::format(R"(Property "{}" does not exist)", name));
    }

    if (valueReadEvents.find(name) == valueReadEvents.end())
    {
        PropertyValueEventEmitter emitter;
        valueReadEvents.emplace(name, emitter);
    }

    *event = valueReadEvents[name].addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

template <typename PropObjInterface, typename ... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::beginUpdate()
{
    if (frozen)
        return OPENDAQ_ERR_FROZEN;

    updateCount++;

    return OPENDAQ_SUCCESS;
}

template <typename PropObjInterface, typename... Interfaces>
void GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::applyUpdate()
{
    for (auto& item : updatingPropsAndValues)
    {
        if (item.second.setValue)
        {
            writeLocalValue(item.first.getName(), item.second.value);
            setOwnerToPropertyValue(item.second.value);
        }
        else
        {
            propValues.erase(item.first.getName());
        }

        if (item.second.setValue)
            callPropertyValueWrite(item.first, item.second.value, PropertyEventType::Update, true);
        else
            callPropertyValueWrite(item.first, nullptr, PropertyEventType::Clear, true);
    }

    updatingValuesWrite(updatingPropsAndValues);
    updatingPropsAndValues.clear();
}

template <typename PropObjInterface, typename... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::endUpdate()
{
    if (updateCount == 0)
        return OPENDAQ_ERR_INVALIDSTATE;

    if (--updateCount == 0)
    {
        return daqTry(
            [this]
            {
                applyUpdate();
                return OPENDAQ_SUCCESS;
            });
    }

    return OPENDAQ_SUCCESS;
}

template <typename PropObjInterface, typename ... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getOnEndUpdate(IEvent** event)
{
    if (event == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    *event = endUpdateEvent.addRefAndReturn();
    return OPENDAQ_SUCCESS;
}

template <typename PropObjInterface, typename... Interfaces>
bool GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::checkIsReferenced(const StringPtr& referencedPropName,
                                                                                   const PropertyInternalPtr& prop)
{
    if (auto refProp = prop.getReferencedPropertyUnresolved(); refProp.assigned())
    {
        for (auto propName : refProp.getPropertyReferences())
        {
            if (propName == referencedPropName)
            {
                return true;
            }
        }
    }
    return false;
}

template <typename PropObjInterface, typename ... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::checkForReferences(IProperty* property, Bool* isReferenced)
{
    if (isReferenced == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    try
    {
        const auto propPtr = PropertyPtr::Borrow(property);
        const auto name = propPtr.getName();

        if (objectClass.assigned())
        {
            for (const auto& prop : objectClass.getProperties(True))
            {
                if (checkIsReferenced(name, prop))
                {
                    *isReferenced = true;
                    return OPENDAQ_SUCCESS;
                }
            }
        }

        for (const auto& prop : localProperties)
        {
            if (checkIsReferenced(name, prop.second))
            {
                *isReferenced = true;
                return OPENDAQ_SUCCESS;
            }
        }
    }
    catch (const DaqException& e)
    {
        return errorFromException(e);
    }
    catch (...)
    {
        return OPENDAQ_ERR_GENERALERROR;
    }


    *isReferenced = false;
    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::serializeCustomValues(ISerializer* /*serializer*/)
{
    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::serializeProperty(const StringPtr& name,
                                                                                      const ObjectPtr<IBaseObject>& value,
                                                                                      ISerializer* serializer)
{
    if (value.assigned())
    {
        ISerializable* serializableValue;
        ErrCode errCode = value->borrowInterface(ISerializable::Id, reinterpret_cast<void**>(&serializableValue));

        if (errCode == OPENDAQ_ERR_NOINTERFACE)
        {
            return OPENDAQ_SUCCESS;
        }

        if (OPENDAQ_FAILED(errCode))
        {
            return errCode;
        }

        errCode = serializer->keyStr(name);
        if (OPENDAQ_FAILED(errCode))
        {
            return errCode;
        }

        errCode = serializableValue->serialize(serializer);
        if (OPENDAQ_FAILED(errCode))
        {
            return errCode;
        }
    }
    else
    {
        ErrCode errCode = serializer->keyStr(name);
        if (OPENDAQ_FAILED(errCode))
        {
            return errCode;
        }

        errCode = serializer->writeNull();
        if (OPENDAQ_FAILED(errCode))
        {
            return errCode;
        }
    }

    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::serializeProperties(ISerializer* serializer)
{
    bool numOfSerializablePropertyValues = std::count_if(
        propValues.begin(),
        propValues.end(),
        [](std::pair<StringPtr, BaseObjectPtr> keyValue) { return keyValue.second.asPtrOrNull<ISerializable>(true).assigned(); });

    if (numOfSerializablePropertyValues == 0)
        return OPENDAQ_SUCCESS;

    serializer->key("propValues");
    serializer->startObject();
    {
        std::map<StringPtr, BaseObjectPtr> sorted(propValues.begin(), propValues.end());

        // Serialize properties with explicit order
        for (auto&& propName : customOrder)
        {
            auto propValue = sorted.find(propName);
            if (propValue != sorted.cend())
            {
                ErrCode err = serializeProperty(propValue->first, propValue->second, serializer);
                if (OPENDAQ_FAILED(err))
                {
                    return err;
                }
                sorted.erase(propValue);
            }
        }

        // Serialize the rest of without set order
        for (auto&& propValue : sorted)
        {
            ErrCode err = serializeProperty(propValue.first, propValue.second, serializer);
            if (OPENDAQ_FAILED(err))
            {
                return err;
            }
        }
    }

    serializer->endObject();

    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::serialize(ISerializer* serializer)
{
    serializer->startTaggedObject(this);

    SERIALIZE_PROP_PTR(className)

    if (frozen)
    {
        serializer->key("frozen");
        serializer->writeBool(frozen);
    }

    ErrCode errCode = serializeCustomValues(serializer);
    if (OPENDAQ_FAILED(errCode))
    {
        return errCode;
    }

    errCode = serializeProperties(serializer);
    if (OPENDAQ_FAILED(errCode))
    {
        return errCode;
    }

    serializer->endObject();
    return OPENDAQ_SUCCESS;
}

// static
template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::Deserialize(ISerializedObject* serialized,
                                                                                IBaseObject* context,
                                                                                IFunction* factoryCallback,
                                                                                IBaseObject** obj)
{
    StringPtr className;
    ErrCode errCode = serialized->readString("className"_daq, &className);
    if (OPENDAQ_FAILED(errCode))
    {
        className.release();
    }

    Bool isFrozen{};
    errCode = serialized->readBool("frozen"_daq, &isFrozen);
    if (errCode != OPENDAQ_ERR_NOTFOUND && OPENDAQ_FAILED(errCode))
    {
        return errCode;
    }
    daqClearErrorInfo();

    PropertyObjectPtr propObjPtr;
    TypeManagerPtr objManager;

    if (context != nullptr && OPENDAQ_SUCCEEDED(context->queryInterface(ITypeManager::Id, reinterpret_cast<void**>(&objManager))))
    {
        errCode = createPropertyObjectWithClassAndManager(&propObjPtr, objManager, className);
    }
    else
    {
        errCode = createPropertyObject(&propObjPtr);
    }

    if (OPENDAQ_FAILED(errCode))
    {
        return errCode;
    }

    errCode = DeserializeProperties(serialized, context, factoryCallback, propObjPtr);
    if (isFrozen)
    {
        if (ObjectPtr<IFreezable> freezable = propObjPtr.asPtrOrNull<IFreezable>(true); freezable.assigned())
        {
            errCode = freezable->freeze();
            if (OPENDAQ_FAILED(errCode))
            {
                return errCode;
            }
        }
    }

    *obj = propObjPtr.addRefAndReturn();
    return errCode;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::DeserializeProperties(ISerializedObject* serialized,
                                                                                          IBaseObject* context,
                                                                                          IFunction* factoryCallback, 
                                                                                          IPropertyObject* propObjPtr)
{
    auto hasKeyStr = String("propValues");

    Bool hasKey;
    ErrCode errCode = serialized->hasKey(hasKeyStr, &hasKey);
    if (OPENDAQ_FAILED(errCode))
    {
        return errCode;
    }

    if (!IsTrue(hasKey))
        return OPENDAQ_SUCCESS;

    SerializedObjectPtr propValues;
    errCode = serialized->readSerializedObject(String("propValues"), &propValues);
    if (OPENDAQ_FAILED(errCode))
    {
        return errCode;
    }

    ListPtr<IString> keys;
    errCode = propValues->getKeys(&keys);
    if (OPENDAQ_FAILED(errCode))
    {
        return errCode;
    }

    IPropertyObjectProtected* protectedPropObjPtr{};
    errCode = propObjPtr->borrowInterface(IPropertyObjectProtected::Id, reinterpret_cast<void**>(&protectedPropObjPtr));
    if (OPENDAQ_FAILED(errCode))
    {
        return errCode;
    }

    for (const auto& key : keys)
    {
        IString* keyStr;
        errCode = key->borrowInterface(IString::Id, reinterpret_cast<void**>(&keyStr));

        if (OPENDAQ_FAILED(errCode))
        {
            return errCode;
        }

        BaseObjectPtr propValue;
        errCode = propValues->readObject(keyStr, context, factoryCallback, &propValue);

        if (OPENDAQ_FAILED(errCode))
        {
            return errCode;
        }

        errCode = protectedPropObjPtr->setProtectedPropertyValue(keyStr, propValue);
        if (OPENDAQ_FAILED(errCode))
        {
            return errCode;
        }
    }

    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getSerializeId(ConstCharPtr* id) const
{
    *id = SerializeId();
    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::toString(CharPtr* str)
{
    if (str == nullptr)
    {
        return this->makeErrorInfo(OPENDAQ_ERR_ARGUMENT_NULL, "Parameter must not be null");
    }

    std::ostringstream stream;
    stream << "PropertyObject";

    if (className.assigned())
        stream << " {" << className << "}";

    return daqDuplicateCharPtr(stream.str().c_str(), str);
}

template <class PropObjInterface, class... Interfaces>
ConstCharPtr GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::SerializeId()
{
    return "PropertyObject";
}

template <class PropObjInterface, class... Interfaces>
WeakRefPtr<IPropertyObject> GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::getOwner() const
{
    return owner;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::setOwner(IPropertyObject* newOwner)
{
    this->owner = newOwner;
    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, typename... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::freeze()
{
    if (frozen)
        return  OPENDAQ_IGNORED;

    frozen = true;
    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, typename... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::isFrozen(Bool* isFrozen) const
{
    if (isFrozen == nullptr)
        return OPENDAQ_ERR_ARGUMENT_NULL;

    *isFrozen = frozen;
    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::hasProperty(IString* propertyName, Bool* hasProperty)
{
    if (hasProperty == nullptr || propertyName == nullptr)
    {
        return OPENDAQ_ERR_ARGUMENT_NULL;
    }

    if (localProperties.find(propertyName) != localProperties.cend())
    {
        *hasProperty = true;
        return OPENDAQ_SUCCESS;
    }

    if (objectClass.assigned())
    {
        try
        {
            *hasProperty = objectClass.hasProperty(propertyName);
            if (*hasProperty)
                return OPENDAQ_SUCCESS;
        }
        catch (...)
        {
        }
    }

    *hasProperty = false;
    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::setPropertyFromSerialized(const StringPtr& propName,
                                                                                              const PropertyObjectPtr& propObj,
                                                                                              const SerializedObjectPtr& serialized)
{
    if (!serialized.assigned())
    {
        return propObj->clearPropertyValue(propName);
    }

    BaseObjectPtr propValue;
    switch (serialized.getType(propName))
    {
        case ctBool:
            propValue = serialized.readBool(propName);
            break;
        case ctInt:
            propValue = serialized.readInt(propName);
            break;
        case ctFloat:
            propValue = serialized.readFloat(propName);
            break;
        case ctString:
            propValue = serialized.readString(propName);
            break;
        case ctList:
            propValue = serialized.readList<IBaseObject>(propName);
            break;
        case ctDict:
        case ctRatio:
        case ctStruct:
        case ctObject:
            propValue = serialized.readObject(propName);
            break;
        case ctProc:
        case ctBinaryData:
        case ctFunc:
        case ctComplexNumber:
        case ctUndefined:
            return OPENDAQ_SUCCESS;
    }

    return propObj.as<IPropertyObjectProtected>(true)->setProtectedPropertyValue(propName, propValue);
}

template <typename PropObjInterface, typename ... Interfaces>
void GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::updatingValuesWrite(
    const UpdatingActions& propsAndValues)
{
    if (endUpdateEvent.hasListeners())
    {
        auto list = List<IString>();
        for (const auto& item: propsAndValues)
            list.pushBack(item.first.getName());
        auto args = EndUpdateEventArgs(list);
        endUpdateEvent(objPtr, args);
    }
}

template <class PropObjInterface, class... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::updateObjectProperties(const PropertyObjectPtr& propObj,
                                                                                           const SerializedObjectPtr& serialized,
                                                                                           const ListPtr<IProperty>& props)
{
    SerializedObjectPtr serializedProps;

    if (serialized.hasKey("propValues"))
        serializedProps = serialized.readSerializedObject("propValues");

    beginUpdate();
    Finally finally([this]() { endUpdate(); });

    for (const auto& prop : props)
    {
        const auto propName = prop.getName();

        const auto propInternal = prop.asPtrOrNull<IPropertyInternal>(true);
        if (propInternal.assigned())
        {
            if (propInternal.getReferencedPropertyUnresolved().assigned())
                continue;
            const auto valueTypeUnresolved = propInternal.getValueTypeUnresolved();
            if (valueTypeUnresolved == CoreType::ctFunc || valueTypeUnresolved == CoreType::ctProc)
                continue;
        }

        if (!serializedProps.assigned() || !serializedProps.hasKey(propName))
        {
            const auto err = propObj.as<IPropertyObjectProtected>(true)->clearProtectedPropertyValue(propName);
            if (!OPENDAQ_SUCCEEDED(err) && err != OPENDAQ_ERR_INVALID_OPERATION)
            {
                return err;
            }

            continue;
        }

        const auto err = setPropertyFromSerialized(propName, propObj, serializedProps);

        if (!OPENDAQ_SUCCEEDED(err))
        {
            return err;
        }

    }

    return OPENDAQ_SUCCESS;
}

template <class PropObjInterface, typename... Interfaces>
ErrCode GenericPropertyObjectImpl<PropObjInterface, Interfaces...>::update(ISerializedObject* obj)
{
    if (obj == nullptr)
    {
        return OPENDAQ_ERR_ARGUMENT_NULL;
    }

    // Don't fail the upgrade if frozen just skip it
    // TODO: Check if upgrade should be allowed
    if (frozen)
        return  OPENDAQ_IGNORED;

    const auto serialized = SerializedObjectPtr::Borrow(obj);

    try
    {
        ListPtr<IProperty> allProps;
        checkErrorInfo(getPropertiesInternal(True, False, &allProps));

        return updateObjectProperties(this->thisInterface(), serialized, allProps);
    }
    catch (const DaqException& e)
    {
        return errorFromException(e);
    }
    catch (const std::exception& e)
    {
        return errorFromException(e);
    }
    catch (...)
    {
        return OPENDAQ_ERR_GENERALERROR;
    }
}

OPENDAQ_REGISTER_DESERIALIZE_FACTORY(PropertyObjectImpl)

END_NAMESPACE_OPENDAQ
