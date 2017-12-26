/**
 * riskservice.hpp
 * Defines the data types and Service for fixed income risk.
 *
 * @author Breman Thuraisingham
 */
#ifndef RISK_SERVICE_HPP
#define RISK_SERVICE_HPP

#include <vector>
#include <iostream>
#include "soa.hpp"
#include "positionservice.hpp"
//#include "products.hpp"

template <typename T>
class Position;

/**
 * PV01 risk.
 * Type T is the product type.
 */
template<typename T>
class PV01
{

public:

	PV01(){}

  // ctor for a PV01 value
  PV01(const T &_product, double _pv01, long _quantity);

  // Get the product on this PV01 value
  const T& GetProduct() const;

  // Get the PV01 value
  double GetPV01() const;

  // Get the quantity that this risk value is associated with
  long GetQuantity() const;

private:
  T product;
  double pv01;
  long quantity;

};

/**
 * A bucket sector to bucket a group of securities.
 * We can then aggregate bucketed risk to this bucket.
 * Type T is the product type.
 */
template<typename T>
class BucketedSector
{

public:

  // ctor for a bucket sector
  BucketedSector(const vector<T> &_products, string _name);

  // Get the products associated with this bucket
  const vector<T>& GetProducts() const;

  // Get the name of the bucket
  const string& GetName() const;

private:
  vector<T> products;
  string name;

};

/**
 * Risk Service to vend out risk for a particular security and across a risk bucketed sector.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class RiskService : public Service<string,PV01<T> >
{

public:
	map<string, PV01<T>> RiskMap;
	vector<ServiceListener<PV01<T>>*> ListenerList;

	// Add a position that the service will risk
	double GetPV01(const T &_product)
	{
		return 0.0001;
	}

	static RiskService<T>* Generate_Instance()
	{
		static RiskService<T> instance;
		return &instance;
	}

	void AddPosition(Position<T> &position)
	{
		const T product = position.GetProduct();
		string id = product.GetProductId();
		long pos = position.GetAggregatePosition();
		PV01<T> pv01(product, GetPV01(product), pos);
		if (RiskMap.find(id) == RiskMap.end())
		{
			RiskMap.insert(pair<string, PV01<T>>(id, pv01));
		}
		else
		{
			RiskMap[id] = pv01;
		}

		for (int i = 0; i < ListenerList.size(); i++)
		{
			ServiceListener<PV01<T>>* listener = ListenerList[i];
			listener->ProcessAdd(pv01);
		}
	}

	void Add(PV01<T> pv01)
	{
		T product = pv01.GetProduct();
		string id = product.GetProductId();
		RiskMap.insert(pair<string, PV01<T>>(id, pv01));
	}

	// Get the bucketed risk for the bucket sector
	const PV01< BucketedSector<T> >& GetBucketedRisk(const BucketedSector<T> &sector)
	{
		vector<T> products = sector.GetProducts();
		double pv01_total = 0;
		for (int i = 0; i < products.size(); i++)
		{
			string id = products[i].GetProductId();
			pv01_total += RiskMap[id].GetPV01() * RiskMap[id].GetQuantity();
		}
	}

	virtual PV01<T>& GetData(string _id)
	{
		return RiskMap[_id];
	}

	virtual void OnMessage(PV01<T> &data) {}

	virtual void AddListener(ServiceListener<PV01<T>>* _listener)
	{
		ListenerList.push_back(_listener);
	}

	virtual const vector< ServiceListener<PV01<T>>* >& GetListeners() const
	{
		return ListenerList;
	}

};

template<typename T>
PV01<T>::PV01(const T &_product, double _pv01, long _quantity) :
  product(_product)
{
  pv01 = _pv01;
  quantity = _quantity;
}

template<typename T>
const T& PV01<T>::GetProduct() const
{
	return product;
}

template<typename T>
double PV01<T>::GetPV01() const
{
	return pv01;
}

template<typename T>
long PV01<T>::GetQuantity() const
{
	return quantity;
}

template<typename T>
BucketedSector<T>::BucketedSector(const vector<T>& _products, string _name) :
  products(_products)
{
  name = _name;
}

template<typename T>
const vector<T>& BucketedSector<T>::GetProducts() const
{
  return products;
}

template<typename T>
const string& BucketedSector<T>::GetName() const
{
  return name;
}

#endif
