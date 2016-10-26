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

	class ClassBV : public virtual ClassA, TFC::PropertyClass<ClassBV>
	{
		using TFC::PropertyClass<ClassBV>::Property;

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

		Property<int>::GetSet<&ClassBV::GetB, &ClassBV::SetB> B;

		ClassBV() : B(this) { }
	};

	class ClassC : public virtual ClassA, TFC::PropertyClass<ClassC>
	{
		using TFC::PropertyClass<ClassC>::Property;

	public:
		int valC;

		int GetC() const
		{
			return valC;
		}

		void SetC(int const& val)
		{
			this->valC = val;
		}

		Property<int>::GetSet<&ClassC::GetC, &ClassC::SetC> C;

		ClassC() : C(this) { }
	};



	class ClassD : public ClassBV, public ClassC, TFC::PropertyClass<ClassD>
	{
		using TFC::PropertyClass<ClassD>::Property;

	public:
		int valD;

		int GetD() const
		{
			return valD;
		}

		void SetD(int const& val)
		{
			this->valD = val;
		}

		Property<int>::GetSet<&ClassD::GetD, &ClassD::SetD> D;

		ClassD() : D(this) { }
	};

	class ClassE : public ClassB, TFC::PropertyClass<ClassE>
	{
		using TFC::PropertyClass<ClassE>::Property;
	public:
		int valE;

		int GetE() const
		{
			return valE;
		}

		void SetE(int const& val)
		{
			this->valE = val;
		}

		Property<int>::GetSet<&ClassE::GetE, &ClassE::SetE> E;

		ClassE() : E(this) { }
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

	EXPECT_EQ(testValA, resultA) << "Property get in base class failed";
	EXPECT_EQ(testValB, resultB) << "Property get in sub class failed";

	testValA = rand();
	testValB = rand();

	b.A = testValA;
	b.B = testValB;

	resultA = b.valA;
	resultB = b.valB;

	EXPECT_EQ(testValA, resultA) << "Property set in base class failed";
	EXPECT_EQ(testValB, resultB) << "Property set in sub class failed";

}

TEST_F(InheritedPropertyTest, PropertyInVirtualSubClass)
{
	ClassC c;
	int testValA = rand();
	int testValC = rand();

	c.valA = testValA;
	c.valC = testValC;

	int resultA = c.A;
	int resultC = c.C;

	EXPECT_EQ(testValA, resultA) << "Property in virtual base class failed";
	EXPECT_EQ(testValC, resultC) << "Property in sub class failed";

	testValA = rand();
	testValC = rand();

	c.A = testValA;
	c.C = testValC;

	resultA = c.valA;
	resultC = c.valC;

	EXPECT_EQ(testValA, resultA) << "Property set in base class failed";
	EXPECT_EQ(testValC, resultC) << "Property set in sub class failed";
}

TEST_F(InheritedPropertyTest, PropertyInDiamondSubClass)
{
	ClassD d;
	int testValA = rand();
	int testValB = rand();
	int testValC = rand();
	int testValD = rand();

	// Virtual inheritance to A on both class BV and C is required to avoid ambiguity
	d.valA = testValA;
	d.valB = testValB;
	d.valC = testValC;
	d.valD = testValD;

	int resultA = d.A;
	int resultB = d.B;
	int resultC = d.C;
	int resultD = d.D;

	EXPECT_EQ(testValA, resultA) << "Property in virtual top base class failed";
	EXPECT_EQ(testValB, resultB) << "Property in base class B failed";
	EXPECT_EQ(testValC, resultC) << "Property in base class C failed";
	EXPECT_EQ(testValD, resultD) << "Property in sub class failed";

	testValA = rand();
	testValB = rand();
	testValC = rand();
	testValD = rand();

	// Virtual inheritance to A on both class BV and C is required to avoid ambiguity
	d.A = testValA;
	d.B = testValB;
	d.C = testValC;
	d.D = testValD;

	resultA = d.valA;
	resultB = d.valB;
	resultC = d.valC;
	resultD = d.valD;

	EXPECT_EQ(testValA, resultA) << "Property in virtual top base class failed";
	EXPECT_EQ(testValB, resultB) << "Property in base class B failed";
	EXPECT_EQ(testValC, resultC) << "Property in base class C failed";
	EXPECT_EQ(testValD, resultD) << "Property in sub class failed";
}

TEST_F(InheritedPropertyTest, PropertyInSubSubClass)
{
	ClassE e;
	int testValA = rand();
	int testValB = rand();
	int testValE = rand();

	e.valA = testValA;
	e.valB = testValB;
	e.valE = testValE;

	int resultA = e.A;
	int resultB = e.B;
	int resultE = e.E;

	EXPECT_EQ(testValA, resultA) << "Property in base class failed";
	EXPECT_EQ(testValB, resultB) << "Property in sub class failed";
	EXPECT_EQ(testValE, resultE) << "Property in sub sub class failed";
}

