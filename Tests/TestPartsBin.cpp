#include "pch.h"
#pragma warning( disable:4251 )

#include "Core/IOState.h"
#include "Core/GateBase.h"
#include "Core/Simulator.h"
#include "BasicGates/ANDGate.h"
#include "BasicGates/ORGate.h"
#include "BasicGates/BufferGate.h"
#include "Parser/TextParser.h"
#include "Parser/PartsBin.h"

namespace UnitTests
{
	using namespace Core;
	using namespace Parser;

	TEST(TestPartsBin, Create)
	{
		PartsBinPtr parts = PartsBin::Create();
		PartsBinPtr parts2 = PartsBin::Create();

		EXPECT_EQ(0, parts->GetPartCount());
		EXPECT_NE(parts.get(), parts2.get());
	}

	TEST(TestPartsBin, Add)
	{
		PartsBinPtr parts = PartsBin::Create();

		EXPECT_THROW(parts->AddPart("AND", nullptr), std::invalid_argument);

		EXPECT_THROW(parts->AddPart(nullptr, BasicGates::ANDGate::Create()), std::invalid_argument);
		EXPECT_THROW(parts->AddPart("", BasicGates::ANDGate::Create()), std::invalid_argument);
		EXPECT_THROW(parts->AddPart(" ", BasicGates::ANDGate::Create()), std::invalid_argument);
		EXPECT_THROW(parts->AddPart("1", BasicGates::ANDGate::Create()), std::invalid_argument);
		EXPECT_THROW(parts->AddPart("/A", BasicGates::ANDGate::Create()), std::invalid_argument);
		EXPECT_THROW(parts->AddPart(" AND", BasicGates::ANDGate::Create()), std::invalid_argument);
		EXPECT_THROW(parts->AddPart("AND ", BasicGates::ANDGate::Create()), std::invalid_argument);
		EXPECT_EQ(0, parts->GetPartCount());

		parts->AddPart("AND", BasicGates::ANDGate::Create());
		EXPECT_EQ(1, parts->GetPartCount());

		EXPECT_THROW(parts->AddPart("AND", BasicGates::ANDGate::Create()), std::invalid_argument);
		EXPECT_EQ(1, parts->GetPartCount());

		parts->AddPart("OR", BasicGates::ORGate::Create());
		EXPECT_EQ(2, parts->GetPartCount());
	}

	TEST(TestPartsBin, Clear)
	{
		PartsBinPtr parts = PartsBin::Create();
		parts->AddPart("AND", BasicGates::ANDGate::Create());
		parts->AddPart("OR", BasicGates::ORGate::Create());

		EXPECT_EQ(2, parts->GetPartCount());
		parts->Clear();
		EXPECT_EQ(0, parts->GetPartCount());
		parts->Clear();
		EXPECT_EQ(0, parts->GetPartCount());
		parts->AddPart("AND", BasicGates::ANDGate::Create());
		EXPECT_EQ(1, parts->GetPartCount());
		parts->Clear();
		EXPECT_EQ(0, parts->GetPartCount());
	}

	TEST(TestPartsBin, Find)
	{
		PartsBinPtr parts = PartsBin::Create();
		parts->AddPart("AND", BasicGates::ANDGate::Create());
		parts->AddPart("OR", BasicGates::ORGate::Create());

		EXPECT_THROW(parts->Find(nullptr), std::invalid_argument);
		EXPECT_EQ(nullptr, parts->Find(""));
		EXPECT_EQ(nullptr, parts->Find(" "));
		EXPECT_EQ(nullptr, parts->Find(" AND"));
		EXPECT_EQ(nullptr, parts->Find("AND "));
		EXPECT_EQ(nullptr, parts->Find(" /AND"));

		EXPECT_NE(nullptr, parts->Find("AND"));
		EXPECT_NE(nullptr, parts->Find("OR"));
	}
}