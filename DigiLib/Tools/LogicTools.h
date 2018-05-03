#pragma once
#include <vector>
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
			static std::string PrintTruthTable(std::vector<Core::IOPinPtr> const& inputs, std::vector<Core::IOPinPtr> const& outputs);

			static std::string PrintInternalConnections(Core::GatePtr gate);

			static std::string PrintPinInfo(Core::GatePtr gate);

		private:

			static void PrintPinInfo(std::ostream& os, Core::GatePtr gate, const Core::IOPinNameToIDMapType& pins);
			static void PrintTruthTable(std::ostream& os, size_t level, std::vector<Core::IOPinPtr> const& inputs, std::vector<Core::IOPinPtr> const& outputs);
			static void GetTruthTable(size_t level, std::vector<Core::IOPinPtr> const& inputs, Core::GatePtr gate, const Core::IOPinNameToIDMapType& outputs, ResultListType& result);
		};
	}
}