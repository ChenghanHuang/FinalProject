/**
* algostreamingservice.hpp
* Construct algo price streams.
*
* @author Chenghan Huang
*/
#ifndef GUISERVICE_SERVICE_HPP
#define GUISERVICE_SERVICE_HPP

#include "soa.hpp"
#include "pricingservice.hpp"
#include "products.hpp"
#include <windows.h>
#include <iostream>
#include <time.h>

template<typename T>
class GUIService : public Service<string, Price <T> >
{
public:
	map<string, Price<T>> PriceMap;
	vector<ServiceListener<Price<T>>*> ListenerList;

	GUIService()
	{

	}

	void AddPrice(Price<T> &_data)
	{
		T BondProduct = _data.GetProduct();
		PriceMap.insert(pair<string, Price<T>>(BondProduct.GetProductId(), _data);

		for (int i = 0; i < ListenerList.size(); i++)
		{
			ServiceListener<Price<T>>* listener = ListenerList[i];
			listener->ProcessAdd(_data);
		}
	}

	virtual Price<T>& GetData(string _id)
	{
		return PriceMap[_id];
	}

	virtual void AddListener(ServiceListener<Price<T>>* _listener)
	{
		ListenerList.push_back(_listener);
	}

	virtual const vector< ServiceListener<Price<T>>* >& GetListeners()
	{
		return ListenerList;
	}

private:
	int maxvol_vis = 1000000;
	int maxvol_hid = 10000000;
};

template<typename T>
class GUIServiceListener : public ServiceListener<AlgoPriceStream<T>>
{
public:
	virtual void ProcessAdd(Price<T> _data)
	{


	}
private:
	PricingService* pricingservice;
};
#endif