#ifndef EXECUTION_SERVICE_LISTENER_HPP
#define EXECUTION_SERVICE_LISTENER_HPP

#include <string>
#include <map>
#include "soa.hpp"
#include "marketdataservice.hpp"
#include "tradebookingservice.hpp"
#include "historicaldataservice.hpp"

template<typename T>
class ExecutionServiceListener : public ServiceListener<ExecutionOrder<T>> 
{
public:
	static ExecutionServiceListener* Generate_Instance() {
		static ExecutionServiceListener instance;
		return &instance;
	}

	// function overloading
	void ProcessAdd(ExecutionOrder<T> &data) 
	{
		_bondHistoryExecutionService->OnMessage(data);
		_bondHistoryExecutionService->PersistData(data.GetProduct().GetProductId(), data);
	}
	void ProcessRemove(ExecutionOrder<T> &data) {}
	void ProcessUpdate(ExecutionOrder<T> &data) {}

	ExecutionService<T>* GetService()
	{
		return _bondExecutionService;
	}

private:
	ExecutionService<T>* _bondExecutionService;
	BondHistoricalExecutionService * _bondHistoryExecutionService;

	ExecutionServiceListener()
	{
		_bondExecutionService = ExecutionService<T>::Generate_Instance();
		_bondHistoryExecutionService = BondHistoricalExecutionService::Generate_Instance();
	}
};

#endif