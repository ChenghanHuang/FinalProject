#ifndef ALGOEXECUTION_SERVICE_LISTENER_HPP
#define ALGOEXECUTION_SERVICE_LISTENER_HPP


#include "soa.hpp"
#include "executionservice.hpp"
#include "algoexecutionservice.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

template<typename T>
class AlgoExecutionServiceListener : public ServiceListener<AlgoExecutionOrder<T>>
{
public:
	static AlgoExecutionServiceListener<T>* Generate_Instance()
	{
		static AlgoExecutionServiceListener<T> instance;
		return &instance;
	}
	// function overloading
	void ProcessAdd(AlgoExecutionOrder<T> &data)
	{
		auto executionorder = data.GetExecutionOrder();
		_bondAlgoExecutionService->ExecuteAlgoOrder(data);
		_bondExecutionService->ExecuteOrder(executionorder, CME);
	}
	void ProcessRemove(AlgoExecutionOrder<T> &data) {}  // No implementation
	void ProcessUpdate(AlgoExecutionOrder<T> &data) {}  // No implementation

	AlgoExecutionService<T>* GetService()
	{
		return _bondAlgoExecutionService;
	}
private:
	AlgoExecutionService<T>* _bondAlgoExecutionService;
	ExecutionService<T>* _bondExecutionService;
	AlgoExecutionServiceListener() 
	{
		_bondAlgoExecutionService = AlgoExecutionService<T>::Generate_Instance(); 
		_bondExecutionService = ExecutionService<T>::Generate_Instance();
	}
};

#endif