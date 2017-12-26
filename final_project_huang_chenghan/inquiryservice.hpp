/**
 * inquiryservice.hpp
 * Defines the data types and Service for customer inquiries.
 *
 * @author Breman Thuraisingham
 */
#ifndef INQUIRY_SERVICE_HPP
#define INQUIRY_SERVICE_HPP

#include "soa.hpp"
#include "tradebookingservice.hpp"
#include "products.hpp"
#include <vector>
#include <map>

using namespace std;

// Various inqyury states
enum InquiryState { RECEIVED, QUOTED, DONE, REJECTED, CUSTOMER_REJECTED };

template<typename T>
class InquiryConnector;

/**
 * Inquiry object modeling a customer inquiry from a client.
 * Type T is the product type.
 */
template<typename T>
class Inquiry
{

public:

	Inquiry() = default;

  // ctor for an inquiry
  Inquiry(string _inquiryId, const T &_product, Side _side, long _quantity, double _price, InquiryState _state);

  // Get the inquiry ID
  const string& GetInquiryId() const;

  // Get the product
  const T& GetProduct() const;

  // Get the side on the inquiry
  Side GetSide() const;

	void Set(double _price, InquiryState _state)
	{
		price = _price;
		state = _state;
	}

  // Get the quantity that the client is inquiring for
  long GetQuantity() const;

  // Get the price that we have responded back with
  double GetPrice() const;

  // Get the current state on the inquiry
  InquiryState GetState() const;

  void SetState(InquiryState _state);

private:
  string inquiryId;
  T product;
  Side side;
  long quantity;
  double price;
  InquiryState state;

};

/**
 * Service for customer inquirry objects.
 * Keyed on inquiry identifier (NOTE: this is NOT a product identifier since each inquiry must be unique).
 * Type T is the product type.
 */
//template<typename T>
//class InquiryService : public Service<string,Inquiry <T> >
//{

//public:
//	map<string, Inquiry<T>> InquiryMap;
//	vector<ServiceListener<Inquiry<T>>*> ListenerList;
	

/*	static InquiryService* Generate_Instance()
	{
		static InquiryService instance;
		return &instance;
	}

	void AddInquiry(Inquiry<T> &inquiry)
	{
		InquiryMap.insert(pair<string, Inquiry<T>>(inquiry.GetInquiryId(), inquiry));
	}

	// Send a quote back to the client
	void SendQuote(const string &inquiryId, double price)
	{
		int s=0;
		Inquiry<T> inquiry = InquiryMap[inquiryId];
		for (int i=0;i<ListenerList.size();i++)
		{
			ServiceListener<Inquiry<T>>* listener = ListenerList[i];
			listener->ProcessAdd(inquiry);
			s=s+i;
		}
	}

	Inquiry<T>& GetData(string id)
	{
		return InquiryMap[id];
	}

	void SetQuoted(Inquiry<T> &inquiry)
	{
		inquiry.SetState(QUOTED);
	}

	void OnMessage(Inquiry<T> &inquiry)
	{
		AddInquiry(inquiry);
		SendQuote(inquiry.GetInquiryId(), inquiry.GetPrice());
	}

	// Reject an inquiry from the client
	void RejectInquiry(const string &inquiryId)
	{
	}

	void AddListener(ServiceListener<Inquiry<T>>* _listener)
	{
		ListenerList.push_back(_listener);
	}

	const vector<ServiceListener<Inquiry<T>>*>& GetListeners() const
	{
		return ListenerList;
	}

private:
	InquiryService() {}

};
 */

template<typename T>
class InquiryService : public Service<string,Inquiry <Bond> >
{

public:
	static InquiryService* Generate_Instance() {
		static InquiryService instance;
		return &instance;
	}

	// Send a quote back to the client
	void SendQuote(const string &inquiryId, double price) {}

	// Reject an inquiry from the client
	void RejectInquiry(const string &inquiryId) {}

	void OnMessage(Inquiry<Bond> &trade) override {
		trade.Set(trade.GetPrice(), DONE);
		std::cout << "flow the data from inquiryservice to the listener." << std::endl;
		for (auto& listener : _listeners)
			listener->ProcessAdd(trade);
	}

	Inquiry<Bond>& GetData(std::string _cusip) override {
		return _inquiryData.at(_cusip);
	}

	void AddListener(ServiceListener<Inquiry<Bond>> *listener) override {
		_listeners.push_back(listener);
	}

	const vector< ServiceListener<Inquiry<Bond>>* >& GetListeners() const override {
		return _listeners;
	}
private:

	std::map<std::string, Inquiry<Bond>> _inquiryData;

	std::vector<ServiceListener<Inquiry<Bond>>*> _listeners;

	InquiryService() { }

};

template<typename T>
class InquiryServiceListener : public ServiceListener<Inquiry<T>>
{
public:
	static InquiryServiceListener<T>* Generate_Instance()
	{
		static InquiryServiceListener<T> instance;
		return &instance;
	}

	virtual void ProcessAdd(Inquiry<T> &_inquiry)
	{
		auto inquiryconnector = InquiryConnector<T>::Generate_Instance();
		inquiryconnector->Publish(_inquiry, 100);
	}

	virtual void ProcessRemove(Inquiry<T> &data) {}

	virtual void ProcessUpdate(Inquiry<T> &data) {}

private:
	InquiryService<T>* _bondInquiryService;
	//InquiryConnector<T>* inquiryconnector;

	InquiryServiceListener()
	{
		_bondInquiryService = InquiryService<T>::Generate_Instance();
	}
};

template<typename T>
class InquiryConnector : public Connector<Inquiry<T>>
{
public:
	static InquiryConnector<T>* Generate_Instance() 
	{
		static InquiryConnector<T> instance;
		return &instance;
	}

	virtual void Publish(Inquiry<T> &data) {}

	void Subscribe() {

		/* some functions*/
		auto SplitLine = [](std::string& line) {
			stringstream enter_line(line);
			std::string item;
			std::vector<std::string> tmp;
			while (getline(enter_line, item, ',')) 	tmp.push_back(item);
			return tmp;
		};
		auto String2Price = [](std::string& str) {
			size_t idx = str.find_first_of('-');
			double result = std::stoi(str.substr(0, idx));
			int num1 = std::stoi(str.substr(idx + 1, 2));
			char ch = str[str.size() - 1];
			if (ch == '+') ch = '4';
			int num2 = ch - '0';
			result += (num1 * 8 + num2) / 256.0;
			return result;
		};

		static int inquiryId = 1; inquiryId++;
		ifstream is("input/inquiries.txt");
		string line;
		getline(is, line); 	// skip the header
		while (getline(is, line))
		{
			std::vector<std::string> elems = SplitLine(line);
			std::string cus_ip, side, quantity, price, state;
			cus_ip = elems[0]; side = elems[1]; quantity = elems[2];
			price = elems[3]; state = elems[4];
			InquiryState _state;
			if (state == "RECEIVED") _state = InquiryState::QUOTED;
			auto bond = _bondProductService->GetData(cus_ip);
			Inquiry<Bond> inq(std::to_string(inquiryId), bond, (side == "BUY" ? Side::BUY : Side::SELL),
				static_cast<long>(std::stod(quantity)), std::stod(price), _state);
			_bondInquiryServiceservice->OnMessage(inq);
		}
		std::cout << "allinquiries.txt Generated." << std::endl;
	}

	void Publish(Inquiry<T> &_data, double quote)
	{
		//_bondInquiryServiceservice->SetQuoted(_data);
		//_bondInquiryServiceservice->OnMessage(_data);
	}

	InquiryService<Bond>* GetService() 
	{
		return _bondInquiryServiceservice;
	}

private:
	InquiryConnector() 
	{
		_bondInquiryServiceservice = InquiryService<Bond>::Generate_Instance();
		_bondProductService = BondProductService::Generate_Instance();
	}
	InquiryService<Bond>* _bondInquiryServiceservice;
	BondProductService* _bondProductService;
};

template<typename T>
Inquiry<T>::Inquiry(string _inquiryId, const T &_product, Side _side, long _quantity, double _price, InquiryState _state) :
  product(_product)
{
  inquiryId = _inquiryId;
  side = _side;
  quantity = _quantity;
  price = _price;
  state = _state;
}

template<typename T>
const string& Inquiry<T>::GetInquiryId() const
{
  return inquiryId;
}

template<typename T>
const T& Inquiry<T>::GetProduct() const
{
  return product;
}

template<typename T>
Side Inquiry<T>::GetSide() const
{
  return side;
}

template<typename T>
long Inquiry<T>::GetQuantity() const
{
  return quantity;
}

template<typename T>
double Inquiry<T>::GetPrice() const
{
  return price;
}

template<typename T>
InquiryState Inquiry<T>::GetState() const
{
  return state;
}

template<typename T>
void Inquiry<T>::SetState(InquiryState _state)
{
	state = _state;
}

#endif
