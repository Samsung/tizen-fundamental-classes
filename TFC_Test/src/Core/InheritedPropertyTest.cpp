/*
 * InheritedPropertyTest.cpp
 *
 *	Test suite in verifying inherited property to work correctly, especially in cases of
 *	multiple inheritance
 *
 *  Created on: Oct 25, 2016
 *      Author: Gilang M. Hamidy (g.hamidy@gmail.com)
 */

#include "TFC/Core.new.h"

#include <gtest/gtest.h>

class InheritedPropertyTest : public testing::Test
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

	class ClassA : TFC::PropertyClass<ClassA>
	{
	public:
		int valA;



		int GetA() const
		{
			return valA;
		}

		void SetA(int const& val)
		{
			this->valA = val;
		}

		Property<int>::GetSet<&ClassA::GetA, &ClassA::SetA> A;

		ClassA() : A(this) { }
	};

	class ClassB : public ClassA, TFC::PropertyClass<ClassB>
	{
		using TFC::PropertyClass<ClassB>::Property;

	public:
		int valB;

		int GetB() const
		{
			return valB;
		}

		void SetB(int const& val)
		{
			this->valB = val;
		}

		Property<int>::GetSet<&ClassB::GetB, &ClassB::SetB> B;

		ClassB() : B(this) { }
	};

	class ClassC : public virtual ClassA, TFC::PropertyClass<ClassC>
	{
	public:
	};

	class ClassD : public ClassB, public ClassC, TFC::PropertyClass<ClassD>
	{
	public:
	};

	class ClassE : public ClassB, TFC::PropertyClass<ClassE>
	{
		using TFC::PropertyClass<ClassE>::Property;
	public:
		int valE;

		int GetE() const
		{
			return valB;
		}

		void SetE(int const& val)
		{
			this->valB = val;
		}

		Property<int>::GetSet<&ClassE::GetE, &ClassE::SetE> E;
	};
}

TEST_F(InheritedPropertyTest, InheritedPropertyDeclaration)
{
	// If it is compiled then the declaration is correct
	ASSERT_TRUE(true);
}

TEST_F(InheritedPropertyTest, PropertyInSubclass)
{
	ClassB b;
	int testValA = rand();
	int testValB = rand();

	b.valA = testValA;
	b.valB = testValB;

	int resultA = b.A;
	int resultB = b.B;

	EXPECT_EQ(testValA, resultA) << "Property in base class failed";
	EXPECT_EQ(testValB, resultB) << "Property in sub class failed";
}
