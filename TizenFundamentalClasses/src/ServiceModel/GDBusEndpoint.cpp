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
GVariantSerializer::PackType GVariantSerializer::EndPack() {
	return g_variant_builder_end(&builder);
}

template<>
LIBAPI
void GVariantSerializer::Pack<int>(int args)
{
	g_variant_builder_add(&builder, "i", args);
}

template<>
LIBAPI
void GVariantSerializer::Pack<double>(double args)
{
	g_variant_builder_add(&builder, "d", args);
}

template<>
LIBAPI
void GVariantSerializer::Pack<std::string>(std::string args)
{
	g_variant_builder_add(&builder, "s", args.c_str());
}

LIBAPI
TFC::ServiceModel::GVariantDeserializer::GVariantDeserializer(PackType p) : variant(p) {
}

template<>
LIBAPI
int GVariantDeserializer::Unpack<int>(int index)
{
	int res;
	g_variant_get_child(variant, index, "i", &res);
	return res;
}

template<>
LIBAPI
double GVariantDeserializer::Unpack<double>(int index)
{
	double res;
	g_variant_get_child(variant, index, "d", &res);
	return res;
}

template<>
LIBAPI
std::string GVariantDeserializer::Unpack<std::string>(int index)
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
TFC::ServiceModel::GDBusClient::GDBusClient(const char* busName,
		const char* objectPath, const char* interfaceName) {
	this->handle = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, G_DBUS_PROXY_FLAGS_NONE, nullptr, busName, objectPath, interfaceName, nullptr, nullptr);
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
