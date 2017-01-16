/*
 * GDBusEndpoint.cpp
 *
 *  Created on: Dec 21, 2016
 *      Author: gilang
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
void GVariantSerializer::Serialize(int args)
{
	g_variant_builder_add(&builder, "i", args);
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
TFC::ServiceModel::GVariantDeserializer::GVariantDeserializer(SerializedType p) : variant(p) {
}

template<>
LIBAPI
int GVariantDeserializer::Deserialize<int>(int index)
{
	int res;
	g_variant_get_child(variant, index, "i", &res);
	return res;
}

template<>
LIBAPI
double GVariantDeserializer::Deserialize<double>(int index)
{
	double res;
	g_variant_get_child(variant, index, "d", &res);
	return res;
}

#include <dlog.h>

template<>
LIBAPI
std::string GVariantDeserializer::Deserialize<std::string>(int index)
{

	auto strVariant = g_variant_get_child_value(variant, index);
	gsize len = 0;
	auto theStr = g_variant_get_string(strVariant, &len);
	std::string res(theStr, len);
	g_variant_unref(strVariant);
	return res;
}





LIBAPI
TFC::ServiceModel::GDBusClient::GDBusClient(GDBusConfiguration const& config,
		const char* objectPath, const char* interfaceName) {
	this->handle = g_dbus_proxy_new_for_bus_sync(config.busType, config.proxyFlags, nullptr, config.busName, objectPath, interfaceName, nullptr, nullptr);
}



LIBAPI
GVariant* TFC::ServiceModel::GDBusClient::RemoteCall(const char* methodName, GVariant* parameter) {
	dlog_print(DLOG_DEBUG, "RPC-Test", "Calling: %s", methodName);
	return g_dbus_proxy_call_sync((GDBusProxy*)this->handle, methodName, parameter, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, nullptr);
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

	this->interfaceInfo.methods = &this->methodInfoList[0];
	this->interfaceInfo.properties = nullptr;
	this->interfaceInfo.signals = nullptr;
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
TFC::ServiceModel::GDBusInterfaceDefinition::~GDBusInterfaceDefinition()
{
	for(auto& methodInfo : this->methodInfoList)
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

		delete methodInfo->in_args;

		auto& outArg = methodInfo->out_args[0];

		if(outArg != nullptr)
		{
			g_free(outArg->name);
			g_free(outArg->signature);
			delete outArg;
		}
		dlog_print(DLOG_DEBUG, "TFC-Debug", "Destroy outArgs completely");
		delete methodInfo->out_args;

		dlog_print(DLOG_DEBUG, "TFC-Debug", "Destroy methodInfo completely");
		delete methodInfo;
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

void TFC::ServiceModel::GDBusServer::OnBusAcquired(GDBusConnection* connection,
		const gchar* name) {

	std::regex dotRegex("\\.");

	std::string rootPath("/");
	rootPath += std::regex_replace(this->config.busName, dotRegex, "/");
	rootPath += "/";

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

			if(errPtr != nullptr)
			{
				std::cout << "Error\n";
				std::cout << errPtr->message << "\n";
			}
		}
	}
}

void TFC::ServiceModel::GDBusServer::OnNameAcquired(GDBusConnection* connection,
		const gchar* name) {

	std::cout << "Name acquired: " << name << std::endl;
}

void TFC::ServiceModel::GDBusServer::OnNameLost(GDBusConnection* connection,
		const gchar* name) {
}

LIBAPI
TFC::ServiceModel::GDBusServer::GDBusServer(Configuration const& config) : config(config)
{

}

LIBAPI
void TFC::ServiceModel::GDBusServer::Initialize()
{
	this->busId = g_bus_own_name(
						config.busType,
						config.busName,
						config.nameOwnerFlags,
						OnBusAcquiredCallback,
						OnNameAcquiredCallback,
						OnNameLostCallback,
						this,
						nullptr);
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
			auto result = obj->second->Invoke(interface_name, method_name, parameters);

			gchar* printed = g_variant_print(result, true);
			dlog_print(DLOG_DEBUG, LOG_TAG, "Invocation result: %s", printed);
			g_free(printed);



			g_dbus_method_invocation_return_value(invocation, result);
		}
	} else std::cout << "Failed parsing object name\n";
}

LIBAPI
void TFC::ServiceModel::GDBusServer::AddServerObject(IServerObject<GDBusChannel>* obj)
{
	std::string const& name = obj->GetName();
	this->objectList.emplace(name, std::unique_ptr<IServerObject<GDBusChannel>>(obj));
}

LIBAPI
void TFC::ServiceModel::GDBusServer::AddServerObject(std::unique_ptr<IServerObject<GDBusChannel>> obj)
{
	std::string const& name = obj->GetName();
	this->objectList.emplace(name, std::move(obj));
}

LIBAPI
TFC::ServiceModel::GDBusServer::~GDBusServer() {
	g_bus_unown_name(this->busId);
}

LIBAPI
void TFC::ServiceModel::GVariantSerializer::Serialize(SerializedType p)
{
	g_variant_builder_add_value(&builder, p);
}
