/*
 * PropertyTest.cpp
 *
 *	Test suite consisting of basic declaration and usage of property in class
 *
 *  Created on: Oct 25, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@gmail.com)
 */

#include "TFC/Core.new.h"

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

		Property<int>::GetSet<
			&PropertyComponent::GetA,
			&PropertyComponent::SetA> A;

		Property<SomeData>::GetSet<
			&PropertyComponent::GetData,
			&PropertyComponent::SetData> Data;

		Property<SomeData&>::GetSet<
			&PropertyComponent::GetDataRef,
			&PropertyComponent::SetDataRef> DataRef;

		Property<SomeData*>::GetSet<
			&PropertyComponent::GetDataPtr,
			&PropertyComponent::SetDataPtr> DataPtr;

		Property<SomeData const*>::GetSet<
			&PropertyComponent::GetDataPtrConst,
			&PropertyComponent::SetDataPtr> DataPtrConst;

		PropertyComponent();

		int a;
		SomeData data;
		SomeData* dataPtr;
	};

	PropertyComponent::PropertyComponent() :
		A(this), Data(this), DataRef(this), DataPtrConst(this), DataPtr(this), a(0), data({0, 0})
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

