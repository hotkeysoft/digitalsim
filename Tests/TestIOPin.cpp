#include "pch.h"
#pragma warning( disable:4251 )

#include "TestCommon.h"
#include "Core/IOState.h"
#include "Core/GateBase.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\BufferGate.h"

namespace UnitTests
{
	using namespace Core;

	TEST(TestIOPin, Mask)
	{
		GatePtr buf = BasicGates::BufferGate::Create(10);

		IOPinPtr in = buf->GetPin("in");
		IOPinPtr in0 = buf->FindPin("in[0]");
		IOPinPtr in1 = buf->FindPin("in[1]");
		IOPinPtr in02 = buf->FindPin("in[0-2]");
		IOPinPtr in58 = buf->FindPin("in[5-8]");
		IOPinPtr in89 = buf->FindPin("in[8-9]");
		IOPinPtr in9 = buf->FindPin("in[9]");

		ASSERT_NE(nullptr, in);
		ASSERT_NE(nullptr, in0);
		ASSERT_NE(nullptr, in1);
		ASSERT_NE(nullptr, in02);
		ASSERT_NE(nullptr, in58);
		ASSERT_NE(nullptr, in89);
		ASSERT_NE(nullptr, in9);

		IOState inMask = in->GetMask();
		IOState in0Mask = in0->GetMask();
		IOState in1Mask = in1->GetMask();
		IOState in02Mask = in02->GetMask();
		IOState in58Mask = in58->GetMask();
		IOState in89Mask = in89->GetMask();
		IOState in9Mask = in9->GetMask();

		EXPECT_EQ(inMask, IOState(  { IOState::LOW,		IOState::LOW,	IOState::LOW,	IOState::LOW,	IOState::LOW,	IOState::LOW,	IOState::LOW,	IOState::LOW,	IOState::LOW,	IOState::LOW, }));

		EXPECT_EQ(in0Mask, IOState( { IOState::LOW,		IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF, }));
		EXPECT_EQ(in1Mask, IOState(	{ IOState::UNDEF,	IOState::LOW,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF, }));
		EXPECT_EQ(in02Mask, IOState({ IOState::LOW,		IOState::LOW,	IOState::LOW,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF, }));
		EXPECT_EQ(in58Mask, IOState({ IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::LOW,	IOState::LOW,	IOState::LOW,	IOState::LOW,	IOState::UNDEF, }));
		EXPECT_EQ(in89Mask, IOState({ IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::LOW,	IOState::LOW, }));
		EXPECT_EQ(in9Mask, IOState( { IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::UNDEF,	IOState::LOW, }));

		IOState mask = in0Mask | in1Mask | in02Mask | in58Mask | in89Mask | in9Mask;
		EXPECT_EQ(mask, IOState(	{ IOState::LOW,		IOState::LOW,	IOState::LOW,	IOState::UNDEF,	IOState::UNDEF,	IOState::LOW,	IOState::LOW,	IOState::LOW,	IOState::LOW,	IOState::LOW, }));

		EXPECT_FALSE(buf->FindPin("in[3]")->Overlaps(mask));
		EXPECT_FALSE(buf->FindPin("in[4]")->Overlaps(mask));
		EXPECT_FALSE(buf->FindPin("in[3-4]")->Overlaps(mask));
		EXPECT_TRUE(buf->FindPin("in")->Overlaps(mask));
		EXPECT_TRUE(buf->FindPin("in[4-5]")->Overlaps(mask));
		EXPECT_TRUE(buf->FindPin("in[2-3]")->Overlaps(mask));
	}
}