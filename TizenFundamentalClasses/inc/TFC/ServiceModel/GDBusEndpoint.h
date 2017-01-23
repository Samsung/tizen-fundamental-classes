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
#include "TFC/Core/Reflection.h"
#include "TFC/ServiceModel/Includes.h"
#include "TFC/ServiceModel/ServerEndpoint.h"

#include "TFC/Serialization/ClassSerializer.h"

namespace TFC {
namespace ServiceModel {

TFC_ExceptionDeclare	(GDBusException, EndpointException);

struct GDBusChannel;

struct GVariantSerializer
{
	GVariantBuilder builder;

	typedef GVariant* SerializedType;
	GVariantSerializer();

	void Serialize(uint32_t args);
	void Serialize(int64_t args);
	void Serialize(int args);
	void Serialize(bool args);
	void Serialize(double args);
	void Serialize(std::string args);

	void Serialize(SerializedType p);

	SerializedType EndPack();
};

struct GVariantDeserializer
{
	typedef GVariant* SerializedType;

	SerializedType variant;

	GVariantDeserializer(SerializedType p);

	template<typename T>
	T DeserializeImpl(int index);

	template<typename T, typename = void>
	struct DeserializerSelector
	{
		static T Deserialize(GVariantDeserializer& deser, int index)
		{
			return deser.DeserializeImpl<T>(index);
		}
	};

	template<typename T>
	T Deserialize(int index)
	{
		dlog_print(DLOG_DEBUG, "TFC-RPC", "Index %d", index);
		return DeserializerSelector<T>::Deserialize(*this, index);
	}



	void Finalize();

private:

	GVariantIter iter;
	gsize maxChild;
	gsize currentChild;
};

template<typename T>
struct GVariantDeserializer::DeserializerSelector<T, Core::Metaprogramming::Void_T<typename Serialization::TypeSerializationInfoSelector<T>::Type>>
{
	static T Deserialize(GVariantDeserializer& deser, int index)
	{
		using namespace TFC::Serialization;

		auto innerVar = g_variant_get_child_value(deser.variant, index);
		return ClassDeserializer<GVariantDeserializer, T>::Deserialize(innerVar);
	}
};

template<typename T>
struct GVariantDeserializer::DeserializerSelector<T, Core::Metaprogramming::Void_T<typename std::enable_if<std::is_enum<T>::value>::type>>
{
	static T Deserialize(GVariantDeserializer& deser, int index)
	{
		return (T)deser.DeserializeImpl<typename std::underlying_type<T>::type>(index);
	}
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
public:
	struct Configuration
	{
		char const* interfacePrefix;
		char const* busName;
		GBusType busType;
		GBusNameOwnerFlags nameOwnerFlags;
	};

private:
	static void OnBusAcquiredCallback(GDBusConnection *connection, const gchar* name, gpointer user_data);
	static void OnNameAcquiredCallback(GDBusConnection *connection, const gchar* name, gpointer user_data);
	static void OnNameLostCallback(GDBusConnection *connection, const gchar* name, gpointer user_data);

	static void OnMethodCallCallback(GDBusConnection       *connection,
			const gchar           *sender,
            const gchar           *object_path,
            const gchar           *interface_name,
            const gchar           *method_name,
            GVariant              *parameters,
            GDBusMethodInvocation *invocation,
            gpointer               user_data);


	void OnBusAcquired(GDBusConnection *connection, const gchar* name);
	void OnNameAcquired(GDBusConnection *connection, const gchar* name);
	void OnNameLost(GDBusConnection *connection, const gchar* name);

	void OnMethodCall(GDBusConnection       *connection,
			const gchar           *sender,
            const gchar           *object_path,
            const gchar           *interface_name,
            const gchar           *method_name,
            GVariant              *parameters,
            GDBusMethodInvocation *invocation);

	static constexpr GDBusInterfaceVTable defaultVtable { OnMethodCallCallback, nullptr, nullptr };

	std::map<std::string, std::unique_ptr<IServerObject<GDBusChannel>>> objectList;
	guint busId;
	Configuration config;
public:
	GDBusServer(Configuration const& config);
	~GDBusServer();
	void Initialize();
	void AddServerObject(IServerObject<GDBusChannel>* obj);
	void AddServerObject(std::unique_ptr<IServerObject<GDBusChannel>> obj);
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
	void SetInterfaceName(std::string const& value);

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
struct GDBusTypeCode
{
	static constexpr char const* value = "r";
};

template<typename T>
struct GDBusTypeCode<T const&> : GDBusTypeCode<T>
{

};

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

	auto& typeDescription = Core::TypeInfo<typename Introspect::DeclaringType>::typeDescription;

	RegisterFunction(
			typeDescription.GetFunctionNameByPointer(ptr),
			GDBusSignatureBuilder<typename Introspect::ArgsTuple>::GetSignatureList(),
			GDBusTypeCode<typename Introspect::ReturnType>::value);
}


#endif /* TFC_SERVICEMODEL_GDBUSENDPOINT_H_ */
