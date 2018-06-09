#include "stdafx.h"
#include "Simulator.h"

namespace DigiLib
{
	namespace Core
	{
		Simulator::Simulator(const char* name) : CompositeGate(name), m_currentTick(0)
		{
			ValidateGateName(name, false);
			m_mode = SYNC;
			m_simulator = this;
		}

		SimulatorPtr Simulator::Create(const char* name)
		{
			if (name == nullptr )
			{
				throw std::invalid_argument("name is null");
			}

			auto ptr = std::make_shared<shared_enabler>(name);
			SimulatorPtr gate = std::static_pointer_cast<Simulator>(ptr);
			gate->Init();
			return gate;
		}

		GatePtr Simulator::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>(name);
			SimulatorPtr clone = std::static_pointer_cast<Simulator>(ptr);
			clone->Init();
			clone->Reset();
			SimulatorPtr source = thisSimulator();

			CompositeGate::InternalCloneInputs(source, clone);
			CompositeGate::InternalCloneOutputs(source, clone);
			CompositeGate::InternalCloneGates(source, clone);
			if (deep)
			{
				CompositeGate::InternalCloneLinks(source, clone);
			}

			return clone;
		}

		void Simulator::Reset()
		{
			m_currentTick = 0;
			m_eventQueue.clear();
			ResetPins();
			InitializeState();
		}

		size_t Simulator::Tick()
		{
			if (m_currentTick == 0)
			{
				InitializeState();
			}

			size_t currentTick = ++m_currentTick;
			
			auto find = m_eventQueue.find(currentTick);
			while (find != m_eventQueue.end())
			{
				find->second.Run();
				m_eventQueue.erase(find);
				find = m_eventQueue.find(currentTick);
			}
			
			return m_currentTick;
		}

		size_t Simulator::Run(size_t maxTicks)
		{
			if (maxTicks < 1)
			{
				throw std::out_of_range("Requires a positive number of ticks");
			}
			size_t max = m_currentTick + maxTicks;

			while (m_eventQueue.size() && m_currentTick < max)
			{
				Tick();
			}

			return m_currentTick;
		}

		void Simulator::Pulse(IOPinPtr pin, size_t begin, size_t end, bool negative)
		{
			if (begin < m_currentTick + 1)
			{
				throw std::out_of_range("can't post event before current tick");
			}
			if (end < begin + 1)
			{
				throw std::out_of_range("pulse must be at least one tick wide");
			}

			PostEvent({ negative? IOState::LOW : IOState::HI, pin }, begin);
			PostEvent({ negative? IOState::HI : IOState::LOW, pin }, end);
		}

		void Simulator::Pulse(IOPinPtr pin, IOState state, size_t begin, size_t end)
		{
			if (begin < m_currentTick + 1)
			{
				throw std::out_of_range("can't post event before current tick");
			}
			if (end < begin + 1)
			{
				throw std::out_of_range("pulse must be at least one tick wide");
			}
			if (state.GetWidth() != pin->GetWidth())
			{
				throw std::out_of_range("pin width mismatch");
			}

			PostEvent({ state, pin } , begin);
			PostEvent({ IOState(IOState::LOW, pin->GetWidth()), pin }, end);
		}

		void Simulator::Clock(IOPinPtr pin, size_t hiWidth, size_t lowWidth, size_t begin, size_t end, bool negative)
		{
			if (begin < m_currentTick + 1)
			{
				throw std::out_of_range("can't post event before current tick");
			}
			if (end < begin + 1 || hiWidth < 1 || lowWidth < 1)
			{
				throw std::out_of_range("pulse and width must be at least one tick wide");
			}

			size_t i = begin;
			while (i < end)
			{
				Pulse(pin, i, i + hiWidth, negative);
				i += hiWidth + lowWidth;
			}
		}

		void Simulator::AddGate(const char* name, GatePtr gate)
		{
			CompositeGate::AddGate(name, gate);
			gate->SetMode(SYNC, this);
		}

		void Simulator::PostEvent(const IOConnection &connection)
		{
			PostEvent(connection, m_currentTick + 1 );
		}

		void Simulator::PostEvent(const IOConnection &connection, size_t at)
		{
			m_eventQueue.insert({ at, connection });
		}

		void Simulator::PostEventRelative(const IOConnection &connection, size_t delay)
		{
			PostEvent(connection, m_currentTick + delay);
		}

		struct Simulator::shared_enabler : public Simulator
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: Simulator(std::forward<Args>(args)...)
			{
			}
		};
	}
}