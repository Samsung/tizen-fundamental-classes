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

class GVariantDeleter
{
public:
	void operator()(GVariant* ptr)
	{
		if(ptr != nullptr)
			g_variant_unref(ptr);
	}
};

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

	void Serialize(std::vector<int> const& args);
	void Serialize(std::vector<uint8_t> const& args);

	GVariantSerializer CreateScope();
	void Serialize(GVariantSerializer& p);

	template<typename T>
	void Serialize(std::vector<T> const& args)
	{
		using namespace TFC::Serialization;

		SerializedType tmp = nullptr;

		if(args.empty())
		{
			tmp = g_variant_new_array(G_VARIANT_TYPE_INT64, nullptr, 0);
		}
		else
		{
			GVariantBuilder arrayBuilder;
			g_variant_builder_init(&arrayBuilder, G_VARIANT_TYPE_ARRAY);

			for(auto& obj : args)
			{
				g_variant_builder_add_value(&arrayBuilder, GenericSerializer<GVariantSerializer, T>::Serialize(obj));
			}
			tmp = g_variant_builder_end(&arrayBuilder);
		}
		Serialize(tmp);
	}

	SerializedType EndPack();
};

struct GVariantDeserializer
{
	typedef GVariant* SerializedType;
	typedef std::unique_ptr<GVariant, GVariantDeleter> GVariantAutoPtr;

	SerializedType variant;

	GVariantDeserializer(SerializedType p);

	template<typename T>
	T DeserializeImpl(SerializedType index);

	template<typename, typename> friend struct DeserializeSelector;

	template<typename T, typename = void>
	struct DeserializerSelector
	{
		static T Deserialize(GVariantDeserializer& deser, int index)
		{
			auto next = deser.NextField();
			return deser.DeserializeImpl<T>(next.get());

		}
	};

	/*
	template<typename T>
	T Deserialize(int index)
	{
		dlog_print(DLOG_DEBUG, "TFC-RPC", "Index %d", index);
		return DeserializerSelector<T>::Deserialize(*this, index);
	}
	*/
	void Deserialize(int8_t& target);
	void Deserialize(int16_t& target);
	void Deserialize(int32_t& target);
	void Deserialize(int64_t& target);

	void Deserialize(uint8_t& target);
	void Deserialize(uint16_t& target);
	void Deserialize(uint32_t& target);
	void Deserialize(uint64_t& target);

	void Deserialize(std::string& target);
	void Deserialize(bool& target);
	void Deserialize(double& target);

	void Deserialize(std::vector<uint8_t>& target);

	template<typename T>
	void Deserialize(std::vector<T>& target)
	{
		target.clear();
		auto arrVariant = g_variant_iter_next_value(&iter);
		auto arrIter = g_variant_iter_new(arrVariant);
		while (auto value = g_variant_iter_next_value(arrIter)) {

			typedef TFC::Serialization::GenericDeserializer<GVariantDeserializer, T> Deserializer;

			GVariantDeserializer ser(value);
			target.push_back(Deserializer::Deserialize(ser));
			g_variant_unref(value);
		}
		g_variant_iter_free(arrIter);
	}

	void Deserialize(SerializedType& composite);

	GVariantDeserializer DeserializeScope();

	void Finalize();

private:

	GVariantIter iter;
	gsize maxChild;
	gsize currentChild;

	GVariantAutoPtr NextField();
};

template<typename T>
struct GVariantDeserializer::DeserializerSelector<T, Core::Metaprogramming::Void_T<typename Serialization::TypeSerializationInfoSelector<T>::Type>>
{
	static T Deserialize(GVariantDeserializer& deser, int index)
	{
		using namespace TFC::Serialization;

		auto innerVar = deser.NextField();
		return ClassDeserializer<GVariantDeserializer, T>::Deserialize(innerVar.get(), false);
	}
};

template<typename T>
struct GVariantDeserializer::DeserializerSelector<T, Core::Metaprogramming::Void_T<typename std::enable_if<std::is_enum<T>::value>::type>>
{
	static T Deserialize(GVariantDeserializer& deser, int index)
	{
		auto innerVar = deser.NextField();
		return (T)deser.DeserializeImpl<typename std::underlying_type<T>::type>(innerVar.get());
	}
};

template<typename T>
struct GVariantDeserializer::DeserializerSelector<std::vector<T>, Core::Metaprogramming::Void_T<typename Serialization::TypeSerializationInfoSelector<T>::Type>>
{
	static std::vector<T> Deserialize(GVariantDeserializer& deser, int index)
	{
		using namespace TFC::Serialization;

		auto innerVar = deser.NextField();
		TFCAssert<Serialization::SerializationException>(g_variant_get_type(innerVar.get()) == G_VARIANT_TYPE_ARRAY, "The specified field is not an array type");

		std::vector<T> ret;

		auto iter = g_variant_iter_new(innerVar.get());

		for(auto current = g_variant_iter_next_value(iter); current != nullptr; current = g_variant_iter_next_value(iter))
		{
			ret.push_back(GenericDeserializer<GVariantDeserializer, T>::Deserialize(current));
		}

		g_variant_iter_free(iter);

		return ret;
	}
};



struct GDBusConfiguration
{
	char const* busName;
	GBusType busType;
	GBusNameOwnerFlags nameOwnerFlags;
	GDBusProxyFlags proxyFlags;
	char const* busPath;
};

class GDBusClient
{
private:
	void* handle;
	std::string objectPath;
	std::string interfaceName;
	GBusType busType;
public:
	GDBusClient(GDBusConfiguration const& config, char const* objectPath, char const* interfaceName);
	GVariant* RemoteCall(char const* methodName, GVariant* parameter);
	bool IsClosed() { return busType != G_BUS_TYPE_NONE || g_dbus_connection_is_closed((GDBusConnection*)handle); }
	~GDBusClient();
};

class GDBusServerObjectList
{

};

class GDBusServer
{
public:
	struct Configuration
	{
		std::string interfacePrefix;
		std::string busName;
		GBusType busType;
		GBusNameOwnerFlags nameOwnerFlags;
		std::string busPath;
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
	::GDBusServer* server;
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
