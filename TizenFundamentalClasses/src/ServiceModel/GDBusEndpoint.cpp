/*
 * Tizen Fundamental Classes - TFC
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *    ServiceModel/GDBusEndpoint.cpp
 *
 * Created on:  Dec 21, 2016
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#include "TFC/Core.h"
#include "TFC/ServiceModel/GDBusEndpoint.h"

#include <glib-2.0/gio/gio.h>

#include <iostream>

#include <regex>
#include <string>

using namespace TFC::ServiceModel;


LIBAPI
GVariantSerializer::GVariantSerializer()
{
	g_variant_builder_init(&builder, G_VARIANT_TYPE_TUPLE);
}

LIBAPI
GVariantSerializer::SerializedType GVariantSerializer::EndPack() {
	return g_variant_builder_end(&builder);
}

LIBAPI
void GVariantSerializer::Serialize(uint32_t args)
{
	g_variant_builder_add(&builder, "u", args);
}

LIBAPI
void GVariantSerializer::Serialize(int32_t args)
{
	g_variant_builder_add(&builder, "i", args);
}

LIBAPI
void GVariantSerializer::Serialize(int64_t args)
{
	g_variant_builder_add(&builder, "x", args);
}

LIBAPI
void GVariantSerializer::Serialize(uint64_t args)
{
	g_variant_builder_add(&builder, "t", args);
}

LIBAPI
void GVariantSerializer::Serialize(bool args)
{
	g_variant_builder_add(&builder, "b", args);
}

LIBAPI
void GVariantSerializer::Serialize(double args)
{
	g_variant_builder_add(&builder, "d", args);
}

LIBAPI
void GVariantSerializer::Serialize(std::string args)
{
	g_variant_builder_add(&builder, "s", args.c_str());
}

LIBAPI
void GVariantSerializer::Serialize(std::vector<uint8_t> const& args)
{
	if (args.empty())
	{
		g_variant_builder_add_value(&builder, g_variant_new_array(G_VARIANT_TYPE_BYTE, nullptr, 0));
	}
	else
	{
		GVariantBuilder arrayBuilder;
		g_variant_builder_init(&arrayBuilder, G_VARIANT_TYPE_ARRAY);

		for(auto& obj : args)
		{
			g_variant_builder_add(&arrayBuilder, "y", obj);
		}

		g_variant_builder_add_value(&builder, g_variant_builder_end(&arrayBuilder));
	}
}

LIBAPI
void GVariantSerializer::Serialize(std::vector<int32_t> const& args)
{
	if (args.empty())
	{
		g_variant_builder_add_value(&builder, g_variant_new_array(G_VARIANT_TYPE_INT32, nullptr, 0));
	}
	else
	{
		GVariantBuilder arrayBuilder;
		g_variant_builder_init(&arrayBuilder, G_VARIANT_TYPE_ARRAY);

		for(auto& obj : args)
		{
			g_variant_builder_add(&arrayBuilder, "i", obj);
		}

		g_variant_builder_add_value(&builder, g_variant_builder_end(&arrayBuilder));
	}
}

LIBAPI
void GVariantSerializer::Serialize(std::vector<uint32_t> const& args)
{
	if (args.empty())
	{
		g_variant_builder_add_value(&builder, g_variant_new_array(G_VARIANT_TYPE_INT32, nullptr, 0));
	}
	else
	{
		GVariantBuilder arrayBuilder;
		g_variant_builder_init(&arrayBuilder, G_VARIANT_TYPE_ARRAY);

		for(auto& obj : args)
		{
			g_variant_builder_add(&arrayBuilder, "u", obj);
		}

		g_variant_builder_add_value(&builder, g_variant_builder_end(&arrayBuilder));
	}
}

LIBAPI
void GVariantSerializer::Serialize(std::vector<int64_t> const& args)
{
	if (args.empty())
	{
		g_variant_builder_add_value(&builder, g_variant_new_array(G_VARIANT_TYPE_INT64, nullptr, 0));
	}
	else
	{
		GVariantBuilder arrayBuilder;
		g_variant_builder_init(&arrayBuilder, G_VARIANT_TYPE_ARRAY);

		for(auto& obj : args)
		{
			g_variant_builder_add(&arrayBuilder, "x", obj);
		}

		g_variant_builder_add_value(&builder, g_variant_builder_end(&arrayBuilder));
	}
}

LIBAPI
GVariantDeserializer::GVariantDeserializer(SerializedType p) : variant(p) {
	dlog_print(DLOG_DEBUG, "TFC-RPC", "Got to serialize: %d", p);
	auto str = g_variant_print(p, true);
	dlog_print(DLOG_DEBUG, "TFC-RPC", "The value is: %s", str);
	g_free(str);

	if(!g_variant_type_is_tuple(g_variant_get_type(p)))
	{
		GVariant* arr[] = { p };
		p = g_variant_new_tuple(arr, 1);
	}

	this->maxChild = g_variant_n_children(p);
	this->currentChild = 0;

	//TODO change to variant serialization and perform safety checking
	g_variant_iter_init(&iter, p);
}

LIBAPI
GVariantDeserializer::GVariantAutoPtr GVariantDeserializer::NextField() {
	auto next = g_variant_iter_next_value(&iter);
	TFCAssert<Serialization::SerializationException>(next != nullptr, "No more field to serialize");
	return GVariantAutoPtr { next };
}


template<>
LIBAPI
int32_t GVariantDeserializer::DeserializeImpl<int32_t>(SerializedType obj)
{
	return g_variant_get_int32(obj);
}

template<>
LIBAPI
int64_t GVariantDeserializer::DeserializeImpl<int64_t>(SerializedType obj)
{
	return g_variant_get_int64(obj);
}

template<>
LIBAPI
uint32_t GVariantDeserializer::DeserializeImpl<uint32_t>(SerializedType obj)
{
	return g_variant_get_uint32(obj);
}

template<>
LIBAPI
double GVariantDeserializer::DeserializeImpl<double>(SerializedType obj)
{
	return g_variant_get_double(obj);
}

#include <dlog.h>

template<>
LIBAPI
std::string GVariantDeserializer::DeserializeImpl<std::string>(SerializedType strVariant)
{
	gsize len = 0;
	auto theStr = g_variant_get_string(strVariant, &len);
	std::string res(theStr, len);
	g_variant_unref(strVariant);
	return res;
}

template<>
LIBAPI
bool GVariantDeserializer::DeserializeImpl<bool>(SerializedType obj)
{
	return g_variant_get_boolean(obj);
}



LIBAPI
TFC::ServiceModel::GDBusClient::GDBusClient(GDBusConfiguration const& config,
		const char* objectPath, const char* interfaceName) : objectPath(objectPath), interfaceName(interfaceName) {
	this->busType = config.busType;

	TFCAssert<ArgumentException>(objectPath != nullptr && objectPath[0] != '\0', "Object path cannot be null or empty string.");
	TFCAssert<ArgumentException>(interfaceName != nullptr && interfaceName[0] != '\0', "Interface name cannot be null or empty string.");

	GError* err = nullptr;

	// Check if the user speficy bus type NONE
	if(this->busType == G_BUS_TYPE_NONE)
	{
		std::string realPath("unix:path=");
		realPath.append(config.busPath);

		dlog_print(DLOG_DEBUG, "RPC-Test", "Create connection for address %s", realPath.c_str());

		// Create connection using direct addressing
		this->handle = g_dbus_connection_new_for_address_sync(realPath.c_str(), G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT, nullptr, nullptr, &err);
		//g_dbus_connection_set_exit_on_close((GDBusConnection*)this->handle, false);
	}
	else if(this->busType == G_BUS_TYPE_SYSTEM || this->busType == G_BUS_TYPE_SESSION)
	{
		// Create connection using DBus proxy
		this->handle = g_dbus_proxy_new_for_bus_sync(config.busType, config.proxyFlags, nullptr, config.busName, objectPath, interfaceName, nullptr, &err);
	}
	dlog_print(DLOG_DEBUG, "RPC-Test", "After new proxy for: %s %s %s, result %d", config.busName, objectPath, interfaceName, handle);


	if(err != nullptr) {
		dlog_print(DLOG_ERROR, "RPC-Test", "Error when init client: %s", err->message);

		std::string errStr("Cannot initialize client for object path: ");
		errStr += objectPath;
		errStr += "; Reason: ";
		errStr += err->message;

		throw GDBusException(std::move(errStr));
	}

	RegisterEvent();
}

namespace {

class GErrorDeleter
{
public:
	void operator()(GError* err)
	{
		if(err != nullptr)
			g_error_free(err);
	}
};

class GDeleter
{
public:
	void operator()(gpointer ptr)
	{
		if(ptr != nullptr)
			g_free(ptr);
	}
};

class CDeleter
{
public:
	void operator()(void* ptr)
	{
		if(ptr != nullptr)
			free(ptr);
	}
};

}

LIBAPI
GVariant* TFC::ServiceModel::GDBusClient::RemoteCall(const char* methodName, GVariant* parameter) {

	if(methodName == nullptr || methodName[0] == '\0')
		throw ArgumentException("Method name cannot be null or empty string.");

	dlog_print(DLOG_DEBUG, "RPC-Test", "Calling: %s", methodName);

	GError* err = nullptr;
	GVariant* ptr = nullptr;

	// Call the remote method based on the bus type
	if(busType == G_BUS_TYPE_NONE)
		ptr = g_dbus_connection_call_sync((GDBusConnection*)this->handle, nullptr, objectPath.c_str(), interfaceName.c_str(), methodName, parameter, nullptr, G_DBUS_CALL_FLAGS_NONE, G_MAXINT, nullptr, &err);
	else
		ptr = g_dbus_proxy_call_sync((GDBusProxy*)this->handle, methodName, parameter, G_DBUS_CALL_FLAGS_NONE, G_MAXINT, nullptr, &err);

	dlog_print(DLOG_DEBUG, "RPC-Test", "Result: %d", ptr);

	if(err != nullptr)
	{
		std::unique_ptr<GError, GErrorDeleter> errPtr(err);

		if(!g_dbus_error_is_remote_error(err))
		{
			dlog_print(DLOG_ERROR, "RPC-Test", "Error when calling: %s", err->message);
			throw Core::InvocationException("Error");
		}
		else
		{
			std::string typeName(g_quark_to_string(err->domain));

			dlog_print(DLOG_ERROR, "RPC-Test", "Error when calling: %s => %s", typeName.c_str(), err->message);

			try
			{
				auto& info = Core::FindTypeByName(typeName);

				std::regex errRegex(R"REGEX(^GDBus\.Error:[A-Za-z0-9._]*:\s?(.*))REGEX");
				std::cmatch match;

				std::string errMessage;

				if(std::regex_match(err->message, match, errRegex))
				{
					errMessage = match[1].str();
				}
				else
				{
					errMessage = err->message;
				}

				try
				{
					info.Throw(errMessage.c_str());
				}
				catch(Core::FunctionNotFoundException const& ex)
				{
					info.Throw();
				}
			}
			catch(Core::ReflectionException const& ex)
			{
				throw Core::InvocationException(err->message);
			}
		}
	}


	int child = g_variant_n_children(ptr);
	if(child != 0)
	{
		auto child = g_variant_get_child_value(ptr, 0);
		g_variant_unref(ptr);

		auto debug = g_variant_print(child, true);
		dlog_print(DLOG_DEBUG, "TFC-RPC", "Client receive response: %s", debug);
		g_free(debug);

		return child;
	}

	return ptr;
}

LIBAPI
TFC::ServiceModel::GDBusClient::~GDBusClient()
{
	if(this->handle)
	{
		if(this->busType == G_BUS_TYPE_NONE)
		{
			g_dbus_connection_close_sync((GDBusConnection*) this->handle, nullptr, nullptr);
		}
		else
		{
			g_signal_handlers_disconnect_by_data(this->handle, this);
		}

		g_object_unref(this->handle);
		this->handle = nullptr;
	}
}


LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Finalize()
{
	g_variant_unref(variant);
}

LIBAPI
TFC::ServiceModel::GDBusInterfaceDefinition::GDBusInterfaceDefinition() {
	this->interfaceInfo.ref_count = -1;
	this->interfaceInfo.name = nullptr;

	this->methodInfoList.push_back(nullptr);
	this->signalInfoList.push_back(nullptr);

	this->interfaceInfo.methods = &this->methodInfoList[0];
	this->interfaceInfo.properties = nullptr;
	this->interfaceInfo.signals = &this->signalInfoList[0];
	this->interfaceInfo.annotations = nullptr;
}

LIBAPI
void TFC::ServiceModel::GDBusInterfaceDefinition::SetInterfaceName(
		std::string&& value) {
	this->interfaceNameStd = std::move(value);

	// Refresh interfaceInfo.name
	this->interfaceInfo.name = const_cast<char*>(this->interfaceNameStd.c_str());
}

LIBAPI
void TFC::ServiceModel::GDBusInterfaceDefinition::SetInterfaceName(
		std::string const& value) {
	this->interfaceNameStd = value;

	// Refresh interfaceInfo.name
	this->interfaceInfo.name = const_cast<char*>(this->interfaceNameStd.c_str());
}

LIBAPI
void TFC::ServiceModel::GDBusInterfaceDefinition::RegisterFunction(
		std::string const& funcName,
		std::vector<std::string> const& args,
		std::string const& retType)
{
	GDBusMethodInfo* m = new GDBusMethodInfo;

	m->ref_count = -1;
	m->name = g_strdup(funcName.c_str());
	m->annotations = nullptr;
	m->in_args = new GDBusArgInfo*[args.size() + 1] {};

	int ctr = 0;
	for(auto& arg : args)
	{
		auto& store = m->in_args[ctr];
		store = new GDBusArgInfo;
		store->name = g_strdup_printf("arg%d", ctr);
		store->ref_count = -1;
		store->signature = g_strdup(arg.c_str());
		store->annotations = nullptr;

		ctr++;
	}

	m->out_args = new GDBusArgInfo*[retType.empty() ? 1 : 2] {};

	if(!retType.empty())
	{
		auto& store = m->out_args[0];
		store = new GDBusArgInfo;
		store->name = g_strdup("argout");
		store->ref_count = -1;
		store->signature = g_strdup(retType.c_str());
		store->annotations = nullptr;
	}

	int lastIndex = this->methodInfoList.size();
	this->methodInfoList[lastIndex - 1] = m;
	this->methodInfoList.push_back(nullptr);

	// Refresh the address
	this->interfaceInfo.methods = &this->methodInfoList[0];
}

LIBAPI
void TFC::ServiceModel::GDBusInterfaceDefinition::RegisterEvent(
		const std::string& eventName, const std::string& arg)
{
	GDBusSignalInfo* s = new GDBusSignalInfo;

	s->ref_count = -1;
	s->name = g_strdup(eventName.c_str());
	s->annotations = nullptr;

	// Store argument
	s->args = new GDBusArgInfo*[2] {};

	auto store = new GDBusArgInfo;
	store->name = "arg";
	store->ref_count = -1;
	store->signature = g_strdup(arg.c_str());
	store->annotations = nullptr;

	s->args[0] = store;

	int lastIndex = this->signalInfoList.size();
	this->signalInfoList[lastIndex - 1] = s;
	this->signalInfoList.push_back(nullptr);

	// Refresh the address
	this->interfaceInfo.signals = &this->signalInfoList[0];
}

LIBAPI
TFC::ServiceModel::GDBusInterfaceDefinition::~GDBusInterfaceDefinition()
{
	for(auto methodInfo : this->methodInfoList)
	{
		if(methodInfo == nullptr)
			break;

		dlog_print(DLOG_DEBUG, "TFC-Debug", "Destroy methodInfo name");
		g_free(methodInfo->name);

		auto inArgs = methodInfo->in_args;

		int cnt = 0;

		while(*inArgs != nullptr)
		{
			auto& inArg = *inArgs;

			dlog_print(DLOG_DEBUG, "TFC-Debug", "Destroy inArgs %d", ++cnt);

			dlog_print(DLOG_DEBUG, "TFC-Debug", "Destroy inArgs name");
			g_free(inArg->name);

			dlog_print(DLOG_DEBUG, "TFC-Debug", "Destroy inArgs signature");
			g_free(inArg->signature);

			dlog_print(DLOG_DEBUG, "TFC-Debug", "Destroy inArgs completely");
			g_free(inArg);
			inArgs++;
		}

		delete[] methodInfo->in_args;

		auto& outArg = methodInfo->out_args[0];

		if(outArg != nullptr)
		{
			g_free(outArg->name);
			g_free(outArg->signature);
			delete outArg;
		}
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Destroy outArgs completely");
		delete[] methodInfo->out_args;

		dlog_print(DLOG_DEBUG, "TFC-Debug", "Destroy methodInfo completely");
		delete methodInfo;
	}

	for(auto signalInfo : signalInfoList)
	{
		if(signalInfo == nullptr)
			break;

		g_free(signalInfo->name);
		g_free(signalInfo->args[0]->signature);
		delete[] signalInfo->args;
	}

	dlog_print(DLOG_DEBUG, "TFC-Debug", "Complete destructor");
}

void TFC::ServiceModel::GDBusServer::OnBusAcquiredCallback(
		GDBusConnection* connection, const gchar* name, gpointer user_data) {
	static_cast<GDBusServer*>(user_data)->OnBusAcquired(connection, name);
}

void TFC::ServiceModel::GDBusServer::OnNameAcquiredCallback(
		GDBusConnection* connection, const gchar* name, gpointer user_data) {
	static_cast<GDBusServer*>(user_data)->OnNameAcquired(connection, name);
}

void TFC::ServiceModel::GDBusServer::OnNameLostCallback(
		GDBusConnection* connection, const gchar* name, gpointer user_data) {
	static_cast<GDBusServer*>(user_data)->OnNameLost(connection, name);
}

GDBusInterfaceVTable const TFC::ServiceModel::GDBusServer::defaultVtable;
std::regex dotRegex("\\.");

void TFC::ServiceModel::GDBusServer::OnBusAcquired(GDBusConnection* connection,
		const gchar* name) {

	std::cout << "Bus acq\n";

	for(auto& obj : this->objectList)
	{
		auto& serverObj = obj.second;

		std::string objPath = rootPath;
		objPath += std::regex_replace(serverObj->GetName(), dotRegex, "/");

		GError* errPtr = nullptr;

		std::cout << "Object path: " << objPath << "\n";

		for(auto& iface : serverObj->GetInterfaceList())
		{
			g_dbus_connection_register_object(connection,
										objPath.c_str(),
										const_cast<GDBusInterfaceInfo*>(&(iface->GetInterfaceInfo())),
										&defaultVtable,
										this,
										nullptr,
										&errPtr);

			dlog_print(DLOG_DEBUG, LOG_TAG, "Registering %s (%s)", objPath.c_str(), iface->GetInterfaceInfo().name);

			if(errPtr != nullptr)
			{
				dlog_print(DLOG_ERROR, LOG_TAG, "Error registering: %s", errPtr->message);

				std::stringstream ss;

				ss << "Error registering object at path " << objPath;
				ss << ": " << errPtr->message << ".";

				g_error_free(errPtr);

				throw GDBusException(ss.str());
			}
		}
	}

	this->connectionList.push_back(connection);
}

void TFC::ServiceModel::GDBusServer::OnNameAcquired(GDBusConnection* connection,
		const gchar* name) {

	std::cout << "Name acquired: " << name << std::endl;
}

void TFC::ServiceModel::GDBusServer::OnNameLost(GDBusConnection* connection,
		const gchar* name) {

}

LIBAPI
TFC::ServiceModel::GDBusServer::GDBusServer(Configuration const& config) :
	busId(0), server(nullptr), config(config)
{

}

static gboolean
GDBusServerAuthorizePeers (GDBusAuthObserver *observer,
                           GIOStream         *stream,
                           GCredentials      *credentials,
                           gpointer           user_data)
{
	return true;
}

LIBAPI
void TFC::ServiceModel::GDBusServer::Initialize()
{
	GError* err = nullptr;

	rootPath = "/";
	rootPath += std::regex_replace(this->config.busName, dotRegex, "/");
	rootPath += "/";

	if(config.busType == GBusType::G_BUS_TYPE_NONE)
	{
		// Not a message bus
		std::unique_ptr<gchar, GDeleter> guid(g_dbus_generate_guid());

		auto serverFlags = G_DBUS_SERVER_FLAGS_RUN_IN_THREAD;

		std::string serverAddress("unix:path=");
		serverAddress.append(config.busPath);

		std::cout << "The path: " << serverAddress << '\n';

		auto authObserver = g_dbus_auth_observer_new();

		g_signal_connect (authObserver,
						  "authorize-authenticated-peer",
						  G_CALLBACK (GDBusServerAuthorizePeers),
						  (gpointer) this);

		server = g_dbus_server_new_sync(serverAddress.c_str(), serverFlags, guid.get(), authObserver, nullptr, &err);

		if(err != nullptr)
		{
			std::unique_ptr<GError, GErrorDeleter> errPtr(err);
			throw TFC::RuntimeException(errPtr->message);
		}

		g_dbus_server_start(server);

		void (*func)(::GDBusServer*, GDBusConnection*, gpointer) =
				[] (::GDBusServer *server, GDBusConnection *connection, gpointer user_data)
				{
					g_object_ref (connection);
					auto thiz = reinterpret_cast<GDBusServer*>(user_data);
					std::cout << "Registering names \n";
					thiz->OnBusAcquired(connection, nullptr);
					g_signal_connect(connection, "closed", G_CALLBACK (&GDBusServer::OnConnectionClosedCallback), user_data);
				};

		g_signal_connect(server, "new-connection", G_CALLBACK (func), (gpointer)this);

	}
	else if(config.busType == G_BUS_TYPE_SESSION || config.busType == G_BUS_TYPE_SYSTEM)
	{
		this->busId = g_bus_own_name(
								config.busType,
								config.busName.c_str(),
								config.nameOwnerFlags,
								OnBusAcquiredCallback,
								OnNameAcquiredCallback,
								OnNameLostCallback,
								this,
								nullptr);
	}
}

void TFC::ServiceModel::GDBusServer::OnMethodCallCallback(
		GDBusConnection* connection, const gchar* sender,
		const gchar* object_path, const gchar* interface_name,
		const gchar* method_name, GVariant* parameters,
		GDBusMethodInvocation* invocation, gpointer user_data) {
	static_cast<GDBusServer*>(user_data)->OnMethodCall(
			connection, sender,
			object_path, interface_name,
			method_name, parameters,
			invocation);
}

void TFC::ServiceModel::GDBusServer::OnMethodCall(GDBusConnection* connection,
		const gchar* sender, const gchar* object_path,
		const gchar* interface_name, const gchar* method_name,
		GVariant* parameters, GDBusMethodInvocation* invocation) {

	std::cout << "Method Call (" << object_path << ", " << interface_name << ", " << method_name << ")\n";

	std::regex objectNameRegex("[A-Za-z0-9]+$");
	std::cmatch match;
	if(std::regex_search(object_path, match, objectNameRegex))
	{
		auto parsedObjectName = match.str();

		std::cout << "PArsed: " << parsedObjectName << "\n";

		auto obj = this->objectList.find(parsedObjectName);

		if(obj != this->objectList.end())
		{
			try
			{
				auto result = obj->second->Invoke(interface_name, method_name, parameters);

				gchar* printed = g_variant_print(result, true);
				dlog_print(DLOG_DEBUG, LOG_TAG, "Invocation result: %s", printed);
				g_free(printed);
				g_dbus_method_invocation_return_value(invocation, result);
				dlog_print(DLOG_DEBUG, LOG_TAG, "After return");
			}
			catch(std::exception& ex)
			{
				dlog_print(DLOG_DEBUG, LOG_TAG, "Type name: %s", typeid(ex).name());
				g_dbus_method_invocation_return_error(invocation, g_quark_from_string(typeid(ex).name()), 0, ex.what());
				//g_dbus_method_invocation_return_dbus_error(invocation, GetInterfaceName("", typeid(ex)).c_str(), ex.what());
			}
		}
	} else std::cout << "Failed parsing object name\n";
}

LIBAPI
void TFC::ServiceModel::GDBusServer::AddServerObject(IServerObject<GDBusChannel>* obj)
{
	std::string const& name = obj->GetName();
	this->objectList.emplace(name, std::unique_ptr<IServerObject<GDBusChannel>>(obj));

	obj->eventEventRaised += EventHandler(GDBusServer::EventCaptureHandler);
}

LIBAPI
void TFC::ServiceModel::GDBusServer::AddServerObject(std::unique_ptr<IServerObject<GDBusChannel>> obj)
{
	std::string const& name = obj->GetName();
	this->objectList.emplace(name, std::move(obj));
}

LIBAPI
TFC::ServiceModel::GDBusServer::~GDBusServer() {
	if(busId != 0)
		g_bus_unown_name(this->busId);

	if(server != nullptr)
		g_dbus_server_stop(this->server);
}


LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(int8_t& target) {
	g_variant_iter_next(&iter, "y", &target);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(int16_t& target) {
	g_variant_iter_next(&iter, "n", &target);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(int32_t& target) {
	g_variant_iter_next(&iter, "i", &target);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(int64_t& target) {
	g_variant_iter_next(&iter, "x", &target);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(uint8_t& target) {
	g_variant_iter_next(&iter, "y", &target);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(uint16_t& target) {
	g_variant_iter_next(&iter, "q", &target);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(uint32_t& target) {
	g_variant_iter_next(&iter, "u", &target);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(uint64_t& target) {
	g_variant_iter_next(&iter, "t", &target);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(std::string& target) {
	auto strVariant = g_variant_iter_next_value(&iter);
	gsize len = 0;
	auto theStr = g_variant_get_string(strVariant, &len);
	target = std::string(theStr, len);
	g_variant_unref(strVariant);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(bool& target) {
	gboolean tmp;
	g_variant_iter_next(&iter, "b", &tmp);
	target = tmp;
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(double& target) {
	g_variant_iter_next(&iter, "d", &target);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(std::vector<uint8_t>& target) {
	target.clear();
	auto arrVariant = g_variant_iter_next_value(&iter);
	auto arrIter = g_variant_iter_new(arrVariant);
	uint8_t value;
	while (g_variant_iter_next(arrIter, "y", &value)) {
		target.push_back(value);
	}
	g_variant_iter_free(arrIter);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(std::vector<int32_t>& target) {
	target.clear();
	dlog_print(DLOG_DEBUG, "TFC-RPC", "Deserialize vector");
	auto arrVariant = g_variant_iter_next_value(&iter);
	auto arrIter = g_variant_iter_new(arrVariant);
	int32_t value;
	while (g_variant_iter_next(arrIter, "i", &value)) {
		target.push_back(value);
	}
	g_variant_iter_free(arrIter);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(std::vector<uint32_t>& target) {
	target.clear();
	dlog_print(DLOG_DEBUG, "TFC-RPC", "Deserialize vector uint32_t");
	auto arrVariant = g_variant_iter_next_value(&iter);
	auto arrIter = g_variant_iter_new(arrVariant);
	int32_t value;
	while (g_variant_iter_next(arrIter, "u", &value)) {
		target.push_back(value);
	}
	g_variant_iter_free(arrIter);
}

LIBAPI
void TFC::ServiceModel::GVariantDeserializer::Deserialize(std::vector<int64_t>& target) {
	target.clear();
	auto arrVariant = g_variant_iter_next_value(&iter);
	auto arrIter = g_variant_iter_new(arrVariant);
	int64_t value;
	while (g_variant_iter_next(arrIter, "x", &value)) {
		target.push_back(value);
	}
	g_variant_iter_free(arrIter);
}

LIBAPI
GVariantSerializer TFC::ServiceModel::GVariantSerializer::CreateScope()
{
	// Just construct new object
	return {};
}

LIBAPI
void TFC::ServiceModel::GVariantSerializer::Serialize(GVariantSerializer& ser) {
	auto p = ser.EndPack();

	if(g_variant_n_children(p) == 0)
	{
		// Compability for GDBus which is angry if it is passed an empty tuple
		g_variant_unref(p);

		GVariantBuilder localBuilder;
		g_variant_builder_init(&localBuilder, G_VARIANT_TYPE_TUPLE);
		g_variant_builder_add(&localBuilder, "i", 0);
		p = g_variant_builder_end(&localBuilder);
	}

	g_variant_builder_add_value(&builder, p);

}

LIBAPI
GVariantDeserializer TFC::ServiceModel::GVariantDeserializer::DeserializeScope()
{
	auto val = g_variant_iter_next_value(&iter);

	auto str = g_variant_print(val, true);
	dlog_print(DLOG_DEBUG, "TFC-RPC", "The value of scope is: %s", str);
	g_free(str);

	TFCAssert(g_variant_type_is_tuple(g_variant_get_type(val)), "Invalid value. Scope value is not tuple.");
	return { val };
}

void TFC::ServiceModel::GDBusServer::OnConnectionClosedCallback(
		GDBusConnection* connection, gboolean remote_peer_vanished,
		GError* error, gpointer user_data) {
	auto thiz = static_cast<GDBusServer*>(user_data);
	dlog_print(DLOG_DEBUG, "RPC-Test-Connection", "Connection is closed");
	g_object_unref(connection);
	std::remove_if(thiz->connectionList.begin(), thiz->connectionList.end(), [connection] (GDBusConnection* data) { return data == connection; });
}

void TFC::ServiceModel::GDBusServer::EventCaptureHandler(IServerObject<GDBusChannel>* source, IServerObject<GDBusChannel>::EventEmissionInfo const& eventInfo)
{
	decltype(auto) name = source->GetName();

	dlog_print(DLOG_DEBUG, LOG_TAG, "EventCaptureHandler %s", name.c_str());

	std::string objPath = rootPath;
	objPath += std::regex_replace(name, dotRegex, "/");

	char const* ifaceName = nullptr;


	// Find interface
	for(auto& iface : source->GetInterfaceList())
	{
		auto signalPtr = iface->GetInterfaceInfo().signals;

		while(auto currSignal = *signalPtr)
		{
			if(eventInfo.eventName.compare(currSignal->name) == 0)
			{
				ifaceName = iface->GetInterfaceInfo().name;
				break;
			}
			else
				++signalPtr;
		}
	}

	dlog_print(DLOG_DEBUG, LOG_TAG, "Found interface %s and object path %s", ifaceName, objPath.c_str());
	auto paramArg = g_variant_ref_sink(eventInfo.eventArgument);

	for(auto connection : this->connectionList)
	{
		GError* err = nullptr;

		g_dbus_connection_emit_signal(connection, nullptr, objPath.c_str(), ifaceName, eventInfo.eventName.c_str(), paramArg, &err);


		dlog_print(DLOG_DEBUG, LOG_TAG, "Emit signal on connection");
		if(err != nullptr)
		{
			dlog_print(DLOG_ERROR, LOG_TAG, "Error on emit signal to connection: %s (%d)", err->message, err->code);
			g_error_free(err);
		}
	}

	g_variant_unref(paramArg);
}

void TFC::ServiceModel::GDBusClient::ReceiveEvent(const char* eventName,
		GVariant* eventArg) {
	auto str = g_variant_print(eventArg, true);
	dlog_print(DLOG_DEBUG, "TFC-RPC-EVENT", "The event arg of %s is: %s", eventName, str);
	g_free(str);

	this->eventEventReceived(this, { eventName, eventArg });
}

void TFC::ServiceModel::GDBusClient::RegisterEvent()
{
	if(busType == G_BUS_TYPE_NONE)
	{
		auto conn = (GDBusConnection*)this->handle;
		g_dbus_connection_signal_subscribe(conn, nullptr, interfaceName.c_str(), nullptr, objectPath.c_str(), nullptr, G_DBUS_SIGNAL_FLAGS_NONE, GDBusClient::GDBusConnectionReceiveSignal, this, nullptr);
	}
	else
	{
		auto proxy = (GDBusProxy*)this->handle;
		g_signal_connect(proxy, "g-signal", G_CALLBACK(GDBusClient::GDBusProxyReceiveSignal), this);
	}
}

void TFC::ServiceModel::GDBusClient::GDBusProxyReceiveSignal(GDBusProxy* proxy,
		gchar* sender_name, gchar* signal_name, GVariant* parameters,
		gpointer user_data) {
	static_cast<GDBusClient*>(user_data)->ReceiveEvent(signal_name, parameters);
}

void TFC::ServiceModel::GDBusClient::GDBusConnectionReceiveSignal(
		GDBusConnection* connection, const gchar* sender_name,
		const gchar* object_path, const gchar* interface_name,
		const gchar* signal_name, GVariant* parameters, gpointer user_data) {
	static_cast<GDBusClient*>(user_data)->ReceiveEvent(signal_name, parameters);
}
