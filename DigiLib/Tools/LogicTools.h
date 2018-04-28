#pragma once
#include <vector>
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
			typedef std::vector<Core::IOPin::IO_STATE> IOStateList;

			static IOStateList GetTruthTable(Core::GateBase* gate);

			static void PrintTruthTable(Core::GateBase* gate);
			static void PrintTruthTable(std::vector<Core::IOPin*> const& inputs, std::vector<Core::IOPin*> const& outputs);

			static void PrintInternalConnections(Core::GateBase* gate);

		private:
			static void PrintTruthTable(size_t level, std::vector<Core::IOPin*> const& inputs, std::vector<Core::IOPin*> const& outputs);
			static void GetTruthTable(size_t level, std::vector<Core::IOPin*> const& inputs, Core::IOPinMapType& outputs, IOStateList& result);
		};
	}
}