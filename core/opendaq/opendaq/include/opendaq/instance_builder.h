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
#include <coretypes/stringobject.h>
#include <opendaq/instance.h>

BEGIN_NAMESPACE_OPENDAQ

/*!
 * @ingroup opendaq_devices
 * @addtogroup opendaq_instance InstanceBuilder
 * @{
 */

/*!
 * @brief Builder component of Instance objects. Contains setter methods to configure the Instance parameters such a Context (Logger, Scheduler, ModuleManager) and RootDevice.
 * Contains a  `build` method that builds the Instance object.
 */
DECLARE_OPENDAQ_INTERFACE(IInstanceBuilder, IBaseObject)
{
    /*!
     * @brief Builds and returns a Instance object using the currently set values of the Builder.
     * @param[out] instance The built Instance.
     */
    virtual ErrCode INTERFACE_FUNC build(IInstance** instance) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the Logger of Instance
     * @param logger The Logger of Instance
     */
    virtual ErrCode INTERFACE_FUNC setLogger(ILogger* logger) = 0;

    /*!
     * @brief Gets the Logger of Instance
     * @param[out] logger Tthe Logger of Instance
     */
    virtual ErrCode INTERFACE_FUNC getLogger(ILogger** logger) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the Logger global level of Instance. Ignored if a custom logger has already been set
     * @param logLevel The Logger global level of Instance
     */
    virtual ErrCode INTERFACE_FUNC setGlobalLogLevel(LogLevel logLevel) = 0;

    /*!
     * @brief Gets the Logger global level of Instance
     * @param[out] logLevel The Logger global level of Instance
     */
    virtual ErrCode INTERFACE_FUNC getGlobalLogLevel(LogLevel* logLevel) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the Logger level of Instance component
     * @param component The name of Instance component
     * @param logLevel The log level of Instance component
     */
    virtual ErrCode INTERFACE_FUNC setComponentLogLevel(IString* component, LogLevel logLevel) = 0;
    
    // [templateType(components, IString, INumber)]
    /*!
     * @brief Get the dictionary of component names and log level which will be added to logger components
     * @param[out] components The dictionary of component names and log level
     */
    virtual ErrCode INTERFACE_FUNC getComponentsLogLevel(IDict** components) = 0;
    
    // [returnSelf]
    /*!
     * @brief Adds the logger sink of default Instance logger. Ignored if a custom logger has already been set
     * @param sink The logger sink of default Instance logger
     */
    virtual ErrCode INTERFACE_FUNC addLoggerSink(ILoggerSink* sink) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the sink logger level of default Instance logger. Ignored if a custom logger has already been set
     * @param sink The sink logger of default Instance logger
     * @param logLevel The sink logger level of default Instance logger
     */
    virtual ErrCode INTERFACE_FUNC setSinkLogLevel(ILoggerSink* sink, LogLevel logLevel) = 0;

    // [elementType(sinks, ILoggerSink)]
    /*!
     * @brief Gets the list of logger sink of  default Instance logger.
     * @param[out] sinks The list of logger sink of  default Instance logger
     */
    virtual ErrCode INTERFACE_FUNC getLoggerSinks(IList** sinks) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the path for default ModuleManager of Instance. Ignored if a custom module manager has already been set
     * @param path The path for default ModuleManager of Instance
     */
    virtual ErrCode INTERFACE_FUNC setModulePath(IString* path) = 0;

    /*!
     * @brief Gets the path for default ModuleManager of Instance.
     * @param[out] path The path for default ModuleManager of Instance
     */
    virtual ErrCode INTERFACE_FUNC getModulePath(IString** path) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the ModuleManager of Instance.
     * @param moduleManager The ModuleManager of Instance
     */
    virtual ErrCode INTERFACE_FUNC setModuleManager(IModuleManager* moduleManager) = 0;

    /*!
     * @brief Gets the ModuleManager of Instance.
     * @param[out] moduleManager The ModuleManager of Instance
     */
    virtual ErrCode INTERFACE_FUNC getModuleManager(IModuleManager** moduleManager) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the amount of worker threads in scheduler of Instance. Ignored if a scheduler has already been set
     * @param numWorkers The amount of worker threads in scheduler of Instance. If @c 0 then maximum number of concurrent threads supported by the implementation is used.
     */
    virtual ErrCode INTERFACE_FUNC setSchedulerWorkerNum(SizeT numWorkers) = 0;

    /*!
     * @brief Gets the amount of worker threads in scheduler of Instance.
     * @param[out] numWorkers The amount of worker threads in scheduler of Instance.
     */
    virtual ErrCode INTERFACE_FUNC getSchedulerWorkerNum(SizeT* numWorkers) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the module manager of Instance
     * @param moduleManager The module manager of Instance
     */
    virtual ErrCode INTERFACE_FUNC setScheduler(IScheduler* scheduler) = 0;

    /*!
     * @brief Gets the module manager of Instance
     * @param[out] moduleManager The module manager of Instance
     */
    virtual ErrCode INTERFACE_FUNC getScheduler(IScheduler** scheduler) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the default root device name
     * @param rootDevice The default root device name
     */
    virtual ErrCode INTERFACE_FUNC setDefaultRootDeviceName(IString* localId) = 0;

    /*!
     * @brief Gets the default root device name
     * @param[out] rootDevice The default root device name
     */
    virtual ErrCode INTERFACE_FUNC getDefaultRootDeviceName(IString** localId) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the connection string for the default root device of Instance.
     * @param connectionString The connection string for the default root device of Instance
     */
    virtual ErrCode INTERFACE_FUNC setRootDevice(IString* connectionString) = 0;

    /*!
     * @brief Gets the connection string for the default root device of Instance.
     * @param[out] rootDevice The connection string for the default root device of Instance
     */
    virtual ErrCode INTERFACE_FUNC getRootDevice(IString** connectionString) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the default device info of Instance. if device info has been set method GetAvailableDevices of Instance will return set device info
     * @param deviceInfo The device info of default device of Instance
     */
    virtual ErrCode INTERFACE_FUNC setDefaultRootDeviceInfo(IDeviceInfo* deviceInfo) = 0;

    /*!
     * @brief Gets the default device info of Instance
     * @param defaultDevice The default device info of Instance
     */
    virtual ErrCode INTERFACE_FUNC getDefaultRootDeviceInfo(IDeviceInfo** deviceInfo) = 0;
};
/*!@}*/

/*!
 * @brief Creates a InstanceBuilder with no parameters configured.
 */
OPENDAQ_DECLARE_CLASS_FACTORY_WITH_INTERFACE(LIBRARY_FACTORY, InstanceBuilder, IInstanceBuilder)

END_NAMESPACE_OPENDAQ
