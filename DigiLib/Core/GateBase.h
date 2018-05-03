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
			GateBase(const char* name);
			virtual ~GateBase() = default;
			GateBase(const GateBase&) = delete;
			GateBase& operator=(const GateBase&) = delete;
			GateBase(GateBase&&) = delete;
			GateBase& operator=(GateBase&&) = delete;

			virtual void Init() {}

			std::string GetName() { return m_name; }
			std::string GetFullName();
			virtual void SetName(const char*);

			virtual GatePtr Clone(const char* name) = 0;

			virtual IOPinPtr GetPin(size_t pinID) { return m_ioPins[pinID]; }
			virtual IOPinPtr GetPin(const char* name);
			virtual IOPinPtr GetPin(const char* name, size_t pin);
			virtual IOPinPtr GetPin(const char* name, size_t low, size_t hi);

			virtual PinConnectionsType& GetConnectedFromPins(size_t pinID);
			virtual PinConnectionsType& GetConnectedToPins(size_t pinID);

			virtual PinConnectionsType& GetConnectedFromPins(const char* sourcePin);
			virtual PinConnectionsType& GetConnectedToPins(const char* sourcePin);
			
			virtual PinConnectionsType& GetConnectedFromPins(IOPinPtr sourcePin);
			virtual PinConnectionsType& GetConnectedToPins(IOPinPtr sourcePin);

			virtual ConnectedPinsType& GetConnectedFromPins() noexcept { return m_connectedFromPins; }
			virtual ConnectedPinsType& GetConnectedToPins() noexcept { return m_connectedToPins; }

			GateRef GetParent() noexcept { return m_parent; }
			virtual void SetParent(GateRef parent);

			virtual void ComputeState() noexcept(false) {}

			size_t GetInputCount() noexcept { return m_inputPinsNames.size(); }
			size_t GetOutputCount() noexcept { return m_outputPinsNames.size(); }

			const IOPinNameToIDMapType& GetInputPins() noexcept { return m_inputPinsNames; }
			const IOPinNameToIDMapType& GetOutputPins() noexcept { return m_outputPinsNames; }

			void ConnectPins(IOPinPtr source, IOPinPtr target);

		protected:
			virtual IOPinPtr AddInput(const char*  name, size_t width = 1);
			virtual IOPinPtr AddOutput(const char*  name, size_t width = 1, IOPin::IO_DIRECTION dir = IOPin::IO_DIRECTION::OUTPUT);

			std::string m_name;
			GateRef m_parent;
			size_t m_ioPinCount;

			ConnectedPinsType m_connectedToPins;
			ConnectedPinsType m_connectedFromPins;

			IOPinNameToIDMapType m_inputPinsNames;
			IOPinNameToIDMapType m_outputPinsNames;

			IOPinListType m_ioPins;

			static AllowedConnectionMapType m_insideInsideMap;
			static AllowedConnectionMapType m_insideParentMap;
			static AllowedConnectionMapType m_parentInsideMap;
			static void InitAllowedConnectionMaps();

			bool IsValidPinName(const char* name);
			void ValidatePinName(const char* name);
			void ValidatePinWidth(size_t width);

			virtual bool IsValidGateName(const char* name);
			virtual void ValidateGateName(const char* name);
		};
	}
}