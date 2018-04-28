#pragma once

#include "Core\GateBase.h"

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

namespace DigiLib {
	namespace BasicGates {

		class DllExport ANDGate : public Core::GateBase
		{
		public:
			ANDGate(size_t inputs = 2);
			virtual ~ANDGate() {}

			virtual GateBase* Clone(const char* name);

			virtual void ComputeState();

		};

	}
}
