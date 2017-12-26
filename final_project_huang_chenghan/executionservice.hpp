/**
 * executionservice.hpp
 * Defines the data types and Service for executions.
 *
 * @author Breman Thuraisingham
 */
#ifndef EXECUTION_SERVICE_HPP
#define EXECUTION_SERVICE_HPP

#include <string>
#include <map>
#include "soa.hpp"
#include "marketdataservice.hpp"
#include "tradebookingservice.hpp"
 //#include "products.hpp"

enum OrderType { FOK, IOC, MARKET, LIMIT, STOP };

enum Market { BROKERTEC, ESPEED, CME };

/**
* An execution order that can be placed on an exchange.
* Type T is the product type.
*/
template<typename T>
class ExecutionOrder
{

public:

	ExecutionOrder() {}

	// ctor for an order
	ExecutionOrder(const T &_product, PricingSide _side, string _orderId, OrderType _orderType, double _price, double _visibleQuantity, double _hiddenQuantity, string _parentOrderId, bool _isChildOrder);

	// Get the product
	const T& GetProduct() const;

	// Get the order ID
	const string& GetOrderId() const;

	// Get the order type on this order
	OrderType GetOrderType() const;

	// Get the price on this order
	double GetPrice() const;

	// Get the visible quantity on this order
	long GetVisibleQuantity() const;

	PricingSide GetSide() const
	{
		return side;
	}

	// Get the hidden quantity
	long GetHiddenQuantity() const;

	// Get the parent order ID
	const string& GetParentOrderId() const;

	// Is child order?
	bool IsChildOrder() const;

	friend ostream& operator << (ostream& os, const ExecutionOrder& t)
	{
		string ot;
		switch (t.orderType) {
		case FOK: ot = "FOK"; break;
		case MARKET: ot = "MARKET"; break;
		case LIMIT: ot = "LIMIT"; break;
		case STOP: ot = "STOP"; break;
		case IOC: ot = "IOC"; break;
		default: ot = "OTHER";
		}
		os << "Product: " << t.GetProduct() << endl;
		os << "  pricingSide: " << (t.side == BID ? "BID" : "OFFER") << endl;
		os << "  orderID: " << t.GetOrderId() << endl;
		os << "  orderType: " << ot << endl;
		os << "  price: " << t.GetPrice() << endl;
		os << "  visibleQuantity: " << t.GetVisibleQuantity() << endl;
		os << "  hiddenQuantity: " << t.GetHiddenQuantity() << endl;
		os << "  parentOrderId: " << t.GetParentOrderId() << endl;
		os << "  isChildOrder: " << std::boolalpha << t.IsChildOrder() << endl;
		return os;
	}

private:
	T product;
	PricingSide side;
	string orderId;
	OrderType orderType;
	double price;
	double visibleQuantity;
	double hiddenQuantity;
	string parentOrderId;
	bool isChildOrder;

};

/**
* Service for executing orders on an exchange.
* Keyed on product identifier.
* Type T is the product type.
*/
template<typename T>
class ExecutionService : public Service<string, ExecutionOrder <T> >
{

public:
	map<string, ExecutionOrder<T>> OrderMap;
	vector<ServiceListener<ExecutionOrder<T>>*> ListenerList;
	vector<string> booklist{ "TRSY1", "TRSY2", "TRSY3" };
	int tradeID, bookID;

	ExecutionService()
	{
		bookID = 0;
		tradeID = 0;
	}

	static ExecutionService<T>* Generate_Instance()
	{
		static ExecutionService<T> instance;
		return &instance;
	}

	// Execute an order on a market
	void ExecuteOrder(const ExecutionOrder<T>& _order, Market _market)
	{
		T product = _order.GetProduct();
		double price = _order.GetPrice();
		Side side = (_order.GetSide() == BID ? BUY : SELL);
		ExecutionOrder<T> order = _order;

		/*char[10] id_char;
		sprintf(id_char, "%d", tradeID++);
		std::string id = String.valueOf(id_char);*/

		stringstream ss;
		ss << tradeID;
		tradeID++;
		string id = ss.str();

		Trade<T> trade(product, id, price, booklist[bookID], _order.GetVisibleQuantity(), side);
		bookID = (bookID + 1) % 3;

		for (int i = 0; i < ListenerList.size(); i++)
		{
			ServiceListener<ExecutionOrder<T>>* listener = ListenerList[i];
			listener->ProcessAdd(order);
		}
	}

	void AddOrder(ExecutionOrder<T> &_order)
	{
		T BondProduct = _order.GetProduct();
		OrderMap.insert(pair<string, ExecutionOrder<T>>(BondProduct.GetProductId(), _order));
	}

	virtual void OnMessage(ExecutionOrder<T> &_order) {}

	virtual ExecutionOrder<T>& GetData(string _id)
	{
		return OrderMap[_id];
	}

	virtual void AddListener(ServiceListener<ExecutionOrder<T>>* _listener)
	{
		ListenerList.push_back(_listener);
	}

	virtual const vector<ServiceListener<ExecutionOrder<T>>* >& GetListeners() const
	{
		return ListenerList;
	}
};

//template<typename T>
//class ExecutionServiceListener : public ServiceListener<ExecutionOrder<T>>
//{
//public:
//	static ExecutionServiceListener<T>* Generate_Instance()
//	{
//		static ExecutionServiceListener<T> instance;
//		return &instance;
//	}
//
//	virtual void ProcessAdd(Trade<T> &_trade)
//	{
//		tradebookingservice->BookTrade(_trade);
//	}
//
//	ExecutionService<T>* GetService()
//	{
//		return _bondExecutionService;
//	}
//
//	virtual void ProcessRemove(Trade<T> &data) {}
//
//	virtual void ProcessUpdate(Trade<T> &data) {}
//
//private:
//	TradeBookingService<T>* tradebookingservice;
//	ExecutionService<T>* _bondExecutionService;
//	ExecutionServiceListener()
//	{
//		_bondExecutionService = ExecutionService<T>::Generate_Instance();
//	}
//};

template<typename T>
ExecutionOrder<T>::ExecutionOrder(const T &_product, PricingSide _side, string _orderId, OrderType _orderType, double _price, double _visibleQuantity, double _hiddenQuantity, string _parentOrderId, bool _isChildOrder) :
	product(_product)
{
	side = _side;
	orderId = _orderId;
	orderType = _orderType;
	price = _price;
	visibleQuantity = _visibleQuantity;
	hiddenQuantity = _hiddenQuantity;
	parentOrderId = _parentOrderId;
	isChildOrder = _isChildOrder;
}

template<typename T>
const T& ExecutionOrder<T>::GetProduct() const
{
	return product;
}

template<typename T>
const string& ExecutionOrder<T>::GetOrderId() const
{
	return orderId;
}

template<typename T>
OrderType ExecutionOrder<T>::GetOrderType() const
{
	return orderType;
}

template<typename T>
double ExecutionOrder<T>::GetPrice() const
{
	return price;
}

template<typename T>
long ExecutionOrder<T>::GetVisibleQuantity() const
{
	return visibleQuantity;
}

template<typename T>
long ExecutionOrder<T>::GetHiddenQuantity() const
{
	return hiddenQuantity;
}

template<typename T>
const string& ExecutionOrder<T>::GetParentOrderId() const
{
	return parentOrderId;
}

template<typename T>
bool ExecutionOrder<T>::IsChildOrder() const
{
	return isChildOrder;
}
#endif
