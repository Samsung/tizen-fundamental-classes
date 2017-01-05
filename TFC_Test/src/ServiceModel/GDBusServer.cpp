/*
 * GDBusServer.cpp
 *
 *  Created on: Dec 30, 2016
 *      Author: gilang
 */

#include "TFC/ServiceModel/InterfaceInspector.h"
#include "TFC/ServiceModel/GDBusEndpoint.h"
#include "TFC/ServiceModel/Reflection.h"
#include "TFC/ServiceModel/ClientEndpoint.h"
#include "TFC/ServiceModel/ServerEndpoint.h"
#include "TFC_Test.h"

#include <gtest/gtest.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>



class GDBusServerTest : public testing::Test
{
	protected:
	// virtual void SetUp() will be called before each test is run.
	// You should define it if you need to initialize the variables.
	// Otherwise, you don't have to provide it.
	virtual void SetUp()
	{

	}
	// virtual void TearDown() will be called after each test is run.
	// You should define it if there is cleanup work to do.
	// Otherwise, you don't have to provide it.
	virtual void TearDown()
	{

	}
};

namespace GDBusServerTestNS
{

struct ServiceEndpoint
{
	typedef TFC::ServiceModel::GDBusChannel Channel;
	static TFC::ServiceModel::GDBusConfiguration configuration;
	static constexpr char const* interfacePrefix = "com.srin.tfc";
};

class ITest
{
public:
	virtual std::string FunctionA(int a, int b, double c, std::string d) = 0;
	virtual void FunctionB(int s) = 0;
	virtual ~ITest() { }
};

class ServerTest : public TFC::ServiceModel::ServerObject<ServiceEndpoint, ITest>
{
private:
	static void Init()
	{
		RegisterFunction(&ITest::FunctionA);
		RegisterFunction(&ITest::FunctionB);
	}
public:
	virtual std::string FunctionA(int a, int b, double c, std::string d) override
	{
		std::cout << "FunctionA in ServerTest is called!" << a << ", " << b << ", " << c << ", " << d << "\n";
		std::string s(d);
		s.append(std::to_string(a));
		s.append(std::to_string(b));
		s.append(std::to_string(c));
		return s;
	}

	virtual void FunctionB(int s) override
	{
		std::cout << "FunctionB in ServerTest is called with args: " << s << std::endl;
	}

	ServerTest() : ServerObject(&ServerTest::Init) { }
};

}

TFC_DefineTypeInfo(GDBusServerTestNS::ITest) {
	{ &GDBusServerTestNS::ITest::FunctionA, "FunctionA" },
	{ &GDBusServerTestNS::ITest::FunctionB, "FunctionB" }
};

TEST_F(GDBusServerTest, GDBusServerDefinition)
{
	using namespace GDBusServerTestNS;

	ServerTest serverObject;

	auto& data = serverObject.GetInterfaceDefinition();

	auto& interfaceInfo = data.GetInterfaceInfo();

	ASSERT_STREQ("GDBusServerTestNS.ITest", interfaceInfo.name) << "Interface name is not equal";
	ASSERT_STREQ("FunctionA", interfaceInfo.methods[0]->name) << "FunctionA name is invalid";

	ASSERT_TRUE(true);
}

class SomeClass
{
public:
	int 	a;
	double	b;
	int 	c;
};

TFC_DefineTypeSerializationInfo(SomeClass,
			TFC_FieldInfo(SomeClass::a),
			TFC_FieldInfo(SomeClass::b),
			TFC_FieldInfo(SomeClass::c));

template<typename TSerializerClass>
using SomeClassSerializer = TFC::ServiceModel::ClassSerializer<TSerializerClass, SomeClass>;

template<typename TDeserializerClass>
using SomeClassDeserializer = TFC::ServiceModel::ClassDeserializer<TDeserializerClass, SomeClass>;

TEST_F(GDBusServerTest, ClassSerializer)
{
	using namespace GDBusServerTestNS;

	SomeClass p;
	p.a = 5;
	p.b = 12.4;
	p.c = 16;

	auto packed = SomeClassSerializer<TFC::ServiceModel::GVariantSerializer>::Serialize(p);

	int actualA, actualC;
	double actualB;

	g_variant_get_child(packed, 0, "i", &actualA);
	g_variant_get_child(packed, 1, "d", &actualB);
	g_variant_get_child(packed, 2, "i", &actualC);

	ASSERT_EQ(p.a, actualA) << "Variable a is incorrect";
	ASSERT_DOUBLE_EQ(p.b, actualB) << "Variable b is incorrect";
	ASSERT_EQ(p.c, actualC) << "Variable c is incorrect";

	auto deserialized = SomeClassDeserializer<TFC::ServiceModel::GVariantDeserializer>::Deserialize(packed);


	ASSERT_EQ(p.a, deserialized.a) << "Deserialized variable a is incorrect";
	ASSERT_DOUBLE_EQ(p.b, deserialized.b) << "Deserialized variable b is incorrect";
	ASSERT_EQ(p.c, deserialized.c) << "Deserialized variable c is incorrect";
}
