/**
 * tradebookingservice.hpp
 * Defines the data types and Service for trade booking.
 *
 * @author Breman Thuraisingham
 */
#ifndef TRADE_BOOKING_SERVICE_HPP
#define TRADE_BOOKING_SERVICE_HPP



#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "soa.hpp"
#include "products.hpp"

 // Trade sides
enum Side { BUY, SELL };

using namespace std;

/**
* Trade object with a price, side, and quantity on a particular book.
* Type T is the product type.
*/

template<typename T>
class PositionService;

template<typename T>
class Trade
{

public:

	Trade() {}

	// ctor for a trade
	Trade(const T &_product, string _tradeId, double _price, string _book, long _quantity, Side _side);

	// Get the product
	const T& GetProduct() const;

	// Get the trade ID
	const string& GetTradeId() const;

	// Get the mid price
	double GetPrice() const;

	// Get the book
	const string& GetBook() const;

	// Get the quantity
	long GetQuantity() const;

	// Get the side
	Side GetSide() const;

	friend ostream& operator << (ostream& os, const Trade<T>& t)
	{
		os << "Trade ID: " << t.GetTradeId() << endl;
		os << "  Product: " << t.GetProduct() << endl;
		os << "  Book: " << t.GetBook() << endl;
		os << "  Price: " << t.GetPrice() << endl;
		os << "  Quantity: " << t.GetQuantity() << endl;
		if (t.GetSide() == BUY)
			os << "  Side: BUY" << endl;
		else os << "  Side: SELL" << endl;
		return os;
	}

private:
	T product;
	string tradeId;
	double price;
	string book;
	long quantity;
	Side side;

};

/**
* Trade Booking Service to book trades to a particular book.
* Keyed on trade id.
* Type T is the product type.
*/
template<typename T>
class TradeBookingService : public Service<string, Trade <T> >
{
public:
	Trade<T> trade;
	map<string, Trade<T>> BookMap;
	vector<ServiceListener<Trade<T>>*> ListenerList;

	TradeBookingService() {}

	static TradeBookingService<T>* Generate_Instance()
	{
		static TradeBookingService<T> instance;
		return &instance;
	}

	// Book the trade
	void BookTrade(const Trade<T> &data)
	{
		string id = data.GetTradeId();
		BookMap.insert(pair<string, Trade<T>>(id, data));
	}

	virtual Trade<T>& GetData(string id)
	{
		return BookMap[id];
	}

	virtual void OnMessage(Trade<T> &data)
	{
		BookTrade(data);
		for (int i = 0; i < ListenerList.size(); i++)
		{
			ServiceListener<Trade<T>>* listener = ListenerList[i];
			listener->ProcessAdd(data);
		}
	}

	virtual void AddListener(ServiceListener<Trade<T>>* _listener)
	{
		ListenerList.push_back(_listener);
	}

	virtual const vector< ServiceListener<Trade<T>>*>& GetListeners() const
	{
		return ListenerList;
	}
};

template<typename T>
class TradeBookingServiceListener : public ServiceListener<Trade<T>>
{
public:
	static TradeBookingServiceListener<T>* Generate_Instance()
	{
		static TradeBookingServiceListener<T> instance;
		return &instance;
	}

	void ProcessAdd(Trade<T> &_trade)
	{
		_bondPositionService->AddTrade(_trade);
	}

	void ProcessRemove(Trade<T> &data) {}
	void ProcessUpdate(Trade<T> &data) {}


private:
	PositionService<T>* _bondPositionService;

	TradeBookingServiceListener()
	{
		_bondPositionService = PositionService<T>::Generate_Instance();
	}

};

class TradeBookingConnector : public Connector<Trade<Bond>>
{
public:
	static TradeBookingConnector* Generate_Instance()
	{
		static TradeBookingConnector instance;
		return &instance;
	}

	void Publish(Trade<Bond> &data) {}

	// reading from file and call service's OnMessage
	void Subscribe() {
		/* some functions*/
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

		ifstream is("input/trades.txt");
		string line;
		string _cusip, _tradeId, _book, _price, _quantity, _side;
		getline(is, line); // skip the header
		while (std::getline(is, line))
		{
			std::vector<std::string> elems = SplitLine(line);
			_cusip = elems[0]; _tradeId = elems[1]; _book = elems[2];
			_price = elems[3]; _quantity = elems[4]; _side = elems[5];
			Bond bond = _bondProductService->GetData(_cusip);
			Trade<Bond> trade(bond, _tradeId, String2Price(_price), _book, std::stol(_quantity), (_side == "BUY" ? BUY : SELL));
			_bondTradeBookingservice->OnMessage(trade);
		}
		std::cout << "risk.txt Generated." << std::endl;
	}

	TradeBookingService<Bond>* GetService()
	{
		return _bondTradeBookingservice;
	}

private:

	TradeBookingConnector()
	{
		_bondTradeBookingservice = TradeBookingService<Bond>::Generate_Instance();
		_bondProductService = BondProductService::Generate_Instance();
	}
	TradeBookingService<Bond>* _bondTradeBookingservice;
	BondProductService * _bondProductService;
};

template<typename T>
Trade<T>::Trade(const T &_product, string _tradeId, double _price, string _book, long _quantity, Side _side) :
	product(_product)
{
	tradeId = _tradeId;
	price = _price;
	book = _book;
	quantity = _quantity;
	side = _side;
}

template<typename T>
const T& Trade<T>::GetProduct() const
{
	return product;
}

template<typename T>
const string& Trade<T>::GetTradeId() const
{
	return tradeId;
}

template<typename T>
double Trade<T>::GetPrice() const
{
	return price;
}

template<typename T>
const string& Trade<T>::GetBook() const
{
	return book;
}

template<typename T>
long Trade<T>::GetQuantity() const
{
	return quantity;
}

template<typename T>
Side Trade<T>::GetSide() const
{
	return side;
}

#endif
