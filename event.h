#pragma once

#include <unordered_set>

#include"delegate.h"

template<typename ReturnType, typename ...FunctionArguments>
class EventBusClient {
protected:
	typedef Delegate<ReturnType, FunctionArguments...> DelegateType;
	typedef std::unordered_set<DelegateType> ContainerType;
private:
	ContainerType listeners;
protected:	inline ContainerType& getListeners() { return listeners; }

public:
	inline EventBusClient() { listeners = ContainerType(); }
	inline EventBusClient(size_t reserve) { listeners = ContainerType(reserve); }

	inline void	subscribe(DelegateType delegate) { listeners.insert(delegate); }
	inline void unsubscribe(DelegateType delegate) { listeners.erase(delegate); }

	inline size_t getNumListeners() const { return listeners.size(); }
};

template<typename ReturnType, typename ...FunctionArguments>
class EventBus : public EventBusClient<ReturnType, FunctionArguments...> {
public:
	void fireEvent(FunctionArguments... args) {
		auto listeners = this->getListeners();
		for (auto i = listeners.begin(); i != listeners.end(); i++) {
			i->invoke(args...);
		}
	}

	inline EventBusClient<ReturnType, FunctionArguments...>& operator!() { return static_cast<EventBusClient<ReturnType, FunctionArguments...>&>(*this); }
};