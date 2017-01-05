/*
 * GDBusEndpoint.cpp
 *
 *  Created on: Dec 21, 2016
 *      Author: gilang
 */


#include "TFC/Core.h"
#include "TFC/ServiceModel/GDBusEndpoint.h"

#include <glib-2.0/gio/gio.h>

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

template<>
LIBAPI
void GVariantSerializer::Serialize<int>(int args)
{
	g_variant_builder_add(&builder, "i", args);
}

template<>
LIBAPI
void GVariantSerializer::Serialize<double>(double args)
{
	g_variant_builder_add(&builder, "d", args);
}

template<>
LIBAPI
void GVariantSerializer::Serialize<std::string>(std::string args)
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

#include <dlog.h>



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
