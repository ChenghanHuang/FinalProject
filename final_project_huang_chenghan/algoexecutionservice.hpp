/**
* algoexecutionservice.hpp
* Construct algo executions.
*
* @author Chenghan Huang
*/
#ifndef ALGOEXECUTION_SERVICE_HPP
#define ALGOEXECUTION_SERVICE_HPP

#include "soa.hpp"
//#include "marketdataservice.hpp"
#include "executionservice.hpp"
//#include "products.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

template<typename T>
class AlgoExecutionOrder
{

public:

	AlgoExecutionOrder() {}

	// ctor
	AlgoExecutionOrder(ExecutionOrder<T> _executionorder)
	{
		product = _executionorder.GetProduct();
		executionorder = _executionorder;
	}

	ExecutionOrder<T> GetExecutionOrder()
	{
		return executionorder;
	}

	// Get the product
	const T& GetProduct() const
	{
		return product;
	}

private:
	T product;
	ExecutionOrder<T> executionorder;
};

template<typename T>
class AlgoExecutionService : public Service<string, AlgoExecutionOrder<T> >
{
public:
	map<string, AlgoExecutionOrder<T>> AlgoOrderMap;
	vector<ServiceListener<AlgoExecutionOrder<T>>*> ListenerList;
	PricingSide pricingside;
	int orderID;

	AlgoExecutionService()
	{
		pricingside = BID;
		orderID = 0;
	}

	static AlgoExecutionService<T>* Generate_Instance()
	{
		static AlgoExecutionService<T> instance;
		return &instance;
	}

	bool Aggress(BidOffer &_bidoffer)
	{
		Order bid = _bidoffer.GetBidOrder();
		Order offer = _bidoffer.GetOfferOrder();
		double bidPrice = bid.GetPrice();
		double offerPrice = offer.GetPrice();
		if (abs(bidPrice - offerPrice) < 1.0 / 32)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	ExecutionOrder<T> ConvertToExecutionOrder(T &_product, BidOffer &_bidoffer)
	{
		stringstream ss;
		ss << orderID;
		orderID++;
		string id = ss.str();

		Order executeorder = _bidoffer.GetOfferOrder();
		if (pricingside == BID)
		{
			executeorder = _bidoffer.GetBidOrder();
		}
		ExecutionOrder<T> executionorder(_product, pricingside, id, MARKET, executeorder.GetPrice(), executeorder.GetQuantity(), executeorder.GetQuantity(), "0", false);
		AlgoExecutionOrder<T> algoorder(executionorder);

		if (pricingside == BID)
		{
			pricingside = OFFER;
		}
		else
		{
			pricingside = BID;
		}

		for (int i = 0; i < ListenerList.size(); i++)
		{
			ServiceListener<AlgoExecutionOrder<T>>* listener = ListenerList[i];
			listener->ProcessAdd(algoorder);
		}

		return executionorder;
	}

	void ExecuteAlgoOrder(AlgoExecutionOrder<T> &_algoorder)
	{
		T BondProduct = _algoorder.GetProduct();
		AlgoOrderMap.insert(pair<string, AlgoExecutionOrder<T>>(BondProduct.GetProductId(), _algoorder));
	}

	virtual void OnMessage(AlgoExecutionOrder<T> &_algoorder) override
	{}

	virtual AlgoExecutionOrder<T>& GetData(string _id) override
	{
		return AlgoOrderMap[_id];
	}

	virtual void AddListener(ServiceListener<AlgoExecutionOrder<T>>* _listener) override
	{
		ListenerList.push_back(_listener);
	}

	virtual const vector< ServiceListener<AlgoExecutionOrder<T>>* >& GetListeners() const override
	{
		return ListenerList;
	}

private:
	int maxvol_vis = 1000000;
	int maxvol_hid = 10000000;
};

//template<typename T>
//class AlgoExecutionServiceListener : public ServiceListener<AlgoExecutionOrder<T>> 
//{
//public:
//	static AlgoExecutionServiceListener<T>* Generate_Instance() 
//	{
//		static AlgoExecutionServiceListener<T> instance;
//		return &instance;
//	}
//
//	 function overloading
//	void ProcessAdd(OrderBook<Bond> &data) 
//	{
//		_bondAlgoExecutionService->AddBook(data);
//	}
//
//	virtual void ProcessRemove(OrderBook<Bond> &data) {}
//
//	virtual void ProcessUpdate(OrderBook<Bond> &data) {}
//
//	AlgoExecutionService<T>* GetService() 
//	{
//		return _bondAlgoExecutionService;
//	}
//
//private:
//	AlgoExecutionService<T>* _bondAlgoExecutionService;
//	AlgoExecutionServiceListener() 
//	{ 
//		_bondAlgoExecutionService = AlgoExecutionService<T>::Generate_Instance(); 
//	}
//
//};

//template<typename T>
//class AlgoExecutionServiceListener : public ServiceListener<AlgoExecutionOrder<T>>
//{
//public:
//	virtual void ProcessAdd(ExecutionOrder<T> _executionorder)
//	{
//
//
//	}
//private:
//	ExecutionService<T>* executionservice;
//};
#endif