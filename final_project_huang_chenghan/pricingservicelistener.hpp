#ifndef PRICING_SERVICE_LISTENER_HPP
#define PRICING_SERVICE_LISTENER_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "streamingservice.hpp"
#include "algostreamingservice.hpp"
#include "soa.hpp"
#include "products.hpp"

template<typename T>
class PricingServiceListener : public ServiceListener<Price<T>>
{
public:
	static PricingServiceListener<T>* Generate_Instance()
	{
		static PricingServiceListener<T> instance;
		return &instance;
	}

	virtual void ProcessAdd(Price<T> &_product)
	{
		PriceStream<T> pricestream = algostream->ConvertToPriceStream(_product);
		AlgoPriceStream<T> algopricestream(pricestream);
	}

	virtual void ProcessRemove(Price<T> &data) {}
	virtual void ProcessUpdate(Price<T> &data) {}

private:
	AlgoStreamingService<T>* algostream;
	PricingServiceListener<T>()
	{
		algostream = AlgoStreamingService<T>::Generate_Instance();
	}
};

#endif