#include "pch.h"
#pragma warning( disable:4251 )

#include "TestCommon.h"
#include "Core/IOState.h"

namespace UnitTests
{	
	using namespace Core;

	TEST(TestIOState, Create)
	{
		IOState state1;
		ASSERT_EQ(1, state1.GetWidth());
		
		IOState state2(IOState::HI);
		ASSERT_EQ(1, state2.GetWidth());
		
		IOState state3(IOState::LOW, 8);
		ASSERT_EQ(8, state3.GetWidth());

		ASSERT_THROW(IOState(IOState::LOW, 0), std::out_of_range);
		ASSERT_THROW(IOState(IOState::LOW, 17), std::out_of_range);
		ASSERT_THROW(IOState(IOState::LOW, -1), std::exception);

		IOState state4(IOState::LOW, 16);
	}

	TEST(TestIOState, InitArray)
	{
		IOState state({ IOState::LOW });
		EXPECT_EQ(1, state.GetWidth());
		EXPECT_EQ(IOState::LOW, state[0]);

		IOState state2({ IOState::LOW, IOState::LOW });
		EXPECT_EQ(2, state2.GetWidth());
		EXPECT_EQ(IOState::LOW, state2[0]);
		EXPECT_EQ(IOState::LOW, state2[1]);

		IOState state3({ IOState::LOW, IOState::LOW, IOState::LOW });
		EXPECT_EQ(3, state3.GetWidth());
		EXPECT_EQ(IOState::LOW, state3[2]);

		IOState state4 = { IOState::LOW };
		EXPECT_EQ(1, state4.GetWidth());
		EXPECT_EQ(IOState::LOW, state4[0]);

		IOState state5 = { IOState::LOW, IOState::LOW };
		EXPECT_EQ(2, state5.GetWidth());
		EXPECT_EQ(IOState::LOW, state5[0]);
		EXPECT_EQ(IOState::LOW, state5[1]);

		IOState state6 = { IOState::LOW, IOState::LOW, IOState::LOW };
		EXPECT_EQ(3, state6.GetWidth());
		EXPECT_EQ(IOState::LOW, state6[2]);
	}

	TEST(TestIOState, Equal)
	{
		IOState state1(IOState::HI);
		IOState state2(IOState::HI);
		IOState state3(IOState::LOW);

		ASSERT_EQ(state1, state2);
		ASSERT_EQ(state2, state1);
		ASSERT_NE(state1, state3);
		ASSERT_NE(state3, state1);

		ASSERT_EQ(state1, IOState::HI);
		ASSERT_EQ(IOState::HI, state1);
		ASSERT_NE(state1, IOState::LOW);
		ASSERT_NE(IOState::LOW, state1);
		ASSERT_EQ(state3, IOState::LOW);
		ASSERT_EQ(IOState::LOW, state3);

		IOState state4(IOState::HI, 8);
		IOState state5(IOState::HI, 4);
		IOState state6(IOState::HI, 8);
		IOState state7(IOState::LOW, 8);

		ASSERT_FALSE(state4 == state5);
		ASSERT_EQ(state4, state6);
		ASSERT_EQ(state6, state4);

		ASSERT_FALSE(state4 != state6);
		ASSERT_NE(state4, state5);
		ASSERT_NE(state5, state4);

		ASSERT_NE(state5, state6);
		ASSERT_NE(state4, state7);
		ASSERT_NE(state4, IOState::HI);
		ASSERT_NE(state4, IOState::LOW);
	}

	TEST(TestIOState, Set)
	{
		IOState state1(IOState::HI);
		EXPECT_EQ(state1, IOState::HI);

		state1.Set(IOState::LOW);
		EXPECT_EQ(state1, IOState::LOW);

		IOState state2(IOState::HI, 4);
		state2.Set({ IOState::LOW, IOState::HI, IOState::HI_Z, IOState::UNDEF });
		EXPECT_EQ(state2.Get(0), IOState::LOW);
		EXPECT_EQ(state2.Get(1), IOState::HI);
		EXPECT_EQ(state2.Get(2), IOState::HI_Z);
		EXPECT_EQ(state2.Get(3), IOState::UNDEF);
		EXPECT_THROW(state2.Get(4), std::out_of_range);
		EXPECT_THROW(state2.Get(-1), std::out_of_range);
		IOState state4({ IOState::LOW, IOState::HI, IOState::HI_Z, IOState::UNDEF });
		EXPECT_EQ(state4, state2);

		IOState state3(IOState::LOW, 4);
		state3.Set(0, IOState::HI);
		state3.Set(1, IOState::HI);
		state3.Set(2, IOState::HI);
		state3.Set(3, IOState::HI);
		EXPECT_THROW(state3.Set(4, IOState::HI), std::out_of_range);
		EXPECT_THROW(state3.Set(-1, IOState::HI), std::out_of_range);
		EXPECT_EQ(IOState(IOState::HI, 4), state3);
		EXPECT_THROW(state3.Set(IOState::HI), std::invalid_argument);	

		state3.Set({ IOState::LOW, IOState::HI_Z, IOState::HI, IOState::UNDEF });
		EXPECT_EQ(IOState::LOW, state3[0]);
		EXPECT_EQ(IOState::HI_Z, state3[1]);
		EXPECT_EQ(IOState::HI, state3[2]);
		EXPECT_EQ(IOState::UNDEF, state3[3]);

		EXPECT_THROW(state3.Set({ IOState::LOW, IOState::HI }), std::invalid_argument);
		EXPECT_THROW(state3.Set({ IOState::LOW }), std::invalid_argument);

	}

	TEST(TestIOState, Get)
	{
		IOState state = { IOState::LOW, IOState::HI, IOState::HI_Z, IOState::UNDEF };

		EXPECT_EQ(IOState::LOW, state.Get());
		EXPECT_EQ(IOState::LOW, state.Get(0));
		EXPECT_EQ(IOState::HI, state.Get(1));
		EXPECT_EQ(IOState::HI_Z, state.Get(2));
		EXPECT_EQ(IOState::UNDEF, state.Get(3));

		EXPECT_THROW(state.Get(-1), std::out_of_range);
		EXPECT_THROW(state.Get(4), std::out_of_range);
		EXPECT_THROW(state.Get(-1, -1), std::out_of_range);
		EXPECT_THROW(state.Get(-1, 4), std::out_of_range);
		EXPECT_THROW(state.Get(1, 4), std::out_of_range);
		EXPECT_THROW(state.Get(5, 3), std::out_of_range);
		EXPECT_THROW(state.Get(2, 1), std::out_of_range);

		EXPECT_EQ(state.Get(0, 3), IOState({ IOState::LOW, IOState::HI, IOState::HI_Z, IOState::UNDEF }));
		EXPECT_EQ(state.Get(0, 2), IOState({ IOState::LOW, IOState::HI, IOState::HI_Z }));
		EXPECT_EQ(state.Get(0, 1), IOState({ IOState::LOW, IOState::HI }));
		EXPECT_EQ(state.Get(0, 0), IOState({ IOState::LOW }));
		EXPECT_EQ(state.Get(0, 0), IOState::LOW);

		EXPECT_EQ(state.Get(1, 3), IOState({ IOState::HI, IOState::HI_Z, IOState::UNDEF }));
		EXPECT_EQ(state.Get(2, 3), IOState({ IOState::HI_Z, IOState::UNDEF }));
		EXPECT_EQ(state.Get(3, 3), IOState({ IOState::UNDEF }));
		EXPECT_EQ(state.Get(3, 3), IOState::UNDEF);
	}

	TEST(TestIOState, TestArrayOp)
	{
		IOState state1(IOState::HI);
		EXPECT_EQ(state1[0], IOState::HI);
		EXPECT_EQ(IOState::HI, state1[0]);

		EXPECT_THROW(state1[-1], std::out_of_range);
		EXPECT_THROW(state1[1], std::out_of_range);

		state1[0] = IOState::LOW;
		EXPECT_EQ(IOState::LOW, state1.Get());
		EXPECT_THROW(state1[-1] = IOState::LOW, std::out_of_range);
		EXPECT_THROW(state1[1] = IOState::LOW, std::out_of_range);

		IOState state2(IOState::HI, 4);
		EXPECT_EQ(state2[3], IOState::HI);
		EXPECT_EQ(IOState::HI, state2[3]);

		EXPECT_THROW(state2[-1], std::out_of_range);
		EXPECT_THROW(state2[4], std::out_of_range);
		
		state2[3] = IOState::LOW;
		EXPECT_EQ(IOState::LOW, state2.Get(3));
		EXPECT_THROW(state2[-1] = IOState::LOW, std::out_of_range);
		EXPECT_THROW(state2[4] = IOState::LOW, std::out_of_range);

		const IOState::IO_STATE io = state2[3];
		EXPECT_EQ(io, state2.Get(3));
	}

	TEST(TestIOState, TestEqualOp)
	{
		IOState state1(IOState::HI);
		state1 = IOState::LOW;
		EXPECT_EQ(state1, IOState::LOW);

		IOState state2;
		state2 = state1;
		EXPECT_EQ(state2, state1);
		state2 = IOState::HI_Z;
		EXPECT_NE(state2, state1);

		IOState state3(IOState::HI, 4);

		IOState state4(IOState::LOW, 4);
		state4 = state3;
		EXPECT_EQ(state4, state3);
		state4[0] = IOState::HI_Z;
		EXPECT_NE(state4, state3);

		EXPECT_THROW(state4 = state1, std::invalid_argument);
		EXPECT_THROW(state1 = state4, std::invalid_argument);
	}

	TEST(TestIOState, ToString)
	{
		EXPECT_STREQ("x", IOState().ToString().c_str());
		EXPECT_STREQ("x", IOState(IOState::UNDEF).ToString().c_str());
		EXPECT_STREQ("0", IOState(IOState::LOW).ToString().c_str());
		EXPECT_STREQ("1", IOState(IOState::HI).ToString().c_str());
		EXPECT_STREQ("Z", IOState(IOState::HI_Z).ToString().c_str());		
		EXPECT_STREQ("{0, 0}", IOState(IOState::LOW, 2).ToString().c_str());
		EXPECT_STREQ("{0, 0, 0, 0}", IOState(IOState::LOW, 4).ToString().c_str());
		EXPECT_STREQ("{1, 1, 1, 1}", IOState(IOState::HI, 4).ToString().c_str());
		EXPECT_STREQ("{1, Z, 0, x}", IOState({IOState::HI, IOState::HI_Z, IOState::LOW, IOState::UNDEF}).ToString().c_str());

		std::ostringstream os;
		os << IOState({IOState::HI, IOState::HI_Z});
		EXPECT_STREQ("{1, Z}", os.str().c_str());
	}

	TEST(TestIOState, ToInt)
	{
		EXPECT_THROW(IOState().ToInt8(), std::invalid_argument);
		EXPECT_THROW(IOState().ToInt16(), std::invalid_argument);
		EXPECT_THROW(IOState(IOState::UNDEF).ToInt8(), std::invalid_argument);
		EXPECT_THROW(IOState(IOState::UNDEF).ToInt16(), std::invalid_argument);
		EXPECT_THROW(IOState(IOState::HI_Z).ToInt8(), std::invalid_argument);
		EXPECT_THROW(IOState(IOState::HI_Z).ToInt16(), std::invalid_argument);

		EXPECT_EQ(0, IOState(IOState::LOW).ToInt8());
		EXPECT_EQ(0, IOState(IOState::LOW).ToInt16());
		EXPECT_EQ(1, IOState(IOState::HI).ToInt8());
		EXPECT_EQ(1, IOState(IOState::HI).ToInt16());

		EXPECT_EQ(3, IOState({ IOState::HI, IOState::HI }).ToInt8());
		EXPECT_EQ(3, IOState({ IOState::HI, IOState::HI }).ToInt16());

		EXPECT_EQ(85, IOState({ IOState::HI, IOState::LOW, IOState::HI, IOState::LOW,
			IOState::HI, IOState::LOW, IOState::HI, IOState::LOW, }).ToInt8());
		EXPECT_EQ(85, IOState({ IOState::HI, IOState::LOW, IOState::HI, IOState::LOW,
			IOState::HI, IOState::LOW, IOState::HI, IOState::LOW, }).ToInt16());

		EXPECT_EQ(170, IOState({ IOState::LOW, IOState::HI, IOState::LOW, IOState::HI,
			IOState::LOW, IOState::HI, IOState::LOW, IOState::HI, }).ToInt8());
		EXPECT_EQ(170, IOState({ IOState::LOW, IOState::HI, IOState::LOW, IOState::HI,
			IOState::LOW, IOState::HI, IOState::LOW, IOState::HI, }).ToInt16());

		EXPECT_EQ(170, IOState({ IOState::LOW, IOState::HI, IOState::LOW, IOState::HI,
			IOState::LOW, IOState::HI, IOState::LOW, IOState::HI, IOState::LOW, IOState::HI }).ToInt8());
		EXPECT_EQ(682, IOState({ IOState::LOW, IOState::HI, IOState::LOW, IOState::HI,
			IOState::LOW, IOState::HI, IOState::LOW, IOState::HI, IOState::LOW, IOState::HI }).ToInt16());

		EXPECT_EQ(255, IOState({ IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI, }).ToInt8());
		EXPECT_EQ(65535, IOState({ IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI, }).ToInt16());
	}

	TEST(TestIOState, ToFromInt)
	{
		EXPECT_THROW(IOState::FromInt(-1, 1), std::out_of_range);
		EXPECT_THROW(IOState::FromInt(0, 0), std::out_of_range);
		EXPECT_THROW(IOState::FromInt(0, 17), std::out_of_range);

		IOState::FromInt(0, 16);

		EXPECT_EQ(IOState(IOState::LOW), IOState::FromInt(0, 1));
		EXPECT_EQ(IOState(IOState::HI), IOState::FromInt(1, 1));
		EXPECT_EQ(IOState({ IOState::HI, IOState::LOW}), IOState::FromInt(1, 2));
		EXPECT_EQ(IOState({ IOState::HI, IOState::HI, IOState::HI, IOState::HI,
			IOState::HI, IOState::HI, IOState::HI, IOState::HI }), IOState::FromInt(255, 8));
	}

	TEST(TestIOState, Random)
	{
		EXPECT_THROW(IOState::Random(0), std::out_of_range);
		EXPECT_THROW(IOState::Random(MAX_PINS+1), std::out_of_range);
		EXPECT_EQ(16, IOState::Random(MAX_PINS).GetWidth());
		EXPECT_EQ(1, IOState::Random(1).GetWidth());

		int total = 0;
		for (int i = 0; i < 1000; ++i)
		{
			total +=  IOState::Random(16).ToInt16();
		}
		int avg1 = total / 1000;

		total = 0;
		for (int i = 0; i < 1000; ++i)
		{
			total += IOState::Random(16).ToInt16();
		}
		int avg2 = total / 1000;

		ASSERT_NE(avg1, avg2);
		ASSERT_NEAR(avg1, 32768, 2000);
	}
}