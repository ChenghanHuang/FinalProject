/**
 * positionservice.hpp
 * Defines the data types and Service for positions.
 *
 * @author Breman Thuraisingham
 */
#ifndef POSITION_SERVICE_HPP
#define POSITION_SERVICE_HPP

#include <string>
#include <map>
#include <iostream>
#include "soa.hpp"
#include "tradebookingservice.hpp"
#include "pricingservice.hpp"
//#include "products.hpp"



/**
 * Position class in a particular book.
 * Type T is the product type.
 */
template<typename T>
class Position
{

public:

	Position(){}

  // ctor for a position
  Position(const T &_product);

  void AddPosition(const Trade<T> &_trade)
  {
	  string id = _trade.GetBook();
	  if (positions.find(id) == positions.end())
	  {
		  positions.insert(pair<string, long>(id, _trade.GetQuantity()));
	  }
	  else
	  {
		  positions[id] += _trade.GetQuantity();
	  }
  }

  // Get the product
  const T& GetProduct() const;

  // Get the position quantity
  long GetPosition(string &book);

  // Get the aggregate position
  long GetAggregatePosition();

private:
  T product;
  map<string,long> positions;

};

/**
 * Position Service to manage positions across multiple books and secruties.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PositionService : public Service<string,Position <T> >
{

public:
	map<string, Position<T>> PositionMap;
	vector<ServiceListener<Position<T>>*> ListenerList;

	static PositionService<T>* Generate_Instance()
	{
		static PositionService<T> instance;
		return &instance;
	}

    // Add a trade to the service
	virtual void AddTrade(const Trade<T> &trade)
	{
		T product = trade.GetProduct();
		string id = product.GetProductId();
		if (PositionMap.find(id) == PositionMap.end())
		{
			Position<T> position(product);
			position.AddPosition(trade);
			PositionMap.insert(pair<string, Position<T>>(id, position));
			PushToListeners(position);
		}
		else
		{
			Position<T> position = PositionMap[id];
			position.AddPosition(trade);
			PushToListeners(position);
		}
	}

	void PushToListeners(Position<T> &position)
	{
		for (int i = 0; i < ListenerList.size(); i++)
		{
			ServiceListener<Position<T>>* listener = ListenerList[i];
			listener->ProcessAdd(position);
		}
	}

	void AddPosition(const Position<T> &position)
	{
		T product = position.GetProduct();
		string id = product.GetProductId();
		PositionMap.insert(pair<string, Position<T>>(id, position));
	}

	virtual Position<T>& GetData(string id)
	{
		return PositionMap[id];
	}

	virtual void OnMessage(Position<T> &data) {}

	virtual void AddListener(ServiceListener<Position<T>>* _listener)
	{
		ListenerList.push_back(_listener);
	}

	virtual const vector< ServiceListener<Position<T>>*>& GetListeners() const
	{
		return ListenerList;
	}
};

template<typename T>
Position<T>::Position(const T &_product) :
  product(_product)
{
	positions.insert(pair<string, long>("TRSY1", 0));
	positions.insert(pair<string, long>("TRSY2", 0));
	positions.insert(pair<string, long>("TRSY3", 0));
}

template<typename T>
const T& Position<T>::GetProduct() const
{
  return product;
}

template<typename T>
long Position<T>::GetPosition(string &book)
{
  return positions[book];
}

template<typename T>
long Position<T>::GetAggregatePosition()
{
	long sum = 0;
	for (auto it = positions.begin(); it != positions.end(); it++)
	{
		sum += it->second;
	}
	return sum;
}

#endif
