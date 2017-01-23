/*
 * ParameterPacker.cpp
 *
 *  Created on: Dec 21, 2016
 *      Author: gilang
 */

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


class RPCUnitTest : public testing::Test
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

using namespace TFC::Serialization;
using namespace TFC::ServiceModel;
using namespace TFC::Core;

namespace RPCTest {

class ITest
{
public:
	virtual std::string FunctionA(int a, int b, double c, std::string d) = 0;
	virtual void FunctionB(int s) = 0;
	virtual ~ITest() { }
};

class TestClass : public ITest
{
public:
	virtual std::string FunctionA(int a, int b, double c, std::string d) override
	{
		std::cout << "FunctionA is called!" << a << ", " << b << ", " << c << ", " << d << "\n";
		std::string s(d);
		s.append(std::to_string(a));
		s.append(std::to_string(b));
		s.append(std::to_string(c));
		return s;
	}

	virtual void FunctionB(int s) override
	{
		std::cout << "FunctionB is called with args: " << s << std::endl;
	}
};
}


using namespace RPCTest;

TEST_F(RPCUnitTest, ParameterSerializeWithGVariant)
{
	std::string p = "blah";
	auto tmp = ParameterSerializer<GVariantSerializer, decltype(&TestClass::FunctionA)>::Serialize(1, 2, 3.5, p);

	std::string expected(p);
	expected.append(std::to_string(1));
	expected.append(std::to_string(2));
	expected.append(std::to_string(3.5));

	TestClass t;
	auto params = ParameterDeserializer<GVariantDeserializer, decltype(&TestClass::FunctionA)>::Deserialize(tmp);
	typedef DelayedInvoker<decltype(&TestClass::FunctionA)> Invoker;
	auto res = Invoker::Invoke(&t, &TestClass::FunctionA, params);

	EXPECT_STREQ(expected.c_str(), res.c_str()) << "unpacker failed to return correct string";
}

#define RPCTEST_BUS_NAME "com.srin.tfc.RPCTest"
#define RPCTEST_OBJECT_PATH "/com/srin/tfc/RPCTest"
#define RPCTEST_IFACE_NAME "com.srin.tfc.RPCTest.ITest"
#define RPCTEST_METHODNAME "FunctionA"

static char const* interface_xml = R"XML(
<node>
    <interface name="com.srin.tfc.RPCTest.ITest">
        <method name="FunctionA">
            <arg type="i" name="a" direction="in" />
            <arg type="i" name="b" direction="in" />
            <arg type="d" name="c" direction="in" />
            <arg type="s" name="d" direction="in" />
            <arg type="s" name="resp" direction="out" />
        </method>
        <method name="FunctionB">
            <arg type="i" name="s" direction="in" />
        </method>
    </interface>
    <interface name="com.srin.tfc.RPCTest.IOther">
        <method name="FunctionA">
            <arg type="i" name="a" direction="in" />
            <arg type="i" name="b" direction="in" />
            <arg type="d" name="c" direction="in" />
            <arg type="s" name="d" direction="in" />
            <arg type="s" name="resp" direction="out" />
        </method>
        <method name="FunctionB">
            <arg type="i" name="s" direction="in" />
        </method>
    </interface>
</node>
)XML";

#include <glib-2.0/glib.h>
#include <glib-2.0/gio/gio.h>



GDBusNodeInfo* introspectData = nullptr;




static void
RPCTEST_HandleMethodCall (GDBusConnection       *connection,
                    const gchar           *sender,
                    const gchar           *object_path,
                    const gchar           *interface_name,
                    const gchar           *method_name,
                    GVariant              *parameters,
                    GDBusMethodInvocation *invocation,
                    gpointer               user_data)
{
	std::string methodName(method_name);

	std::cout << "Incoming: " << methodName << std::endl;

	std::cout << "Object Path: " << object_path << std::endl;

	std::cout << "Interface Name: " << interface_name << std::endl;

	if(methodName == RPCTEST_METHODNAME)
	{
		TestClass t;
		auto params = ParameterDeserializer<GVariantDeserializer, decltype(&TestClass::FunctionA)>::Deserialize(parameters, false);
		typedef DelayedInvoker<decltype(&TestClass::FunctionA)> Invoker;
		auto res = Invoker::Invoke(&t, &TestClass::FunctionA, params);

		auto str = g_strdup_printf(res.c_str());
		g_dbus_method_invocation_return_value (invocation, g_variant_new ("(s)", str));
		g_free(str);
	}
	else if(methodName == "FunctionB")
	{
		TestClass t;
		auto params = ParameterDeserializer<GVariantDeserializer, decltype(&TestClass::FunctionB)>::Deserialize(parameters, false);
		typedef DelayedInvoker<decltype(&TestClass::FunctionB)> Invoker;
		Invoker::Invoke(&t, &TestClass::FunctionB, params);
		g_dbus_method_invocation_return_value (invocation, nullptr);
	}
}

static const GDBusInterfaceVTable interface_vtable =
{
	RPCTEST_HandleMethodCall,
	nullptr,
	nullptr
};

static GDBusConnection* globalConnection = nullptr;

static void RPCTEST_OnBusAcquired (GDBusConnection *connection, const gchar* name, gpointer user_data)
{
	g_dbus_connection_register_object(connection,
					RPCTEST_OBJECT_PATH,
					introspectData->interfaces[0],
					&interface_vtable,
					nullptr,
					nullptr,
					nullptr);
}

static void RPCTEST_OnNameAcquired (GDBusConnection *connection, const gchar* name, gpointer user_data)
{
	std::cout << "Success acquiring name\n";
	globalConnection = connection;

	auto mtx = static_cast<std::mutex*>(user_data);
	mtx->unlock();
}

static void RPCTEST_OnNameLost (GDBusConnection *connection, const gchar* name, gpointer user_data)
{
	std::cout << "Name cannot be acquired\n";
	auto mtx = static_cast<std::mutex*>(user_data);
	mtx->unlock();
}

template<>
struct TFC::Core::TypeInfo<TestClass>
{
	static constexpr char const* typeName = "TestClass";
};

TEST_F(RPCUnitTest, DBusClientServer)
{
	std::mutex mtx;
	mtx.lock();
	introspectData = g_dbus_node_info_new_for_xml(interface_xml, nullptr);
	auto id = g_bus_own_name(G_BUS_TYPE_SESSION,
				   RPCTEST_BUS_NAME,
				   G_BUS_NAME_OWNER_FLAGS_NONE,
				   RPCTEST_OnBusAcquired,
				   RPCTEST_OnNameAcquired,
				   RPCTEST_OnNameLost,
				   &mtx,
				   nullptr);

	std::cout << "Registering name owning\n";
	mtx.lock();
	std::cout << "Complete registration\n";

	GError* err = nullptr;

	auto regId = 12;

	std::cout << "regId: " << regId << std::endl;

	if(regId == 0)
	{
		std::cout << err->message << std::endl;
	}

	auto conn = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, nullptr);
	if(conn != nullptr)
	{
		auto proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, nullptr, RPCTEST_BUS_NAME, RPCTEST_OBJECT_PATH, RPCTEST_IFACE_NAME, nullptr, nullptr);

		if(proxy != nullptr)
		{
			std::string p = "blah";
			auto params = ParameterSerializer<GVariantSerializer, decltype(&TestClass::FunctionA)>::Serialize(1, 2, 3.5, p);
			auto res = g_dbus_proxy_call_sync(proxy, RPCTEST_METHODNAME, params, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, nullptr);
			std::cout << "Success calling via dbus proxy\n";
		}
		else std::cout << "Failed to create proxy\n";
	}
	else std::cout << "Failed to get bus\n";

	mtx.unlock();

	g_bus_unown_name(id);
}

TEST_F(RPCUnitTest, ReflectionTest)
{
	TypeDescriptionTemplate<TestClass> testClassInfo {
		{ &TestClass::FunctionA, "FunctionA" }
	};


	for(auto& val : testClassInfo.functionMap)
	{
		std::cout << val.first << " => " << val.second->functionName << std::endl;
	}
}

struct TestServiceEndpoint
{
	typedef TFC::ServiceModel::GDBusChannel Channel;
	static GDBusConfiguration configuration;
	static constexpr char const* interfacePrefix = "com.srin.tfc";
};

GDBusConfiguration TestServiceEndpoint::configuration  {
	RPCTEST_BUS_NAME,
	G_BUS_TYPE_SYSTEM,
	G_BUS_NAME_OWNER_FLAGS_NONE,
	G_DBUS_PROXY_FLAGS_NONE
};

class TestClient : TFC::ServiceModel::ClientEndpoint<TestServiceEndpoint, ITest>
{
public:
	TestClient() :
		ClientEndpoint(RPCTEST_OBJECT_PATH)
	{
	}

	virtual std::string FunctionA(int a, int b, double c, std::string d)
	{
		return Invoke(&ITest::FunctionA, a, b, c, d);
	}

	virtual void FunctionB(int s) override
	{
		Invoke(&ITest::FunctionB, s);
	}
};



TFC_DefineTypeInfo(ITest) {
	{ &ITest::FunctionA, "FunctionA" },
	{ &ITest::FunctionB, "FunctionB" }
};

class ServerTest : public ServerObject<TestServiceEndpoint, ITest>
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
	}

	ServerTest() { }

};

TEST_F(RPCUnitTest, GetInterfaceStr)
{
	std::cout << TFC::Core::GetInterfaceName("com.srin.tfc", typeid(ITest)) << std::endl;
}

TEST_F(RPCUnitTest, TestWithClient)
{
	using Ms = std::chrono::milliseconds;

	std::mutex mtx;
	mtx.lock();
	introspectData = g_dbus_node_info_new_for_xml(interface_xml, nullptr);
	auto id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
				   RPCTEST_BUS_NAME,
				   G_BUS_NAME_OWNER_FLAGS_NONE,
				   RPCTEST_OnBusAcquired,
				   RPCTEST_OnNameAcquired,
				   RPCTEST_OnNameLost,
				   &mtx,
				   nullptr);

	std::cout << "Registering name owning\n";
	mtx.lock();
	std::cout << "Complete registration\n";



	TestClient client;

	std::cout << "Calling FunctionB" << std::endl;

	client.FunctionB(2560);

	std::cout << "Calling FunctionA" << std::endl;

	std::string p = "blah";
	auto result = client.FunctionA(1, 2, 3.5, p);

	std::cout << "Result of the function: " << result << std::endl;

	std::this_thread::sleep_for(Ms(1000 * 60 * 10));
	mtx.unlock();

	g_bus_unown_name(id);
}

static gchar **
subtree_enumerate (GDBusConnection       *connection,
                   const gchar           *sender,
                   const gchar           *object_path,
                   gpointer               user_data)
{
  gchar **nodes;
  GPtrArray *p;


	std::cout << "enumerate\n";

  p = g_ptr_array_new ();
  g_ptr_array_add (p, g_strdup ("obj1"));
  g_ptr_array_add (p, g_strdup ("obj2"));
  g_ptr_array_add (p, g_strdup ("obj3"));
  g_ptr_array_add (p, NULL);
  nodes = (gchar **) g_ptr_array_free (p, FALSE);

  return nodes;
}

static GDBusInterfaceInfo **
subtree_introspect (GDBusConnection       *connection,
                    const gchar           *sender,
                    const gchar           *object_path,
                    const gchar           *node,
                    gpointer               user_data)
{
	std::cout << "introspect: " << sender << " " << object_path << " " << (node == nullptr ? "" : node) << "\n";
  GPtrArray *p;
  p = g_ptr_array_new ();
  g_ptr_array_add (p, g_dbus_interface_info_ref (introspectData->interfaces[0]));

  g_ptr_array_add (p, g_dbus_interface_info_ref (introspectData->interfaces[1]));

  g_ptr_array_add (p, NULL);
  return (GDBusInterfaceInfo **) g_ptr_array_free (p, FALSE);
}

static const GDBusInterfaceVTable *
subtree_dispatch (GDBusConnection             *connection,
                  const gchar                 *sender,
                  const gchar                 *object_path,
                  const gchar                 *interface_name,
                  const gchar                 *node,
                  gpointer                    *out_user_data,
                  gpointer                     user_data)
{
  std::cout << "subtree_dispatch, sender: " << sender << ", object_path: " << object_path
		  	<< ", interface_name: " << interface_name << ", node: " << (node == nullptr ? "" : node) << std::endl;

  *out_user_data = nullptr;

  return &interface_vtable;
}


const GDBusSubtreeVTable subtree_vtable =
{
  subtree_enumerate,
  subtree_introspect,
  subtree_dispatch
};

static void RPCTEST_OnBusSubtreeAcquired (GDBusConnection *connection, const gchar* name, gpointer user_data)
{
	g_dbus_connection_register_subtree(connection,
					RPCTEST_OBJECT_PATH,
					&subtree_vtable,
					G_DBUS_SUBTREE_FLAGS_DISPATCH_TO_UNENUMERATED_NODES,
					nullptr,
					nullptr,
					nullptr);
}

TEST_F(RPCUnitTest, TestSubTree)
{
	using Ms = std::chrono::milliseconds;
	std::mutex mtx;
		mtx.lock();
		introspectData = g_dbus_node_info_new_for_xml(interface_xml, nullptr);
		auto id = g_bus_own_name(G_BUS_TYPE_SESSION,
					   RPCTEST_BUS_NAME,
					   G_BUS_NAME_OWNER_FLAGS_NONE,
					   RPCTEST_OnBusSubtreeAcquired,
					   RPCTEST_OnNameAcquired,
					   RPCTEST_OnNameLost,
					   &mtx,
					   nullptr);

		std::cout << "Registering name owning\n";
		mtx.lock();
		std::cout << "Complete registration\n";
		std::this_thread::sleep_for(Ms(1000));
		auto conn = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, nullptr);

		if(conn != nullptr)
		{
			auto proxy = g_dbus_proxy_new_sync(conn, G_DBUS_PROXY_FLAGS_NONE, nullptr, RPCTEST_BUS_NAME, RPCTEST_OBJECT_PATH "/objbanget/tong", RPCTEST_IFACE_NAME, nullptr, nullptr);
			std::this_thread::sleep_for(Ms(1000 * 60 * 10));
			if(proxy != nullptr)
			{
				std::string p = "blah";
				auto params = ParameterSerializer<GVariantSerializer, decltype(&TestClass::FunctionA)>::Serialize(1, 2, 3.5, p);
				auto res = g_dbus_proxy_call_sync(proxy, RPCTEST_METHODNAME, params, G_DBUS_CALL_FLAGS_NONE, -1, nullptr, nullptr);
				std::this_thread::sleep_for(Ms(1000));
				std::cout << "Success calling via dbus proxy\n";
			}
			else std::cout << "Failed to create proxy\n";
		}
		else std::cout << "Failed to get bus\n";

		mtx.unlock();
		std::this_thread::sleep_for(Ms(1000));
		g_bus_unown_name(id);
}
