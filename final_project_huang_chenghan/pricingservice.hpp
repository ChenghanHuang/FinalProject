/**
 * pricingservice.hpp
 * Defines the data types and Service for internal prices.
 *
 * @author Breman Thuraisingham
 */
#ifndef PRICING_SERVICE_HPP
#define PRICING_SERVICE_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "algostreamingservice.hpp"
#include "soa.hpp"
#include "products.hpp"

using namespace std;

template<typename T>
class AlgoStreamingService;

/**
* A price object consisting of mid and bid/offer spread.
* Type T is the product type.
*/
template<typename T>
class Price
{

public:

	Price() :product(T()) {}

	// ctor for a price
	Price(const T &_product, double _mid, double _bidOfferSpread);

	// Get the product
	const T& GetProduct() const;

	// Get the mid price
	double GetMid() const;

	// Get the bid/offer spread around the mid
	double GetBidOfferSpread() const;

private:
	const T& product;
	double mid;
	double bidOfferSpread;

};

/**
* Pricing Service managing mid prices and bid/offers.
* Keyed on product identifier.
* Type T is the product type.
*/
template<typename T>
class PricingService : public Service<string, Price <T> >
{
public:
	map<string, Price<T>> PriceMap;
	vector<ServiceListener<Price<T>>*> ListenerList;

	static PricingService<T>* Generate_Instance()
	{
		static PricingService<T> instance;
		return &instance;
	}

	void BookPrice(Price<T> &data)
	{
		T BondProduct = data.GetProduct();
		PriceMap.insert(pair<string, Price<T>>(BondProduct.GetProductId(), data));
	}

	virtual Price<T>& GetData(string id)
	{
		return PriceMap[id];
	}

	virtual void OnMessage(Price<T> &data)
	{
		BookPrice(data);
		for (int i = 0; i < ListenerList.size();i++)
		{
			ServiceListener<Price<T>>* listener = ListenerList[i];
			listener->ProcessAdd(data);
		}
	}

	virtual void AddListener(ServiceListener<Price<T>>* _listener)
	{
		ListenerList.push_back(_listener);
	}

	virtual const vector<ServiceListener<Price<T>>*>& GetListeners() const
	{
		return ListenerList;
	}
};



class PricingServiceConnector : public Connector<Price<Bond>> {
public:
	static PricingServiceConnector* Generate_Instance()
	{
		static PricingServiceConnector instance;
		return &instance;
	}

	void Publish(Price<Bond> &data) {}

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
			istringstream iss;
			iss.str(str.substr(0, idx));
			double result = 0;
			iss >> result;
			iss.str(str.substr(idx + 1, 2));
			int num1=0;
			iss >> num1;
			char ch = str[str.size() - 1];
			if (ch == '+') ch = '4';
			int num2 = ch - '0';
			result += (num1 * 8 + num2) / 256.0;
			return result;
		};

		ifstream is("input/prices.txt");
		string line;
		std::getline(is, line); 	// skip the header
		string _cusip, _mid, _bidofferspread;
		while (std::getline(is, line)) {
			std::vector<std::string> elems = SplitLine(line);
			_cusip = elems[0]; _mid = elems[1]; _bidofferspread = elems[2];
			double mid_price = String2Price(_mid);
			double spread = String2Price(_bidofferspread);
			auto bond = _bondProductService->GetData(_cusip);
			Price<Bond> price(bond, mid_price, spread);
			_bondPricingService->OnMessage(price);
		}
		std::cout << "streaming.txt Generated." << std::endl;
	}

	PricingService<Bond>* GetService()
	{
		return _bondPricingService;
	}

private:
	PricingServiceConnector()
	{
		_bondPricingService = PricingService<Bond>::Generate_Instance();
		_bondProductService = BondProductService::Generate_Instance();
	}

	PricingService<Bond> *_bondPricingService;;
	BondProductService* _bondProductService;
};

//template<typename T>
//class PricingConnector : public Connector<Price <T> >
//{
//	//Read data from file
//	void ReadPriceData()
//	{
//		//Read data
//
//		//Call OnMessage
//
//	}
//};

template<typename T>
Price<T>::Price(const T &_product, double _mid, double _bidOfferSpread) :
	product(_product)
{
	mid = _mid;
	bidOfferSpread = _bidOfferSpread;
}

template<typename T>
const T& Price<T>::GetProduct() const
{
	return product;
}

template<typename T>
double Price<T>::GetMid() const
{
	return mid;
}

template<typename T>
double Price<T>::GetBidOfferSpread() const
{
	return bidOfferSpread;
}

#endif
