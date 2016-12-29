/*
 * GDBusEndpoint.h
 *
 *  Created on: Dec 21, 2016
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_GDBUSENDPOINT_H_
#define TFC_SERVICEMODEL_GDBUSENDPOINT_H_

#include <glib-2.0/glib.h>
#include <glib-2.0/gio/gio.h>

namespace TFC {
namespace ServiceModel {

struct GVariantSerializer
{
	GVariantBuilder builder;

	typedef GVariant* PackType;
	GVariantSerializer();

	template<typename T>
	void Pack(T args);



	PackType EndPack();
};

struct GVariantDeserializer
{
	typedef GVariant* PackType;

	PackType variant;

	GVariantDeserializer(PackType p);

	template<typename T>
	T Unpack(int index);

	void Finalize();
};

struct GDBusConfiguration
{
	char const* busName;
	GBusType busType;
	GBusNameOwnerFlags nameOwnerFlags;
	GDBusProxyFlags proxyFlags;
};

class GDBusClient
{
private:
	void* handle;
public:
	GDBusClient(GDBusConfiguration const& config, char const* objectPath, char const* interfaceName);
	GVariant* RemoteCall(char const* methodName, GVariant* parameter);
};

class GDBusServerObjectList
{

};

class GDBusServer
{
private:
	static void OnBusAcquiredCallback(GDBusConnection *connection, const gchar* name, gpointer user_data);
	static void OnNameAcquiredCallback(GDBusConnection *connection, const gchar* name, gpointer user_data);
	static void OnNameLostCallback(GDBusConnection *connection, const gchar* name, gpointer user_data);

	void OnBusAcquired(GDBusConnection *connection, const gchar* name);
	void OnNameAcquired(GDBusConnection *connection, const gchar* name);
	void OnNameLost(GDBusConnection *connection, const gchar* name);

public:
	GDBusServer();
};

struct GDBusInterfaceDefinition
{
	void SetInterfaceName(std::string&& value);

	template<typename TFuncPtr>
	void RegisterFunction(TFuncPtr ptr);
};

struct GDBusChannel
{
	typedef GVariantSerializer 	 	 Serializer;
	typedef GVariantDeserializer 	 Deserializer;
	typedef GDBusClient			 	 Client;
	typedef GDBusServer			 	 Server;
	typedef GVariant* 			 	 PackType;
	typedef GDBusConfiguration		 ConfigurationType;
	typedef GDBusInterfaceDefinition InterfaceDefinition;
};

}
}


#endif /* TFC_SERVICEMODEL_GDBUSENDPOINT_H_ */
