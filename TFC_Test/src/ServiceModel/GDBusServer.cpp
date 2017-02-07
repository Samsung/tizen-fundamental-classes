/*
 * GDBusServer.cpp
 *
 *  Created on: Dec 30, 2016
 *      Author: gilang
 */

#include "TFC/Serialization/ClassSerializer.h"

#include "TFC/Core/Invocation.h"
#include "TFC/ServiceModel/GDBusEndpoint.h"
#include "TFC/Core/Reflection.h"
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

#define RPCTEST_BUS_NAME "com.srin.tfc.RPCTest"
#define RPCTEST_OBJECT_PATH /*"/opt/usr/apps/org/example/tfc_test/data/mydbus/MyObject"*/ "/com/srin/tfc/RPCTest/MyObject"
#define RPCTEST_BUS_PATH "/opt/usr/apps/org.example.tfc_test/data/mydbus"
static int testStore;

namespace GDBusServerTestNS
{

struct ServiceEndpoint
{
	typedef TFC::ServiceModel::GDBusChannel Channel;
	static TFC::ServiceModel::GDBusConfiguration configuration;
	static constexpr char const* interfacePrefix = "com.srin.tfc";
};

TFC::ServiceModel::GDBusConfiguration ServiceEndpoint::configuration {
	RPCTEST_BUS_NAME,
	G_BUS_TYPE_SYSTEM,
	G_BUS_NAME_OWNER_FLAGS_NONE,
	G_DBUS_PROXY_FLAGS_NONE
};

class ITest : public TFC::EventEmitterClass<ITest>
{
public:
	virtual std::string FunctionA(int a, int b, double c, std::string d) = 0;
	virtual void FunctionB(int s) = 0;

	Event<int> eventSomething;

	virtual ~ITest() { }
};

class ITestAgain
{
public:
	virtual std::string FunctionC(int x) = 0;
	virtual ~ITestAgain() { }
};

class MyException : public std::exception
{
	std::string whoa;
public:
	MyException() : whoa() { }
	MyException(std::string const& msg) : whoa(msg) { }
	virtual char const* what() const noexcept override { return whoa.c_str(); }
};

class ServerTest : public TFC::ServiceModel::ServerObject<ServiceEndpoint, ITest>
{
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
		::testStore = s;

		eventSomething(this, s);

		//throw MyException("Anything aah");
	}
};

class ServerTestMultiIface : public TFC::ServiceModel::ServerObject<ServiceEndpoint, ITest>,
							 public TFC::ServiceModel::ServerObject<ServiceEndpoint, ITestAgain>
{
public:
	virtual std::string FunctionA(int a, int b, double c, std::string d) override
	{
		std::cout << "FunctionA in ServerTestMultiIface is called!" << a << ", " << b << ", " << c << ", " << d << "\n";
		std::string s(d);
		s.append(std::to_string(a));
		s.append(std::to_string(b));
		s.append(std::to_string(c));
		return s;
	}

	virtual void FunctionB(int s) override
	{
		std::cout << "FunctionB in ServerTestMultiIface is called with args: " << s << std::endl;
	}

	virtual std::string FunctionC(int s) override
	{
		std::cout << "FunctionC in ServerTestMultiIface is called with args: " << s << std::endl;

	}
};

class TestClient : TFC::ServiceModel::ClientEndpoint<ServiceEndpoint, ITest>
{
public:
	void EventHandlerTest(ITest* sender, int event);

	TestClient() :
		ClientEndpoint(RPCTEST_OBJECT_PATH)
	{
		RegisterEvent(&ITest::eventSomething);

		this->eventSomething += EventHandler(TestClient::EventHandlerTest);
	}

	virtual std::string FunctionA(int a, int b, double c, std::string d) override
	{
		return Invoke(&ITest::FunctionA, a, b, c, d);
	}

	virtual void FunctionB(int s) override
	{
		Invoke(&ITest::FunctionB, s);
	}
};


void TestClient::EventHandlerTest(ITest* sender, int event)
{
	std::cout << "Event raised: " << event << '\n';
}

}

TFC_DefineTypeInfo(GDBusServerTestNS::ITest) {
	{ &GDBusServerTestNS::ITest::FunctionA, "FunctionA" },
	{ &GDBusServerTestNS::ITest::FunctionB, "FunctionB" },
	{ &GDBusServerTestNS::ITest::eventSomething, "eventSomething" }
};

TFC_DefineTypeInfo(GDBusServerTestNS::ITestAgain) {
	{ &GDBusServerTestNS::ITestAgain::FunctionC, "FunctionC" }
};

TFC_DefineTypeInfo(GDBusServerTestNS::MyException) {
	{ TFC::Core::Constructor<GDBusServerTestNS::MyException>(), "Default" },
	{ TFC::Core::Constructor<GDBusServerTestNS::MyException, std::string>(), "String" }
};

TEST_F(GDBusServerTest, GDBusServerDefinition)
{
	using namespace GDBusServerTestNS;

	ServerTest serverObject;

	auto data = serverObject.GetInterfaceList();

	auto& interfaceInfo = data[0]->GetInterfaceInfo();

	ASSERT_STREQ("GDBusServerTestNS.ITest", interfaceInfo.name) << "Interface name is not equal";
	ASSERT_STREQ("FunctionA", interfaceInfo.methods[0]->name) << "FunctionA name is invalid";

	ASSERT_TRUE(true);
}

TEST_F(GDBusServerTest, GDBusServerDefinitionMultiIface)
{
	using namespace GDBusServerTestNS;

	ServerTestMultiIface serverObject;

	auto data = serverObject.GetInterfaceList();

	auto& interfaceInfo = data[0]->GetInterfaceInfo();

	ASSERT_STREQ("GDBusServerTestNS.ITest", interfaceInfo.name) << "Interface first name is not equal";
	ASSERT_STREQ("FunctionA", interfaceInfo.methods[0]->name) << "FunctionA name is invalid";

	auto& interfaceInfo1 = data[1]->GetInterfaceInfo();


	ASSERT_STREQ("GDBusServerTestNS.ITestAgain", interfaceInfo1.name) << "Interface second name is not equal";
	ASSERT_STREQ("FunctionC", interfaceInfo1.methods[0]->name) << "FunctionC name is invalid";

	ASSERT_TRUE(true);
}

TEST_F(GDBusServerTest, InvokeMethodViaReflection)
{
	using namespace GDBusServerTestNS;

	auto& desc = TFC::Core::TypeInfo<ITest>::typeDescription;

	ServerTestMultiIface test;

	ITest* ptr = &test;

	auto tuple = std::make_tuple(12345);

	auto& res = desc.GetFunctionByName("FunctionB");
	res.Invoke(ptr, &tuple);

	ASSERT_TRUE(true);
}


TEST_F(GDBusServerTest, GDBusServerCall)
{
	using namespace GDBusServerTestNS;

	TFC::ServiceModel::GDBusServer server({ "com.srin.tfc.RPCTest", RPCTEST_BUS_NAME,  G_BUS_TYPE_SYSTEM, G_BUS_NAME_OWNER_FLAGS_NONE });
	auto ptr = new ServerTest;
	ptr->SetName("MyObject");

	server.AddServerObject(ptr);

	server.Initialize();

	using Ms = std::chrono::milliseconds;
	std::this_thread::sleep_for(Ms(1000));

	std::cout << "Wake!\n";

	TestClient client;

	int random = rand();

	try
	{
		client.FunctionB(random);
		ASSERT_EQ(::testStore, random) << "Invocation via GDBus client server failed";
	}
	catch(MyException const& ex)
	{
		std::cout << ex.what() << '\n';
		ASSERT_TRUE(true);
	}
	catch(TFC::TFCException const& ex)
	{
		std::cout << typeid(ex).name() << " => " << ex.what() << '\n';
		ASSERT_TRUE(false) << "Exception was not propagated.";
	}
	catch(std::exception const& ex)
	{
		std::cout << typeid(ex).name() << " => " << ex.what() << '\n';
		ASSERT_TRUE(false) << "Exception was not propagated.";
	}

	std::this_thread::sleep_for(Ms(1000));

	auto actualStr = client.FunctionA(1, 2, 3.5, "some");
	std::this_thread::sleep_for(Ms(1000));
	ASSERT_STREQ("some123.500000", actualStr.c_str()) << "Invocation via GDBus client server returning string failed";
}

class SomeClass
{
public:
	int 	a;
	double	b;
	int 	c;
	std::string d;


};

TFC_DefineTypeSerializationInfo(SomeClass,
			TFC_ConstantValue(0x123456),
			TFC_FieldInfo(SomeClass::a),
			TFC_FieldInfo(SomeClass::b),
			TFC_FieldInfo(SomeClass::c),
			TFC_FieldInfo(SomeClass::d));

template<typename TSerializerClass>
using SomeClassSerializer = TFC::Serialization::ClassSerializer<TSerializerClass, SomeClass>;

template<typename TDeserializerClass>
using SomeClassDeserializer = TFC::Serialization::ClassDeserializer<TDeserializerClass, SomeClass>;

TEST_F(GDBusServerTest, ClassSerializer)
{
	using namespace GDBusServerTestNS;

	SomeClass p;
	p.a = 5;
	p.b = 12.4;
	p.c = 16;

	auto packed = SomeClassSerializer<TFC::ServiceModel::GVariantSerializer>::Serialize(p);

	int constructor;
	int actualA, actualC;
	double actualB;

	g_variant_get_child(packed, 0, "i", &constructor);
	g_variant_get_child(packed, 1, "i", &actualA);
	g_variant_get_child(packed, 2, "d", &actualB);
	g_variant_get_child(packed, 3, "i", &actualC);

	ASSERT_EQ(p.a, actualA) << "Variable a is incorrect";
	ASSERT_DOUBLE_EQ(p.b, actualB) << "Variable b is incorrect";
	ASSERT_EQ(p.c, actualC) << "Variable c is incorrect";

	auto deserialized = SomeClassDeserializer<TFC::ServiceModel::GVariantDeserializer>::Deserialize(packed);


	ASSERT_EQ(p.a, deserialized.a) << "Deserialized variable a is incorrect";
	ASSERT_DOUBLE_EQ(p.b, deserialized.b) << "Deserialized variable b is incorrect";
	ASSERT_EQ(p.c, deserialized.c) << "Deserialized variable c is incorrect";
}

template<>
void TFC::ServiceModel::ServerObject<GDBusServerTestNS::ServiceEndpoint, GDBusServerTestNS::ITest>::InitializeInterface()
{
	RegisterFunction(&ITest::FunctionA, "FunctionA");
	RegisterFunction(&ITest::FunctionB, "FunctionB");
	RegisterEvent(&ITest::eventSomething, "eventSomething");
}

template<>
void TFC::ServiceModel::ServerObject<GDBusServerTestNS::ServiceEndpoint, GDBusServerTestNS::ITestAgain>::InitializeInterface()
{
	RegisterFunction(&ITestAgain::FunctionC, "FunctionC");
}

class ReflectableClass
{
public:
	int value;
	std::string str;

	ReflectableClass(int value) : value(value), str("default") { std::cout << "Calling int constructor\n"; }
	ReflectableClass(std::string param) : value(123), str(param) { std::cout << "Calling string constructor param: " << param << "\n"; }
	~ReflectableClass() { std::cout << "Calling destructor\n"; }
};

TFC_DefineTypeInfo(ReflectableClass) {
	{ Constructor<ReflectableClass, int>(), "i" },
	{ Constructor<ReflectableClass, std::string>(), "s" },
	{ Destructor<ReflectableClass>() }
};

TEST_F(GDBusServerTest, ReflectionConstructor)
{
	using namespace TFC::Core;

	int random = rand();

	decltype(auto) info = FindTypeByName("ReflectableClass");

	void* something = info.GetConstructor<int>().Construct(random);
	ASSERT_EQ(random, reinterpret_cast<ReflectableClass*>(something)->value) << "Value in Reflectable Class is wrong";

	void* anotherThing = info.Construct(std::string("Masa sich"));

	info.Delete(something);
	info.Delete(anotherThing);

	bool fail = true;

	try
	{
		info.Throw(std::string("Something is throwed"));
	}
	catch(ReflectableClass const& t)
	{
		std::cout << t.str << '\n';
		fail = false;
	}

	ASSERT_FALSE(fail) << "Using Throw failed";
}

#include "TFC/ServiceModel/BinarySerializer.h"

TEST_F(GDBusServerTest, BinarySerializer)
{
	using namespace GDBusServerTestNS;

	SomeClass p;
	p.a = 5;
	p.b = 12.4;
	p.c = 16;
	p.d = "Gilang Ganteng";

	auto packed = SomeClassSerializer<TFC::ServiceModel::BinarySerializer>::Serialize(p);

	for(auto byte : packed)
	{
		std::cout << (int)byte << ' ';
	}
	std::cout << '\n';

	auto d = SomeClassDeserializer<TFC::ServiceModel::BinaryDeserializer>::Deserialize(packed);

	ASSERT_EQ(p.a, d.a) << "Variable a is incorrect";
	ASSERT_DOUBLE_EQ(p.b, d.b) << "Variable b is incorrect";
	ASSERT_EQ(p.c, d.c) << "Variable c is incorrect";
	ASSERT_STREQ(p.d.c_str(), d.d.c_str()) << "Variable d is incorrect";
}
