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

PyDaqIntf<daq::IInstance, daq::IDevice> declareIInstance(pybind11::module_ m)
{
    return wrapInterface<daq::IInstance, daq::IDevice>(m, "IInstance");
}

void defineIInstance(pybind11::module_ m, PyDaqIntf<daq::IInstance, daq::IDevice> cls)
{
    cls.doc() = "The top-level openDAQ object. It acts as container for the openDAQ context and the base module manager.";

    m.def("Instance", &daq::Instance_Create);
    m.def("Client", &daq::Client_Create);

    cls.def_property_readonly("module_manager",
        [](daq::IInstance *object)
        {
            const auto objectPtr = daq::InstancePtr::Borrow(object);
            return objectPtr.getModuleManager().detach();
        },
        py::return_value_policy::take_ownership,
        "Gets the Module manager.");
    cls.def_property_readonly("root_device",
        [](daq::IInstance *object)
        {
            const auto objectPtr = daq::InstancePtr::Borrow(object);
            return objectPtr.getRootDevice().detach();
        },
        py::return_value_policy::take_ownership,
        "Gets the current root device.");
    cls.def("set_root_device",
        [](daq::IInstance *object, const std::string& connectionString, daq::IPropertyObject* config)
        {
            const auto objectPtr = daq::InstancePtr::Borrow(object);
            objectPtr.setRootDevice(connectionString, config);
        },
        py::arg("connection_string"), py::arg("config") = nullptr,
        "Adds a device with the connection string as root device.");
    cls.def_property_readonly("available_server_types",
        [](daq::IInstance *object)
        {
            const auto objectPtr = daq::InstancePtr::Borrow(object);
            return objectPtr.getAvailableServerTypes().detach();
        },
        py::return_value_policy::take_ownership,
        "Get a dictionary of available server types as <IString, IServerType> pairs");
    cls.def("add_server",
        [](daq::IInstance *object, const std::string& serverTypeId, daq::IPropertyObject* serverConfig)
        {
            const auto objectPtr = daq::InstancePtr::Borrow(object);
            return objectPtr.addServer(serverTypeId, serverConfig).detach();
        },
        py::arg("server_type_id"), py::arg("server_config"),
        "Creates and adds a server with the provided serverType and configuration.");
    cls.def("add_standard_servers",
        [](daq::IInstance *object)
        {
            const auto objectPtr = daq::InstancePtr::Borrow(object);
            return objectPtr.addStandardServers().detach();
        },
        "Creates and adds streaming and \"openDAQ OpcUa\" servers with default configurations.");
    cls.def("remove_server",
        [](daq::IInstance *object, daq::IServer* server)
        {
            const auto objectPtr = daq::InstancePtr::Borrow(object);
            objectPtr.removeServer(server);
        },
        py::arg("server"),
        "Removes the server provided as argument.");
    cls.def_property_readonly("servers",
        [](daq::IInstance *object)
        {
            const auto objectPtr = daq::InstancePtr::Borrow(object);
            return objectPtr.getServers().detach();
        },
        py::return_value_policy::take_ownership,
        "Get list of added servers.");
    cls.def("find_component",
        [](daq::IInstance *object, daq::IComponent* component, const std::string& id)
        {
            const auto objectPtr = daq::InstancePtr::Borrow(object);
            return objectPtr.findComponent(component, id).detach();
        },
        py::arg("component"), py::arg("id"),
        "Finds the component (signal/device/function block) with the specified (global) id.");
}
