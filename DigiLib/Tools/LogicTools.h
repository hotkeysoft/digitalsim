#pragma once
#include <vector>
#include <sstream>
#include "Core\Common.h"
#include "Core\IOState.h"
#include "Core\IOPin.h"
#include "Core\GateBase.h"

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

namespace DigiLib {
	namespace Tools {

		class DllExport LogicTools
		{
		public:
			typedef std::vector<Core::IOState::IO_STATE> ResultListType;

			static ResultListType GetTruthTable(Core::GatePtr gate);

			static std::string PrintTruthTable(Core::GatePtr gate);
			static std::string PrintTruthTable(const Core::IOPinListType & inputs, const Core::IOPinListType & outputs);

			static std::string PrintInternalConnections(Core::GatePtr gate);
			static std::string PrintInternalStates(Core::GatePtr gate, size_t maxLevel = SIZE_MAX);

			static std::string PrintPinInfo(Core::GatePtr gate);
			
			static std::string LogicAnalyser(Core::SimulatorPtr sim, const Core::IOPinListType & inputs, size_t maxTicks = SIZE_MAX, size_t modulo = 1);

		private:
			static void PrintPinInfo(std::ostream& os, Core::GatePtr gate, const Core::IOPinNameToIDMapType& pins);
			static void PrintTruthTable(std::ostream& os, size_t level, const Core::IOPinListType & inputs, const Core::IOPinListType outputs);
			static void GetTruthTable(size_t level, std::vector<Core::IOPinPtr> const& inputs, Core::GatePtr gate, const Core::IOPinNameToIDMapType& outputs, ResultListType& result);
			static void OutputStates(const Core::IOPinListType & inputs, std::vector<std::ostringstream> & outputs);
		};
	}
}