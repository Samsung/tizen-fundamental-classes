/*
 * PropertyTest.cpp
 *
 *	Test suite consisting of basic declaration and usage of property in class
 *
 *  Created on: Oct 25, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@gmail.com)
 */

#include "TFC/Core.h"

#include <gtest/gtest.h>



class PropertyTest : public testing::Test
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

namespace {
	struct SomeData
	{
		int valA;
		int valB;


		void MemberFunction(int val)
		{
			valA = val;
		}

		int ConstMemberFunction() const
		{
			return valA;
		}
	};

	bool operator==(SomeData const& a, SomeData const& b)
	{
		return a.valA == b.valA && a.valB == b.valB;
	}

	bool operator!=(SomeData const& a, SomeData const& b)
	{
		return !(a == b);
	}

	class PropertyComponent : TFC::PropertyClass<PropertyComponent>
	{
	public:
		int GetA() const;
		void SetA(int const& a);

		SomeData GetData() const;
		void SetData(SomeData const& d);

		SomeData const& GetDataRef() const;
		void SetDataRef(SomeData const& ref);

		SomeData* GetDataPtr();
		void SetDataPtr(SomeData* ptr);

		SomeData const* GetDataPtrConst() const;

		int* GetAPtr();
		void SetAPtr(int* ptr);

		Property<int>::Get<
			&PropertyComponent::GetA>::Set<&PropertyComponent::SetA> A;

		Property<int*>::Get<
			&PropertyComponent::GetAPtr>::Set<&PropertyComponent::SetAPtr> APtr;

		Property<SomeData>::Get<
			&PropertyComponent::GetData>::Set<&PropertyComponent::SetData> Data;

		Property<SomeData const&>::Get<
			&PropertyComponent::GetDataRef>::Set<&PropertyComponent::SetDataRef> DataRef;

		Property<SomeData*>::Get<&PropertyComponent::GetDataPtr>
						   ::Set<&PropertyComponent::SetDataPtr> DataPtr;

		Property<SomeData const*>::Get<
			&PropertyComponent::GetDataPtrConst>::Set<&PropertyComponent::SetDataPtr> DataPtrConst;

		std::string const& GetValC() const;
		void SetValC(std::string const& valC);

		Property<std::string const&>::Get<&PropertyComponent::GetValC>::Set<&PropertyComponent::SetValC> ValC;

		PropertyComponent();

		int a;
		int* aPtr;
		SomeData data;
		SomeData* dataPtr;
		std::string valC;
	};

	const std::string& PropertyComponent::GetValC() const {
		return this->valC;
	}

	void PropertyComponent::SetValC(const std::string& valC) {
		this->valC = valC;
	}

	PropertyComponent::PropertyComponent() :
		A(this), Data(this), DataRef(this), DataPtrConst(this), DataPtr(this), a(0), data({0, 0}), aPtr(nullptr), APtr(this), ValC(this)
	{

	}

	int PropertyComponent::GetA() const
	{
		return this->a;
	}

	void PropertyComponent::SetA(int const& a)
	{
		this->a = a;
	}

	int* PropertyComponent::GetAPtr()
	{
		return this->aPtr;
	}

	void PropertyComponent::SetAPtr(int* a)
	{
		this->aPtr = a;
	}


	SomeData PropertyComponent::GetData() const
	{
		return this->data;
	}

	void PropertyComponent::SetData(SomeData const& d)
	{
		this->data = d;
	}

	SomeData const& PropertyComponent::GetDataRef() const
	{
		return this->data;
	}

	void PropertyComponent::SetDataRef(SomeData const& ref)
	{
		this->data = ref;
	}

	SomeData* PropertyComponent::GetDataPtr()
	{
		return this->dataPtr;
	}

	void PropertyComponent::SetDataPtr(SomeData* ptr)
	{
		this->dataPtr = ptr;
	}

	SomeData const* PropertyComponent::GetDataPtrConst() const
	{
		return this->dataPtr;
	}
}

TEST_F(PropertyTest, PropertyDeclaration)
{
	// If it is compiled then the declaration is correct
	ASSERT_TRUE(true);
}

/**
 * Test getting property data by value for primitive data type
 */
TEST_F(PropertyTest, PropertyGetByValue_Int)
{
	int testData = rand();

	PropertyComponent comp;
	comp.a = testData;

	int result = comp.A;
	EXPECT_EQ(testData, result) << "Retrieved invalid value";
}

/**
 * Test getting property data by value for composite data type
 */
TEST_F(PropertyTest, PropertyGetByValue_Struct)
{
	SomeData testData = { rand(), rand() };
	PropertyComponent comp;

	comp.data = testData;

	// Use decltype to fetch typename
	decltype(comp.Data.T()) result = comp.Data;
	EXPECT_EQ(testData, result) << "Retrieved invalid value";

	/*
	// Using by-reference fetching data (MUST NOT COMPILE)
	SomeData& resultRef = comp.Data;
	*/

	/*
	// Using auto keyword in declaring data type (MUST NOT COMPILE)
	auto resultAuto = comp.Data;
	*/

	// Using explicit typename
	SomeData resultExplicit = comp.Data;
	EXPECT_EQ(testData, resultExplicit) << "Retrieved invalid value";

	// Try modifying source value
	testData.valA = rand();
	result = comp.Data;
	EXPECT_NE(testData, result) << "Value was modified by reference";
}

/**
 * Test setting property data by value for primitive data type
 */
TEST_F(PropertyTest, PropertySetByValue_Int)
{
	int testData = rand();

	PropertyComponent comp;
	comp.A = testData;

	int result = comp.a;
	EXPECT_EQ(testData, result) << "Retrieved invalid value";
}

/**
 * Test getting property data for composite data declared to be retrieved by value
 */
TEST_F(PropertyTest, PropertySetByValue_Struct)
{
	SomeData testData = { rand(), rand() };
	PropertyComponent comp;

	// Set data via property
	comp.Data = testData;

	SomeData result = comp.data;
	EXPECT_EQ(testData, result) << "Set invalid value";

	// Try modifying source value
	testData.valA = rand();
	result = comp.data;
	EXPECT_NE(testData, result) << "Value was modified by reference";
}

/**
 * Test getting property data by reference for composite data type
 */
TEST_F(PropertyTest, PropertyGetByRef_Struct)
{
	SomeData testData = { rand(), rand() };
	PropertyComponent comp;

	comp.data = testData;

	SomeData const& result = comp.DataRef;
	EXPECT_EQ(testData, result) << "Retrieved invalid value";

	EXPECT_EQ(&result, &comp.data) << "Reference does not have same pointer";

	// Try modifying internal value
	testData = { rand(), rand() };
	comp.data = testData;
	EXPECT_EQ(testData, result) << "Reference was not modified";

	// Using non-reference typename
	SomeData resultExplicit = comp.DataRef;
	EXPECT_EQ(testData, resultExplicit) << "Retrieved invalid value";

	// Using decltype
	decltype(comp.DataRef.T()) resultDecl = comp.DataRef;
	EXPECT_EQ(testData, resultDecl) << "Retrieved invalid value on decltype var";

	// Try modifying internal value
	testData = { rand(), rand() };
	comp.data = testData;
	EXPECT_EQ(testData, resultDecl) << "Reference was not modified on decltype var";
}

/**
 * Test setting property data for composite data declared to be retrieved by reference
 */
TEST_F(PropertyTest, PropertySetByRef_Struct)
{
	SomeData testData = { rand(), rand() };
	PropertyComponent comp;

	// Set data via property
	comp.DataRef = testData;

	SomeData result = comp.data;
	EXPECT_EQ(testData, result) << "Set invalid value";

	// Try modifying source value
	testData.valA = rand();
	result = comp.data;
	EXPECT_NE(testData, result) << "Value was modified by reference";
}

/**
 * Test getting property data by reference for composite data type
 */
TEST_F(PropertyTest, PropertyGetByPtr_Struct)
{
	SomeData testData = { rand(), rand() };
	PropertyComponent comp;

	comp.dataPtr = &testData;

	SomeData* result = comp.DataPtr;
	EXPECT_EQ(&testData, result) << "Pointer of testData is different";
}

/**
 * Test getting property data by reference for composite data type
 */
TEST_F(PropertyTest, PropertySetByPtr_Struct)
{
	SomeData testData = { rand(), rand() };
	PropertyComponent comp;

	comp.DataPtr = &testData;

	SomeData* result = comp.dataPtr;
	EXPECT_EQ(&testData, result) << "Pointer of testData is different";
}

/**
 * Validate const-correctess of Property object. It will behaves as intended and compile
 * successfully. Failure to compile means there is syntactical or semantical error in the
 * Property core component
 */
TEST_F(PropertyTest, PropertyConstCorrect)
{
	int testInt = rand();
	SomeData testData = { rand(), rand() };

	PropertyComponent comp;
	comp.data = testData;
	comp.dataPtr = &testData;
	comp.a = testInt;

	PropertyComponent const& constComp(comp);

	int resultInt = constComp.A;
	EXPECT_EQ(testInt, resultInt) << "Integer is different";

	SomeData resultDataByVal = constComp.Data;
	EXPECT_EQ(testData, resultDataByVal) << "SomeData is different";

	/*
	// Using non-const reference fetching data (MUST NOT COMPILE)
	SomeData& resultRefFail = comp.DataRef;
	*/

	SomeData const& resultDataByRef = constComp.DataRef;
	EXPECT_EQ(testData, resultDataByRef) << "SomeData reference is different";

	/*
	// Using non-const pointer fetching data (MUST NOT COMPILE)
	SomeData* resultPtrFail = comp.DataPtr;
	*/

	SomeData const* resultDataByPtr = constComp.DataPtr;
	EXPECT_EQ(&testData, resultDataByPtr) << "SomeData pointer is different";

	/*
	// All set function on const reference MUST NOT COMPILE

	constComp.A = 10;
	constComp.DataRef = testData;
	*/
}

/**
 * Validate the usage of pointer-member-access operator (->) correctness to call its member as
 * well as its const correctness of the member-access operator operating on const property object
 */
TEST_F(PropertyTest, PropertyMemberAccess)
{
	int testInt = rand();
	SomeData testData = { rand(), rand() };

	PropertyComponent comp;
	comp.dataPtr = &testData;
	comp.aPtr = &testInt;

	/*
	// Calling operator -> to non-class pointer (MUST NOT COMPILE)
	comp.APtr->valA();
	*/


	int value = testData.valA;
	int result = comp.DataPtr->ConstMemberFunction();

	EXPECT_EQ(value, result) << "SomeData valA is different";

	value = rand();
	comp.DataPtr->MemberFunction(value);
	result = testData.valA;

	EXPECT_EQ(value, result) << "SomeData valA is different after set";

	PropertyComponent const& constComp = comp;

	/*
	// Calling non-const member of pointer-referred object (MUST NOT COMPILE)
	constComp.DataPtr->MemberFunction(123);
	*/

	constComp.DataPtr->ConstMemberFunction(); // Allowed


}

TEST_F(PropertyTest, StringPropertyType)
{
	PropertyComponent comp;

	comp.ValC = "Test";

	std::string tst = comp.ValC;

	EXPECT_STREQ("Test", tst.c_str()) << "String via property is not equal";

	bool comparison = comp.ValC == "Test";

	ASSERT_TRUE(comparison) << "Comparison failed";
}

TEST_F(PropertyTest, IntComparisonProperty)
{
	PropertyComponent comp;
	comp.A = 12345;

	ASSERT_TRUE(comp.A == 12345) << "Comparison using property and literal is failed";
}
