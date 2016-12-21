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

struct GVariantPackerPolicy
{
	GVariantBuilder builder;

	typedef GVariant* PackType;
	GVariantPackerPolicy();

	template<typename T>
	void Pack(T args);



	PackType EndPack();
};

struct GVariantUnpackerPolicy
{
	typedef GVariant* PackType;

	PackType variant;

	GVariantUnpackerPolicy(PackType p);
	~GVariantUnpackerPolicy();
	template<typename T>
	T Unpack(int index);


};

}
}


#endif /* TFC_SERVICEMODEL_GDBUSENDPOINT_H_ */
