//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//
//     RTGen (PythonGenerator).
// </auto-generated>
//------------------------------------------------------------------------------

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

#include "py_core_objects/py_core_objects.h"
#include "py_core_types/py_converter.h"

PyDaqIntf<daq::IPropertyObjectClassBuilder, daq::IBaseObject> declareIPropertyObjectClassBuilder(pybind11::module_ m)
{
    return wrapInterface<daq::IPropertyObjectClassBuilder, daq::IBaseObject>(m, "IPropertyObjectClassBuilder");
}

void defineIPropertyObjectClassBuilder(pybind11::module_ m, PyDaqIntf<daq::IPropertyObjectClassBuilder, daq::IBaseObject> cls)
{
    cls.doc() = "The builder interface of Property object classes. Allows for their modification and building of Property object classes.";

    m.def("PropertyObjectClassBuilder", &daq::PropertyObjectClassBuilder_Create);
    m.def("PropertyObjectClassBuilderWithManager", &daq::PropertyObjectClassBuilderWithManager_Create);

    cls.def_property("name",
        nullptr,
        [](daq::IPropertyObjectClassBuilder *object, const std::string& className)
        {
            const auto objectPtr = daq::PropertyObjectClassBuilderPtr::Borrow(object);
            objectPtr.setName(className);
        },
        "Sets the name of the property class.");
    cls.def_property("parent_name",
        nullptr,
        [](daq::IPropertyObjectClassBuilder *object, const std::string& parentName)
        {
            const auto objectPtr = daq::PropertyObjectClassBuilderPtr::Borrow(object);
            objectPtr.setParentName(parentName);
        },
        "Gets the name of the parent of the property class.");
    cls.def("add_property",
        [](daq::IPropertyObjectClassBuilder *object, daq::IProperty* property)
        {
            const auto objectPtr = daq::PropertyObjectClassBuilderPtr::Borrow(object);
            objectPtr.addProperty(property);
        },
        py::arg("property"),
        "Adds a property to the class.");
    cls.def("remove_property",
        [](daq::IPropertyObjectClassBuilder *object, const std::string& propertyName)
        {
            const auto objectPtr = daq::PropertyObjectClassBuilderPtr::Borrow(object);
            objectPtr.removeProperty(propertyName);
        },
        py::arg("property_name"),
        "Removes a property with the given name from the class.");
    cls.def_property("property_order",
        nullptr,
        [](daq::IPropertyObjectClassBuilder *object, daq::IList* orderedPropertyNames)
        {
            const auto objectPtr = daq::PropertyObjectClassBuilderPtr::Borrow(object);
            objectPtr.setPropertyOrder(orderedPropertyNames);
        },
        "Sets a custom order of properties as defined in the list of property names.");
    cls.def("build",
        [](daq::IPropertyObjectClassBuilder *object)
        {
            const auto objectPtr = daq::PropertyObjectClassBuilderPtr::Borrow(object);
            return objectPtr.build().detach();
        },
        "Builds and returns a Property object class using the currently set values of the Builder.");
}
