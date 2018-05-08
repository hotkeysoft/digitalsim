#pragma once

#include <vector>
#include "Core\GateBase.h"
#include "Core\IOPin.h"

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
			static Core::GatePtr Create(size_t inputs = 2);			
			Core::GatePtr Clone(const char * name, bool deep = true) override;

			void ComputeState() override;

		protected:
			ANDGate(size_t pins = 2) noexcept;
			void Init() override;

			size_t m_inputs;
			std::vector<Core::IOPinPtr> m_inputPins;
			Core::IOPinPtr m_out;

			struct shared_enabler;
		};

	}
}
