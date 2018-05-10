#pragma once

#include <set>
#include <map>
#include <memory>
#include "Common.h"
#include "IOPin.h"
#include "IOConnection.h"
#pragma warning ( disable:26426 )

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
		typedef std::vector<IOPinPtr> IOPinListType;
		typedef std::map<std::string, size_t> IOPinNameToIDMapType;
		typedef std::set<IOConnection> PinConnectionsType;
		typedef std::vector<PinConnectionsType> ConnectedPinsType;
		typedef std::map<IOPin::IO_DIRECTION, std::map<IOPin::IO_DIRECTION, bool> > AllowedConnectionMapType;

		class DllExport GateBase : public std::enable_shared_from_this<GateBase>
		{
		public:
			enum Mode {ASYNC, SYNC};

			GateBase(const char* name, size_t delay = 1);
			virtual ~GateBase() = default;
			GateBase(const GateBase&) = delete;
			GateBase& operator=(const GateBase&) = delete;
			GateBase(GateBase&&) = delete;
			GateBase& operator=(GateBase&&) = delete;

			virtual void Init();
			virtual void InitializeState();

			std::string GetName() { return m_name; }
			std::string GetFullName();
			virtual void SetName(const char*);

			virtual GatePtr Clone(const char* name, bool deep = true) = 0;

			virtual Mode GetMode() { return this->m_mode; }
			virtual void SetMode(Mode mode, SimulatorRef simulator = nullptr);
			virtual SimulatorRef GetSimulator() { return m_simulator; }

			size_t GetDelay() { return m_delay;  }

			virtual size_t GetGateCount(bool recursive = false) noexcept { return 1; }

			virtual void Reset();
			virtual void ResetPins();

			virtual IOPinPtr FindPin(const char* name);

			virtual IOPinPtr GetPin(size_t pinID) { return m_ioPins[pinID]; }
			virtual IOPinPtr GetPin(const char* name);
			virtual IOPinPtr GetPin(const char* name, size_t pin);
			virtual IOPinPtr GetPin(const char* name, size_t low, size_t hi);

			virtual PinConnectionsType& GetConnectedFromPin(const char* sourcePin);
			virtual PinConnectionsType& GetConnectedToPin(const char* sourcePin);
			
			virtual PinConnectionsType& GetConnectedFromPin(IOPinPtr sourcePin);
			virtual PinConnectionsType& GetConnectedToPin(IOPinPtr sourcePin);

			ConnectedPinsType& GetConnectedFromPins() noexcept { return m_connectedFromPins; }
			ConnectedPinsType& GetConnectedToPins() noexcept { return m_connectedToPins; }

			GateRef GetParent() noexcept { return m_parent; }
			virtual void SetParent(GateRef parent);

			virtual void ComputeState() noexcept(false) {}

			size_t GetInputCount() noexcept { return m_inputPinsNames.size(); }
			size_t GetOutputCount() noexcept { return m_outputPinsNames.size(); }

			const IOPinNameToIDMapType& GetInputPins() noexcept { return m_inputPinsNames; }
			const IOPinNameToIDMapType& GetOutputPins() noexcept { return m_outputPinsNames; }

			bool ConnectPins(IOPinPtr source, IOPinPtr target, bool inverted = false);

			static bool IsValidGateName(const char* name);
			static bool IsValidPinName(const char* name);

		protected:
			virtual IOPinPtr AddInput(const char*  name, size_t width = 1);
			virtual IOPinPtr AddOutput(const char*  name, size_t width = 1, IOPin::IO_DIRECTION dir = IOPin::IO_DIRECTION::OUTPUT);
			void InitVccGndPins();

			std::string m_name;
			Mode m_mode;
			size_t m_delay;
			SimulatorRef m_simulator;
			GateRef m_parent;
			size_t m_ioPinCount;

			ConnectedPinsType m_connectedToPins;
			ConnectedPinsType m_connectedFromPins;

			IOPinNameToIDMapType m_inputPinsNames;
			IOPinNameToIDMapType m_outputPinsNames;

			IOPinListType m_ioPins;

			IOPinPtr m_vccPin;
			PinConnectionsType m_connectedToVcc;

			IOPinPtr m_gndPin;
			PinConnectionsType m_connectedToGnd;

			static AllowedConnectionMapType m_outsideOutsideMap;
			static AllowedConnectionMapType m_insideInsideMap;
			static AllowedConnectionMapType m_insideParentMap;
			static AllowedConnectionMapType m_parentInsideMap;
			static void InitAllowedConnectionMaps();
			
			void ValidatePinName(const char* name);
			void ValidatePinWidth(size_t width);

			PinConnectionsType& GetConnectedFromPin(size_t pinID);
			PinConnectionsType& GetConnectedToPin(size_t pinID);

			bool CanConnectToTarget(const IOConnection & link);

			virtual void ValidateGateName(const char* name);
		};
	}
}