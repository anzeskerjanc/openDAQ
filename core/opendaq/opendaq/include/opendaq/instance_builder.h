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
 *
 * The InstanceBuilder provides a fluent interface for setting various configuration options for an Instance
 * object before its creation. It allows customization of the logger, module manager, scheduler and root device. 
 * Once configured, calling the `build` method returns a fully initialized Instance object with the specified settings.
 * 
 * @subsection Configuration Methods:
 * The InstanceBuilder provides the following configuration methods:
 *
 * - **setLogger:** Sets the custom Logger for the Instance. This logger will be used for logging messages related to
 *   the Instance and its components.
 *
 * - **setGlobalLogLevel:** Sets the default Logger global log level for the Instance. All log messages with a severity
 *   level equal to or higher than the specified level will be processed.
 *
 * - **setComponentLogLevel:** Sets the Logger level for a specific component of the Instance. Log messages related to
 *   that component will be processed according to the specified log level.
 *
 * - **addLoggerSink:** Adds a logger sink to the default Instance logger. This sink will be responsible for processing
 *   log messages, such as writing them to a file or sending them to a remote server.
 *
 * - **setModulePath:** Sets the path for the default ModuleManager of the Instance. If a custom module manager has not
 *   been set, this path will be used to load modules.
 *
 * - **setModuleManager:** Sets the custom ModuleManager for the Instance. If set, the default module manager path
 *   will be ignored.
 *
 * - **setSchedulerWorkerNum:** Sets the number of worker threads in the scheduler of the Instance. If a scheduler has
 *   not already been set, this defines the number of threads available for parallel processing.
 *
 * - **setScheduler:** Sets a custom scheduler for the Instance. If set, the number of worker threads will be ignored.
 *
 * - **setDefaultRootDeviceName:** Sets the virtual Client as the default root device with the specified name. If the
 *   RootDevice has not been set, the Instance Root Device will also be set as the virtual Client.
 *
 * - **setDefaultRootDeviceInfo:** Sets the default device information for the Instance. If the device information has
 *   not been set, the `GetAvailableDevices` method of the Instance will return this set device information.
 *
 * - **setRootDevice:** Sets the connection string for the root device of the Instance. This defines the device that
 *   serves as the entry point for the Instance.
 *
 * - **setSinkLogLevel:** Sets the sink logger level of the default Instance logger. This level is ignored if a custom
 *   logger has already been set.
 *
 * Note:
 * Some configuration options, such as the custom logger, module manager, and scheduler, are optional. If not set, the
 * Instance will use default implementations. Users can choose to configure only the specific aspects they need,
 * allowing for a tailored setup.
 * 
 * @subsection InstanceBuilder Configuration Methods Relationship Diagram
 * This diagram provides a visual representation of the dependencies between various configuration methods in the InstanceBuilder class. 
 * Notably, invoking certain methods, such as setLogger, as well as other methods, disables or turns off the impact of subsequent calls to related methods 
 * like setGlobalLogLevel, setSinkLogLevel, addLoggerSink, and analogous behaviors exist for other methods.
 * 
 * InstanceBuilder
 * │
 * ├── setLogger
 * │   ├── setGlobalLogLevel
 * │   ├── setSinkLogLevel
 * │   └── addLoggerSink
 * │
 * ├── setComponentLogLevel
 * │
 * ├── setModulePath
 * │   └── setModuleManager
 * │
 * ├── setSchedulerWorkerNum
 * │   └── setScheduler
 * │
 * └── setRootDevice
 *     ├── setDefaultRootDeviceName
 *     └── setDefaultRootDeviceInfo
 * 
 * Note:
 * The `setRootDevice` method overrides the root device configuration, providing a specific device. 
 * However, it's important to note that the default root device remains configured through the use of `setDefaultRootDeviceName` and `setDefaultRootDeviceInfo`.
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
     * @brief Sets the custom Logger of Instance
     * @param logger The custom Logger of Instance
     */
    virtual ErrCode INTERFACE_FUNC setLogger(ILogger* logger) = 0;

    /*!
     * @brief Gets the Logger of Instance. Returns nullptr if custom logger has not been set
     * @param[out] logger The Logger of Instance
     */
    virtual ErrCode INTERFACE_FUNC getLogger(ILogger** logger) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the default Logger global level of Instance. Ignored if a custom logger has already been set
     * @param logLevel The Logger global level of Instance
     */
    virtual ErrCode INTERFACE_FUNC setGlobalLogLevel(LogLevel logLevel) = 0;

    /*!
     * @brief Gets the default Logger global level of Instance
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
     * @brief Gets the dictionary of component names and log level which will be added to logger components
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
     * @brief Gets the list of logger sink of default Instance logger.
     * @param[out] sinks The list of logger sink of default Instance logger
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
     * @brief Sets the custom ModuleManager of Instance.
     * @param moduleManager The custom ModuleManager of Instance
     */
    virtual ErrCode INTERFACE_FUNC setModuleManager(IModuleManager* moduleManager) = 0;

    /*!
     * @brief Gets the custom ModuleManager of Instance. Returns nullptr is custom ModuleManager has not been set
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
     * @brief Sets the scheduler of Instance
     * @param moduleManager The scheduler of Instance
     */
    virtual ErrCode INTERFACE_FUNC setScheduler(IScheduler* scheduler) = 0;

    /*!
     * @brief Gets the scheduler of Instance
     * @param[out] moduleManager The scheduler of Instance
     */
    virtual ErrCode INTERFACE_FUNC getScheduler(IScheduler** scheduler) = 0;

    // [returnSelf]
    /*!
     * @brief Sets the virtual Client as default root device with set name. if RootDevice has not been set, Instance Root Device as well will be set as virtual Client 
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
     * @brief Sets the connection string for the root device of Instance.
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
     * @brief Sets the default device info of Instance. if device info has been set method GetAvailableDevices of Instance will return set device info if has not been set Root Device
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