//
// Created by think on 2017/12/25.
//

#ifndef INC_2_POSITIONSERVICELISTENER_H
#define INC_2_POSITIONSERVICELISTENER_H

#include <string>
#include <map>
#include <iostream>
#include "soa.hpp"
#include "tradebookingservice.hpp"
#include "pricingservice.hpp"
#include "riskservice.hpp"
#include "positionservice.hpp"

using namespace std;

template<typename T>
class RiskService;

template<typename T>
class PositionServiceListener : public ServiceListener<Position<T>>
{
public:
	static PositionServiceListener<T>* Generate_Instance()
	{
		static PositionServiceListener<T> instance;
		return &instance;
	}

	void ProcessAdd(Position<T> &data)
	{
		_bondRiskService->AddPosition(data);
	}

	void ProcessRemove(Position<T> &data) {}
	void ProcessUpdate(Position<T> &data) {}

	PositionService<T>* GetService()
	{
		return _bondPositionService;
	}

private:
	PositionService<T>* _bondPositionService;
	RiskService<T>* _bondRiskService;

	PositionServiceListener()
	{
		_bondPositionService = PositionService<T>::Generate_Instance();
		_bondRiskService = RiskService<T>::Generate_Instance();
	}

};
#endif //INC_2_POSITIONSERVICELISTENER_H
