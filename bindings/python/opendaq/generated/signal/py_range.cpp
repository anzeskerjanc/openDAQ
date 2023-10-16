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

PyDaqIntf<daq::IRange, daq::IBaseObject> declareIRange(pybind11::module_ m)
{
    return wrapInterface<daq::IRange, daq::IBaseObject>(m, "IRange");
}

void defineIRange(pybind11::module_ m, PyDaqIntf<daq::IRange, daq::IBaseObject> cls)
{
    cls.doc() = "Describes a range of values between the `lowValue` and `highValue` boundaries.";

    m.def("Range", &daq::Range_Create);

    cls.def_property_readonly("low_value",
        [](daq::IRange *object)
        {
            const auto objectPtr = daq::RangePtr::Borrow(object);
            return objectPtr.getLowValue().detach();
        },
        py::return_value_policy::take_ownership,
        "Gets the lower boundary value of the range.");
    cls.def_property_readonly("high_value",
        [](daq::IRange *object)
        {
            const auto objectPtr = daq::RangePtr::Borrow(object);
            return objectPtr.getHighValue().detach();
        },
        py::return_value_policy::take_ownership,
        "Gets the upper boundary value of the range.");
}
