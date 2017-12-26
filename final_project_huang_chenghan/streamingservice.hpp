/**
 * streamingservice.hpp
 * Defines the data types and Service for price streams.
 *
 * @author Breman Thuraisingham
 */
#ifndef STREAMING_SERVICE_HPP
#define STREAMING_SERVICE_HPP

#include <iostream>
#include "soa.hpp"
#include "marketdataservice.hpp"
#include "pricingservice.hpp"
#include "riskservice.hpp"
//#include "products.hpp"
//#include "historicaldataservice.hpp"

using namespace std;

/**
 * A price stream order with price and quantity (visible and hidden)
 */
class PriceStreamOrder
{

public:

	PriceStreamOrder() {}

  // ctor for an order
	PriceStreamOrder(double _price, long _visibleQuantity, long _hiddenQuantity, PricingSide _side)
	{
		price = _price;
		visibleQuantity = _visibleQuantity;
		hiddenQuantity = _hiddenQuantity;
		side = _side;
	}

  // The side on this order
	PricingSide GetSide() const
	{
		return side;
	}

  // Get the price on this order
	double GetPrice() const
	{
		return price;
	}

  // Get the visible quantity on this order
	long GetVisibleQuantity() const
	{
		return visibleQuantity;
	}

  // Get the hidden quantity on this order
  long GetHiddenQuantity() const
  {
	  return hiddenQuantity;
  }

private:
  double price;
  long visibleQuantity;
  long hiddenQuantity;
  PricingSide side;

};

/**
 * Price Stream with a two-way market.
 * Type T is the product type.
 */
template<typename T>
class PriceStream
{

public:

  PriceStream() {}

  // ctor
  PriceStream(const T &_product, const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder);

  // Get the product
  const T& GetProduct() const;

  // Get the bid order
  const PriceStreamOrder& GetBidOrder() const;

  // Get the offer order
  const PriceStreamOrder& GetOfferOrder() const;

private:
  T product;
  PriceStreamOrder bidOrder;
  PriceStreamOrder offerOrder;

};

/**
 * Streaming service to publish two-way prices.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class StreamingService : public Service<string,PriceStream<T>>
{
public:
	map<string, PriceStream<T>> StreamMap;
	vector<ServiceListener<PriceStream<T>>*> ListenerList;

	static StreamingService* Generate_Instance() 
	{
		static StreamingService instance;
		return &instance;
	}

	void PublishPrice(PriceStream<T>& priceStream)
	{
		T product = priceStream.GetProduct();
		string product_ID = product.GetProductId();

		auto it = StreamMap.find(product_ID);
		if (it == StreamMap.end()) {
			AddStream(priceStream);
		}

		for (int i = 0; i < ListenerList.size(); i++)
		{
			ServiceListener<PriceStream<T>>* listener = ListenerList[i];
			listener->ProcessAdd(priceStream);
		}

	}

	void AddStream(PriceStream<T> &_stream)
	{
		T BondProduct = _stream.GetProduct();
		StreamMap.insert(pair<string, PriceStream<T>>(BondProduct.GetProductId(), _stream));
	}

	virtual PriceStream<T>& GetData(string _id)
	{
		return StreamMap[_id];
	}

	virtual void OnMessage(PriceStream<T> &data) {}

	virtual void AddListener(ServiceListener<PriceStream<T>>* _listener)
	{
		ListenerList.push_back(_listener);
	}

	virtual const vector<ServiceListener<PriceStream<T>>* >& GetListeners() const
	{
		return ListenerList;
	}
};



//long PriceStreamOrder::GetHiddenQuantity() const
//{
//  return hiddenQuantity;
//}

template<typename T>
PriceStream<T>::PriceStream(const T &_product, const PriceStreamOrder &_bidOrder, const PriceStreamOrder &_offerOrder) :
  product(_product), bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}

template<typename T>
const T& PriceStream<T>::GetProduct() const
{
  return product;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetBidOrder() const
{
  return bidOrder;
}

template<typename T>
const PriceStreamOrder& PriceStream<T>::GetOfferOrder() const
{
  return offerOrder;
}

#endif
