#ifndef RISK_SERVICE_LISTENER_HPP
#define RISK_SERVICE_LISTENER_HPP

#include <vector>
#include <iostream>
#include "soa.hpp"
#include "riskservice.hpp"
#include "historicaldataservice.hpp"

class BondHistoricalPV01Service;

template<typename T>
class RiskServiceListener : public ServiceListener<PV01<T>>
{
public:
	static RiskServiceListener<T>* Generate_Instance()
	{
		static RiskServiceListener<T> instance;
		return &instance;
	}

	// function overloading
	void ProcessAdd(PV01<Bond> &data)
	{
		_bondHistoryPV01Service->OnMessage(data);
		_bondHistoryPV01Service->PersistData(data.GetProduct().GetProductId(), data); // to write.
	}

	void ProcessRemove(PV01<Bond> &data) {}
	void ProcessUpdate(PV01<Bond> &data) {}

	RiskService<T>* GetService()
	{
		return _bondRiskService;
	}

private:
	BondHistoricalPV01Service* _bondHistoryPV01Service;
	RiskService<T>* _bondRiskService;

	RiskServiceListener()
	{
		_bondHistoryPV01Service = BondHistoricalPV01Service::Generate_Instance();
		_bondRiskService = RiskService<T>::Generate_Instance();
	}

};
#endif