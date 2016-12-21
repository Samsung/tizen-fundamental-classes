/*
 * GDBusEndpoint.cpp
 *
 *  Created on: Dec 21, 2016
 *      Author: gilang
 */


#include "TFC/Core.h"
#include "TFC/ServiceModel/GDBusEndpoint.h"

#include <string>

using namespace TFC::ServiceModel;


LIBAPI
GVariantPackerPolicy::GVariantPackerPolicy()
{
	g_variant_builder_init(&builder, G_VARIANT_TYPE_TUPLE);

}

LIBAPI
GVariantPackerPolicy::PackType GVariantPackerPolicy::EndPack() {
	return g_variant_builder_end(&builder);
}

template<>
LIBAPI
void GVariantPackerPolicy::Pack<int>(int args)
{
	g_variant_builder_add(&builder, "i", args);
}

template<>
LIBAPI
void GVariantPackerPolicy::Pack<double>(double args)
{
	g_variant_builder_add(&builder, "d", args);
}

template<>
LIBAPI
void GVariantPackerPolicy::Pack<std::string>(std::string args)
{
	g_variant_builder_add(&builder, "s", args.c_str());
}

LIBAPI
TFC::ServiceModel::GVariantUnpackerPolicy::GVariantUnpackerPolicy(PackType p) : variant(p) {
}

template<>
LIBAPI
int GVariantUnpackerPolicy::Unpack<int>(int index)
{
	int res;
	g_variant_get_child(variant, index, "i", &res);
	return res;
}

template<>
LIBAPI
double GVariantUnpackerPolicy::Unpack<double>(int index)
{
	double res;
	g_variant_get_child(variant, index, "d", &res);
	return res;
}

template<>
LIBAPI
std::string GVariantUnpackerPolicy::Unpack<std::string>(int index)
{
	auto strVariant = g_variant_get_child_value(variant, index);
	gsize len = 0;
	auto theStr = g_variant_get_string(strVariant, &len);
	std::string res(theStr, len);
	g_variant_unref(strVariant);
	return res;
}

LIBAPI
TFC::ServiceModel::GVariantUnpackerPolicy::~GVariantUnpackerPolicy() {
	g_variant_unref(variant);
}
