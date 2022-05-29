#pragma once
#include "../sdk/interfaces.h"
#include <functional>

namespace eventlistner
{
	class listener : public IGameEventListener2
	{
	public:
		listener() = default;
		~listener() = default;

		void FireGameEvent(IGameEvent* p_event);
		int GetEventDebugID()
		{
			return 0x2A;
		};
	};

	void undo();
	void add(const std::string_view event_name, const std::function<void(IGameEvent* e)>& callback);
	inline bool create_event_callback_impl(std::string_view event_name, const std::function<void(IGameEvent * e)> & callback) {
		add(event_name, callback);
		return true;
	}
	void on_init();
}
#define get_event_callback( func ) bool m_event_ ## func = eventlistner::create_event_callback_impl( #func, func );
