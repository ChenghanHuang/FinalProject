#ifndef MARKET_DATA_SERVICE_LISTENER_HPP
#define MARKET_DATA_SERVICE_LISTENER_HPP


#include <string>
#include <vector>
#include <iostream>
#include "soa.hpp"
#include "products.hpp"
#include "marketdataservice.hpp"
#include "algoexecutionservice.hpp"

using namespace std;

template<typename T>
class MarketDataServiceListener : public ServiceListener<OrderBook<T>>
{
public:
	static MarketDataServiceListener<T>* Generate_Instance()
	{
		static MarketDataServiceListener<T> instance;
		return &instance;
	}

	virtual void ProcessAdd(OrderBook<T> &data)
	{
		T _product = data.GetProduct();
		BidOffer _bidoffer = marketdataservice->GetBestBidOffer(data);
		if (algoexecutionservice->Aggress(_bidoffer))
		{
			ExecutionOrder<T> executionorder = algoexecutionservice->ConvertToExecutionOrder(_product, _bidoffer);
		}
	}

	void ProcessRemove(OrderBook<T> &data) {}  // No implementation
	void ProcessUpdate(OrderBook<T> &data) {}  // No implementation

private:
	AlgoExecutionService<T>* algoexecutionservice;
	MarketDataService<T>* marketdataservice;

	MarketDataServiceListener<T>()
	{
		algoexecutionservice = AlgoExecutionService<T>::Generate_Instance();
	}
};

#endif