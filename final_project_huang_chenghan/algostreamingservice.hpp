/**
* algostreamingservice.hpp
* Construct algo price streams.
*
* @author Chenghan Huang
*/
#ifndef ALGOSTREAMING_SERVICE_HPP
#define ALGOSTREAMING_SERVICE_HPP

#include "soa.hpp"
#include "pricingservice.hpp"
#include "streamingservice.hpp"
//#include "products.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

template <typename T>
class Price;

template<typename T>
class AlgoPriceStream
{

public:

	AlgoPriceStream(){}

	// ctor
	AlgoPriceStream(PriceStream<T>& _pricestream)
	{
		product = _pricestream.GetProduct();
		pricestream = _pricestream;
	}

	// Get the product
	const T& GetProduct() const
	{
		return product;
	}

	const PriceStream<T>& GetPriceStream() const
	{
		return pricestream;
	}

private:
	T product;
	PriceStream<T> pricestream;
};

template<typename T>
class AlgoStreamingService : public Service<string,AlgoPriceStream<T>>
{
public:
	map<string, AlgoPriceStream<T>> AlgoStreamMap;
	vector<ServiceListener<AlgoPriceStream<T>>*> ListenerList;

	static AlgoStreamingService* Generate_Instance() 
	{
		static AlgoStreamingService instance;
		return &instance;
	}

	PriceStream<T> ConvertToPriceStream(Price<T> &_product)
	{
		srand((unsigned)time(NULL));
		double bidP, askP, bidvol_vis, bidvol_hid, askvol_vis, askvol_hid;
		bidP = _product.GetMid() - _product.GetBidOfferSpread();
		askP = _product.GetMid() + _product.GetBidOfferSpread();
		bidvol_vis = (rand() % (int)maxvol_vis) + 1;
		bidvol_hid = (rand() % (int)(maxvol_hid - bidvol_vis)) + bidvol_vis + 1;
		askvol_vis = (rand() % (int)maxvol_vis) + 1;
		askvol_hid = (rand() % (int)(maxvol_hid - askvol_vis)) + askvol_vis + 1;
		PriceStreamOrder bidorder(bidP, bidvol_vis, bidvol_hid, BID);
		PriceStreamOrder askorder(askP, askvol_vis, askvol_hid, OFFER);
		PriceStream<T> productstream(_product.GetProduct(), bidorder, askorder);
		AlgoPriceStream<T> algostream(productstream);

		for (int i = 0; i < ListenerList.size(); i++)
		{
			ServiceListener<AlgoPriceStream<T>>* listener = ListenerList[i];
			listener->ProcessAdd(algostream);
		}
		return productstream;
	}

	void AddAlgoStream(AlgoPriceStream<T> &_algostream)
	{
		T BondProduct = _algostream.GetProduct();
		AlgoStreamMap.insert(pair<string, AlgoPriceStream<T>>(BondProduct.GetProductId(), _algostream));
	}

	virtual void OnMessage(AlgoPriceStream<T> &data){}

	virtual AlgoPriceStream<T>& GetData(string _id)
	{
		return AlgoStreamMap[_id];
	}

	virtual void AddListener(ServiceListener<AlgoPriceStream<T>>* _listener)
	{
		ListenerList.push_back(_listener);
	}

	virtual const vector<ServiceListener<AlgoPriceStream<T>>* >& GetListeners() const
	{
		return ListenerList;
	}

private:
	int maxvol_vis = 1000000;
	int maxvol_hid = 10000000;
};

#endif