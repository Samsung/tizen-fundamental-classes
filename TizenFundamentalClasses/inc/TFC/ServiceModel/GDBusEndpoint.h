/*
 * GDBusEndpoint.h
 *
 *  Created on: Dec 21, 2016
 *      Author: gilang
 */

#ifndef TFC_SERVICEMODEL_GDBUSENDPOINT_H_
#define TFC_SERVICEMODEL_GDBUSENDPOINT_H_

#include <glib-2.0/glib.h>

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
	~GVariantDeserializer();
	template<typename T>
	T Unpack(int index);
};


class GDBusClient
{
private:
	void* handle;
public:
	GDBusClient(char const* busName, char const* objectPath, char const* interfaceName);
	GVariant* RemoteCall(char const* methodName, GVariant* parameter);
};

struct GDBusChannel
{
	typedef GVariantSerializer 	 Serializer;
	typedef GVariantDeserializer Deserializer;
	typedef GDBusClient			 Client;
};

}
}


#endif /* TFC_SERVICEMODEL_GDBUSENDPOINT_H_ */
