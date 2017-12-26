/**
 * marketdataservice.hpp
 * Defines the data types and Service for order book market data.
 *
 * @author Breman Thuraisingham
 */
#ifndef MARKET_DATA_SERVICE_HPP
#define MARKET_DATA_SERVICE_HPP


#include <string>
#include <vector>
#include <iostream>
#include "soa.hpp"
#include "products.hpp"

using namespace std;

// Side for market data
enum PricingSide { BID, OFFER };

/**
* A market data order with price, quantity, and side.
*/
class Order
{

public:

	Order() {}

	// ctor for an order
	Order(double _price, long _quantity, PricingSide _side)
	{
		price = _price;
		quantity = _quantity;
		side = _side;
	}

	// Get the price on the order
	double GetPrice() const
	{
		return price;
	}

	// Get the quantity on the order
	long GetQuantity() const
	{
		return quantity;
	}

	// Get the side on the order
	PricingSide GetSide() const
	{
		return side;
	}

private:
	double price;
	long quantity;
	PricingSide side;

};

/**
* Class representing a bid and offer order
*/
class BidOffer
{

public:

	BidOffer() {}

	// ctor for bid/offer
	BidOffer(const Order &_bidOrder, const Order &_offerOrder);

	// Get the bid order
	const Order& GetBidOrder() const;

	// Get the offer order
	const Order& GetOfferOrder() const;

private:
	Order bidOrder;
	Order offerOrder;

};

/**
* Order book with a bid and offer stack.
* Type T is the product type.
*/
template<typename T>
class OrderBook
{

public:

	OrderBook() {}

	// ctor for the order book
	OrderBook(const T &_product, const vector<Order> &_bidStack, const vector<Order> &_offerStack);

	// Get the product
	const T& GetProduct() const;

	// Get the bid stack
	const vector<Order>& GetBidStack() const;

	// Get the offer stack
	const vector<Order>& GetOfferStack() const;

	void AddBidStack(const vector<Order> &_bidStack)
	{
		bidStack.insert(bidStack.end(), _bidStack.begin(), _bidStack.end());
	}

	void AddOfferStack(const vector<Order> &_offerStack)
	{
		offerStack.insert(offerStack.end(), _offerStack.begin(), _offerStack.end());
	}

private:
	T product;
	vector<Order> bidStack;
	vector<Order> offerStack;

};

/**
* Market Data Service which distributes market data
* Keyed on product identifier.
* Type T is the product type.
*/
template<typename T>
class MarketDataService : public Service<string, OrderBook<T>>
{
public:
	map<string, OrderBook<T>> MarketDataMap;
	vector<ServiceListener<OrderBook<T>>*> ListenerList;

	MarketDataService() {}

	static MarketDataService<T>* Generate_Instance()
	{
		static MarketDataService<T> instance;
		return &instance;
	}

	void AddMarketData(const OrderBook<T> &orderbook)
	{
		T product = orderbook.GetProduct();
		string id = product.GetProductId();
		if (MarketDataMap.find(id) == MarketDataMap.end())
		{
			MarketDataMap.insert(pair<string, OrderBook<T>>(id, orderbook));
		}
		else
		{
			MarketDataMap[id].AddBidStack(orderbook.GetBidStack());
			MarketDataMap[id].AddOfferStack(orderbook.GetOfferStack());
		}

		for (int i = 0; i < ListenerList.size(); i++)
		{
			ServiceListener<OrderBook<T>>* listener = ListenerList[i];
			listener->ProcessAdd(orderbook);
		}
	}

	// Get the best bid/offer order
	const BidOffer& GetBestBidOffer(OrderBook<T> orderbook)
	{
		vector<Order> bidStack = orderbook.GetBidStack();
		vector<Order> offerStack = orderbook.GetOfferStack();
		double bestbidprice = bidStack[0].GetPrice();
		double bestofferprice = offerStack[0].GetPrice();
		Order bestbid, bestoffer;
		for (int i = 0; i < bidStack.size(); i++)
		{
			if (bidStack[i].GetPrice() >= bestbidprice)
			{
				bestbidprice = bidStack[i].GetPrice();
				bestbid = bidStack[i];
			}
		}
		for (int i = 0; i < offerStack.size(); i++)
		{
			if (offerStack[i].GetPrice() <= bestofferprice)
			{
				bestofferprice = offerStack[i].GetPrice();
				bestoffer = offerStack[i];
			}
		}
		BidOffer* bestbidoffer = new BidOffer(bestbid, bestoffer);
		return *bestbidoffer;
	}

	// Aggregate the order book
	OrderBook<T>& GetData(string productId) override
	{
		return MarketDataMap[productId];
	}

	virtual void OnMessage(OrderBook<T> &orderbook) override
	{
		for (int i = 0; i < ListenerList.size(); i++)
		{
			ServiceListener<OrderBook<T>>* listener = ListenerList[i];
			listener->ProcessAdd(orderbook);
		}
	}

	virtual void AddListener(ServiceListener<OrderBook<T>>* _listener) override
	{
		ListenerList.push_back(_listener);
	}

	virtual const vector< ServiceListener<OrderBook<T>>* >& GetListeners() const override
	{
		return ListenerList;
	}
};

template<typename T>
class MarketDataConnector :public Connector<OrderBook<T>>
{
public:
	static MarketDataConnector<T>* Generate_Instance()
	{
		static MarketDataConnector<T> instance;
		return &instance;
	}

	void Publish(OrderBook<T> &data) {}

	void Subscribe()
	{
		auto SplitLine = [](std::string& line)
		{
			stringstream enter_line(line);
			std::string item;
			std::vector<std::string> tmp;
			while (getline(enter_line, item, ',')) 	tmp.push_back(item);
			return tmp;
		};
		auto String2Price = [](std::string& str)
		{
			size_t idx = str.find_first_of('-');
			double result = std::stoi(str.substr(0, idx));
			int num1 = std::stoi(str.substr(idx + 1, 2));
			char ch = str[str.size() - 1];
			if (ch == '+') ch = '4';
			int num2 = ch - '0';
			result += (num1 * 8 + num2) / 256.0;
			return result;
		};

		ifstream is("input/marketdata.txt");
		string line, _cusip;
		// skip the header
		getline(is, line);
		for (int i = 0; i < 12; ++i)
		{
			getline(is, line);
			std::vector<std::string> elems = SplitLine(line);
			_cusip = elems[0];
			PricingSide side;
			vector<Order> bid_stack, offer_stack;
			double price;	long quantity;
			int idx = 1;
			for (int k = 1; k <= 5; ++k)
			{
				price = String2Price(elems[idx++]);
				quantity = std::stol(elems[idx++]);
				side = BID;
				Order bid_order(price, quantity, side);
				bid_stack.push_back(bid_order);
			}
			for (int k = 1; k <= 5; ++k)
			{
				price = String2Price(elems[idx++]);
				quantity = std::stol(elems[idx++]);
				side = OFFER;
				Order offer_order(price, quantity, side);
				offer_stack.push_back(offer_order);
			}

			auto bond = _bondProductService->GetData(_cusip);
			OrderBook<Bond> order_book(bond, bid_stack, offer_stack);
			_bondMarketDataService->OnMessage(order_book);
		}
		std::cout << "executions.txt Generated." << std::endl;
	}


	MarketDataService<T>* GetService()
	{
		return _bondMarketDataService;
	}

private:
	MarketDataService<T>* _bondMarketDataService;
	BondProductService* _bondProductService;
	MarketDataConnector()
	{
		_bondMarketDataService = MarketDataService<T>::Generate_Instance();
		_bondProductService = BondProductService::Generate_Instance();
	}
};

BidOffer::BidOffer(const Order &_bidOrder, const Order &_offerOrder) :
	bidOrder(_bidOrder), offerOrder(_offerOrder)
{
}

const Order& BidOffer::GetBidOrder() const
{
	return bidOrder;
}

const Order& BidOffer::GetOfferOrder() const
{
	return offerOrder;
}

template<typename T>
OrderBook<T>::OrderBook(const T &_product, const vector<Order> &_bidStack, const vector<Order> &_offerStack) :
	product(_product), bidStack(_bidStack), offerStack(_offerStack)
{
}

template<typename T>
const T& OrderBook<T>::GetProduct() const
{
	return product;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetBidStack() const
{
	return bidStack;
}

template<typename T>
const vector<Order>& OrderBook<T>::GetOfferStack() const
{
	return offerStack;
}

//template<typename T>
//void vector<Order>& OrderBook<T>::AddBidStack(const vector<Order> &_bidStack)
//{
//	bidStack.insert(bidStack.end(), _bidStack.begin(), _bidStack.end());
//}



#endif
