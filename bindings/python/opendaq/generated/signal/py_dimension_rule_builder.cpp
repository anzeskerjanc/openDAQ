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

#include "py_opendaq/py_opendaq.h"
#include "py_core_types/py_converter.h"

PyDaqIntf<daq::IDimensionRuleBuilder, daq::IBaseObject> declareIDimensionRuleBuilder(pybind11::module_ m)
{
    return wrapInterface<daq::IDimensionRuleBuilder, daq::IBaseObject>(m, "IDimensionRuleBuilder");
}

void defineIDimensionRuleBuilder(pybind11::module_ m, PyDaqIntf<daq::IDimensionRuleBuilder, daq::IBaseObject> cls)
{
    cls.doc() = "Configuration component of Dimension rule objects. Contains setter methods that allow for Dimension rule parameter configuration, and a `build` method that builds the Dimension rule.";

    m.def("DimensionRuleBuilder", &daq::DimensionRuleBuilder_Create);
    m.def("DimensionRuleBuilderFromExisting", &daq::DimensionRuleBuilderFromExisting_Create);

    cls.def_property("type",
        nullptr,
        [](daq::IDimensionRuleBuilder *object, daq::DimensionRuleType type)
        {
            const auto objectPtr = daq::DimensionRuleBuilderPtr::Borrow(object);
            objectPtr.setType(type);
        },
        "Sets the type of the dimension rule. Rule parameters must be configured to match the requirements of the rule type.");
    cls.def_property("parameters",
        nullptr,
        [](daq::IDimensionRuleBuilder *object, daq::IDict* parameters)
        {
            const auto objectPtr = daq::DimensionRuleBuilderPtr::Borrow(object);
            objectPtr.setParameters(parameters);
        },
        "Sets a dictionary of string-object key-value pairs representing the parameters used to evaluate the rule.");
    cls.def("add_parameter",
        [](daq::IDimensionRuleBuilder *object, const std::string& name, const py::object& parameter)
        {
            const auto objectPtr = daq::DimensionRuleBuilderPtr::Borrow(object);
            objectPtr.addParameter(name, pyObjectToBaseObject(parameter));
        },
        py::arg("name"), py::arg("parameter"),
        "Adds a string-object pair parameter to the Dictionary of Dimension rule parameters.");
    cls.def("remove_parameter",
        [](daq::IDimensionRuleBuilder *object, const std::string& name)
        {
            const auto objectPtr = daq::DimensionRuleBuilderPtr::Borrow(object);
            objectPtr.removeParameter(name);
        },
        py::arg("name"),
        "Removes the parameter with the given name from the Dictionary of Dimension rule parameters.");
    cls.def("build",
        [](daq::IDimensionRuleBuilder *object)
        {
            const auto objectPtr = daq::DimensionRuleBuilderPtr::Borrow(object);
            return objectPtr.build().detach();
        },
        "Builds and returns a Dimension rule object using the currently set values of the Builder.");
}
