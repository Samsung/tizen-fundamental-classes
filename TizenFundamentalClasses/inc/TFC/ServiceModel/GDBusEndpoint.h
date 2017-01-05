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
#include <vector>

#include "TFC/Core/Introspect.h"
#include "TFC/ServiceModel/Reflection.h"

namespace TFC {
namespace ServiceModel {

struct GVariantSerializer
{
	GVariantBuilder builder;

	typedef GVariant* SerializedType;
	GVariantSerializer();

	template<typename T>
	void Serialize(T args);



	SerializedType EndPack();
};

struct GVariantDeserializer
{
	typedef GVariant* SerializedType;

	SerializedType variant;

	GVariantDeserializer(SerializedType p);

	template<typename T>
	T Deserialize(int index);

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

class GDBusInterfaceDefinition
{
public:
	GDBusInterfaceDefinition();
	GDBusInterfaceDefinition(GDBusInterfaceDefinition const&) = delete;
	GDBusInterfaceDefinition(GDBusInterfaceDefinition&&) = delete;

	GDBusInterfaceDefinition& operator=(GDBusInterfaceDefinition const&) = delete;
	GDBusInterfaceDefinition& operator=(GDBusInterfaceDefinition&&) = delete;
	void SetInterfaceName(std::string&& value);

	template<typename TFuncPtr>
	void RegisterFunction(TFuncPtr ptr);

	~GDBusInterfaceDefinition();

private:
	GDBusInterfaceInfo interfaceInfo;
	std::string interfaceNameStd;

	std::vector<GDBusMethodInfo*> methodInfoList;

	void RegisterFunction(std::string const& funcName, std::vector<std::string> const& args, std::string const& retType);

public:
	GDBusInterfaceInfo const& GetInterfaceInfo() const { return this->interfaceInfo; };
};

template<typename T>
struct GDBusSignatureBuilder;

template<typename... TArgs>
struct GDBusSignatureFiller;

template<typename T>
struct GDBusTypeCode;

#define GDBus_TypeCode_Define(TYPE, CHAR) \
		template<> \
		struct GDBusTypeCode< TYPE > \
		{ \
			static constexpr char const* value = CHAR; \
		};

GDBus_TypeCode_Define(unsigned char, "y");
GDBus_TypeCode_Define(bool, "b");
GDBus_TypeCode_Define(short, "n");
GDBus_TypeCode_Define(unsigned short, "q");
GDBus_TypeCode_Define(int, "i");
GDBus_TypeCode_Define(unsigned int, "u");
GDBus_TypeCode_Define(long long, "x");
GDBus_TypeCode_Define(unsigned long long, "t");
GDBus_TypeCode_Define(double, "d");
GDBus_TypeCode_Define(std::string, "s");
GDBus_TypeCode_Define(void, "");

template<typename... TArgs>
struct GDBusSignatureBuilder<std::tuple<TArgs...>>
{
	static std::vector<std::string> GetSignatureList()
	{
		std::vector<std::string> ret;
		GDBusSignatureFiller<TArgs...>::GetSignature(ret);
		return ret;
	}
};



template<typename TCurrent, typename... TArgs>
struct GDBusSignatureFiller<TCurrent, TArgs...>
{
	static void GetSignature(std::vector<std::string>& param)
	{
		param.push_back(GDBusTypeCode<TCurrent>::value);
		GDBusSignatureFiller<TArgs...>::GetSignature(param);
	}
};

template<>
struct GDBusSignatureFiller<>
{
	static void GetSignature(std::vector<std::string>& param)
	{

	}
};

struct GDBusChannel
{
	typedef GVariantSerializer 	 	 Serializer;
	typedef GVariantDeserializer 	 Deserializer;
	typedef GDBusClient			 	 Client;
	typedef GDBusServer			 	 Server;
	typedef GVariant* 			 	 SerializedType;
	typedef GDBusConfiguration		 ConfigurationType;
	typedef GDBusInterfaceDefinition InterfaceDefinition;
};

}
}

template<typename TFuncPtr>
inline void TFC::ServiceModel::GDBusInterfaceDefinition::RegisterFunction(
		TFuncPtr ptr) {

	typedef Core::Introspect::MemberFunction<TFuncPtr> Introspect;

	auto& typeDescription = TypeInfo<typename Introspect::DeclaringType>::typeDescription;

	RegisterFunction(
			typeDescription.GetFunctionNameByPointer(ptr),
			GDBusSignatureBuilder<typename Introspect::ArgsTuple>::GetSignatureList(),
			GDBusTypeCode<typename Introspect::ReturnType>::value);
}


#endif /* TFC_SERVICEMODEL_GDBUSENDPOINT_H_ */
