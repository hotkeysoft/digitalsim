#pragma once

#include <set>
#include <map>
#include "Common.h"
#include "IOPin.h"
#include "IOConnection.h"
#include "GateBase.h"

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

namespace DigiLib
{
	namespace Core
	{
		typedef std::map<std::string, GatePtr > GateMapType;

		class DllExport CompositeGate : public GateBase
		{
		public:
			static CompositeGatePtr Create(const char*name);
			GatePtr Clone(const char * name, bool deep = true) override;

			void Reset() override;
			void ResetPins() override;
			void InitializeState() override;

			using GateBase::AddInput;
			using GateBase::AddOutput;

			IOPinPtr FindPin(const char* name) override;

			void SetName(const char *name) override;

			void SetMode(Mode mode, SimulatorRef simulator) override;

			virtual void AddGate(const char* name, GatePtr gate);
			size_t GetGateCount(bool recursive = false) noexcept override;
			virtual GatePtr GetGate(const char*  name);
			virtual GateMapType& GetInternalGates() noexcept { return m_internalGates; }

		protected:
			CompositeGate(const char* name);
			GateMapType m_internalGates;

			CompositeGatePtr thisCompositeGate() { return std::dynamic_pointer_cast<CompositeGate>(shared_from_this()); }

			static void InternalCloneOutputs(CompositeGatePtr source, CompositeGatePtr clone);
			static void InternalCloneInputs(CompositeGatePtr source, CompositeGatePtr clone);
			static void InternalCloneGates(CompositeGatePtr source, CompositeGatePtr clone);
			static void InternalCloneLinks(GatePtr source, GatePtr clone);
			static void InternalCloneInnerLinks(GatePtr source, GatePtr clone);

			void ValidateGateName(const char* name, bool checkDuplicate = true);

			static bool ExtractGatePinName(const std::string in, std::string& gate, std::string& pin);

			struct shared_enabler;
		};
	}
}
