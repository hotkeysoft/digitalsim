#include "pch.h"
#pragma warning( disable:4251 )

#include "TestCommon.h"
#include "Core\GateBase.h"
#include "Core\CompositeGate.h"
#include "BasicGates\NOTGate.h"
#include "BasicGates\ANDGate.h"
#include "BasicGates\NANDGate.h"
#include "BasicGates\ORGate.h"
#include "BasicGates\XORGate.h"
#include "BasicGates\WireGate.h"
#include "BasicGates\BufferGate.h"
#include "Tools\LogicTools.h"

namespace UnitTests
{
	using namespace Core;

	// Not validated for now, just for code coverage
	TEST(TestLogicTools, PrintTruthTable)
	{
		Tools::LogicTools::PrintTruthTable(BasicGates::ANDGate::Create());
	}

	// Not validated for now, just for code coverage
	TEST(TestLogicTools, PrintTruthTable2)
	{
		GatePtr andGate = BasicGates::ANDGate::Create();
		Tools::LogicTools::PrintTruthTable({ andGate->GetPin("in1") }, { andGate->GetPin("out") });

		Tools::LogicTools::PrintTruthTable({ andGate->GetPin("in1") }, {  });

		Tools::LogicTools::PrintTruthTable({ }, { andGate->GetPin("out") });
	}

	// Not validated for now, just for code coverage
	TEST(TestLogicTools, PrintInternalConnections)
	{
		Tools::LogicTools::PrintInternalConnections(BuildDecoder());
	}

	TEST(TestLogicTools, PrintPinInfo)
	{
		Tools::LogicTools::PrintPinInfo(BuildDecoder());
		Tools::LogicTools::PrintPinInfo(BasicGates::BufferGate::Create(4));
	}
}