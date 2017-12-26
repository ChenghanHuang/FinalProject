/**
* historicaldataservice.hpp
* historicaldataservice.hpp
*
* @author Breman Thuraisingham
* Defines the data types and Service for historical data.
*
* @author Breman Thuraisingham
*/
#ifndef HISTORICAL_DATA_SERVICE_HPP
#define HISTORICAL_DATA_SERVICE_HPP

#include "riskservice.hpp"
#include "executionservice.hpp"
#include "streamingservice.hpp"
#include "inquiryservice.hpp"
#include "soa.hpp"
#include "products.hpp"
#include <fstream>

/**
* Service for processing and persisting historical data to a persistent store.
* Keyed on some persistent key.
* Type T is the data type to persist.
*/
template<typename T>
class HistoricalDataService : Service<string, T>
{

public:

	// Persist data to a store
	virtual void PersistData(string persistKey, T& data) = 0;

};



/* Risk.txt*/
class BondHistoricalPV01Connector : public Connector<PV01<Bond>>
{
public:
	static BondHistoricalPV01Connector* Generate_Instance()
	{
		static BondHistoricalPV01Connector instance;
		return &instance;
	}

	void Publish(PV01<Bond>& data)
	{
		ofstream os("output/risk.txt", ios_base::app);
		string msg = "PV01 is " + std::to_string(data.GetPV01());
		os << msg << endl;
	}

	void Subscribe() {}

private:
	BondHistoricalPV01Connector() {}

};

class BondHistoricalPV01Service : public HistoricalDataService<PV01<Bond> >
{

public:
	static BondHistoricalPV01Service* Generate_Instance()
	{
		static BondHistoricalPV01Service instance;
		return &instance;
	}

	// function overloading
	PV01<Bond> & GetData(string persistKey)
	{
		return _Data.at(persistKey);
	}
	void OnMessage(PV01<Bond> &b)
	{
		auto persistKey = b.GetProduct().GetProductId();
		_Data[persistKey] = b;
		std::cout << "flow the data from BondHistoricalPV01Service to the listener." << std::endl;
		for (auto& lp : _listeners) lp->ProcessAdd(b); // notify listeners
	}

	void AddListener(ServiceListener<PV01<Bond> > *listener)
	{
		_listeners.push_back(listener);
	}
	const vector<ServiceListener<PV01<Bond> > *>& GetListeners() const
	{
		return _listeners;
	}
	void PersistData(string persistKey, PV01<Bond>& data)
	{
		_bondHistoricalPV01Connector->Publish(data);
	}

private:
	vector<ServiceListener<PV01<Bond> >*> _listeners;      // member data for listeners
	map<string, PV01 <Bond> > _Data;                       // store the type data to persist
	BondHistoricalPV01Connector* _bondHistoricalPV01Connector; // call connector to write
	BondHistoricalPV01Service() { _bondHistoricalPV01Connector = BondHistoricalPV01Connector::Generate_Instance(); }
};



/*execution.txt*/

class BondHistoricalExecutionConnector : public Connector<ExecutionOrder<Bond>>
{
public:
	static BondHistoricalExecutionConnector* Generate_Instance()
	{
		static BondHistoricalExecutionConnector instance;
		return &instance;
	}

	// function overloading
	void Publish(ExecutionOrder<Bond>& data)
	{
		ofstream os("output/executions.txt", ios_base::app);
		std::string msg = "executing order. orderid:" + data.GetOrderId()
			+ " CUSID: " + data.GetProduct().GetProductId();
		os << data << endl;
	}

	void Subscribe() {}  // implement nothing, publish-only

private:
	BondHistoricalExecutionConnector() {}

};

class BondHistoricalExecutionService : public HistoricalDataService<ExecutionOrder<Bond> >
{

public:
	static BondHistoricalExecutionService* Generate_Instance()
	{
		static BondHistoricalExecutionService instance;
		return &instance;
	}

	// function overloading
	ExecutionOrder<Bond> & GetData(string persistKey)
	{
		return _Data.at(persistKey);
	}
	void OnMessage(ExecutionOrder<Bond> &b)
	{
		auto persistKey = b.GetProduct().GetProductId();
		_Data[persistKey] = b;
		std::cout << "flow the data from BondHistoricalExecutionService to the listener." << std::endl;
		for (auto& lp : _listeners) lp->ProcessAdd(b); // notify listeners
	}

	void AddListener(ServiceListener<ExecutionOrder<Bond>> *listener)
	{
		_listeners.push_back(listener);
	}
	const vector<ServiceListener<ExecutionOrder<Bond>>*>& GetListeners() const
	{
		return _listeners;
	}
	void PersistData(string persistKey, ExecutionOrder<Bond>& data)
	{
		_bondHistoricalExecutionConnector->Publish(data);
	}

private:
	vector<ServiceListener<ExecutionOrder<Bond> >*> _listeners;      // member data for listeners
	map<string, ExecutionOrder <Bond> > _Data;                       // store the type data to persist
	BondHistoricalExecutionConnector* _bondHistoricalExecutionConnector; // call connector to write
	BondHistoricalExecutionService() { _bondHistoricalExecutionConnector = BondHistoricalExecutionConnector::Generate_Instance(); }
};




/*stream.txt*/
class BondHistoricalStreamingConnector : public Connector<PriceStream<Bond>>
{
public:
	static BondHistoricalStreamingConnector* Generate_Instance()
	{
		static BondHistoricalStreamingConnector instance;
		return &instance;
	}

	// function overloading
	void Publish(PriceStream<Bond>& data)
	{
		ofstream os("output/streaming.txt", ios_base::app);

		double bid = data.GetBidOrder().GetPrice();
		double offer = data.GetOfferOrder().GetPrice();
		std::string cus_id = data.GetProduct().GetProductId();
		std::string msg = "CUSID: " + cus_id + "; bid price: " + std::to_string(bid) +
			"; offer price: " + std::to_string(offer);
		os << msg << endl;
	}

	void Subscribe() {}  // implement nothing, publish-only

private:
	BondHistoricalStreamingConnector() {}

};

class BondHistoricalStreamingService : public HistoricalDataService<PriceStream<Bond> >
{

public:
	static BondHistoricalStreamingService* Generate_Instance()
	{
		static BondHistoricalStreamingService instance;
		return &instance;
	}

	// function overloading
	PriceStream<Bond> & GetData(string persistKey)
	{
		return _Data.at(persistKey);
	}
	void OnMessage(PriceStream<Bond> &b)
	{
		auto persistKey = b.GetProduct().GetProductId();
		_Data[persistKey] = b;
		std::cout << "flow the data from BondHistoricalExecutionService to the listener." << std::endl;
		for (auto& lp : _listeners) lp->ProcessAdd(b); // notify listeners
	}

	void AddListener(ServiceListener<PriceStream<Bond> > *listener)
	{
		_listeners.push_back(listener);
	}
	const vector<ServiceListener<PriceStream<Bond> > *>& GetListeners() const
	{
		return _listeners;
	}
	void PersistData(string persistKey, PriceStream<Bond>& data)
	{
		_bondHistoricalStreamingConnector->Publish(data);
	}

private:
	vector<ServiceListener<PriceStream<Bond> >*> _listeners;      // member data for listeners
	map<string, PriceStream <Bond> > _Data;                       // store the type data to persist
	BondHistoricalStreamingConnector* _bondHistoricalStreamingConnector; // call connector to write
	BondHistoricalStreamingService() { _bondHistoricalStreamingConnector = BondHistoricalStreamingConnector::Generate_Instance(); }
};

class BondHistoricalStreamingServiceListener : public ServiceListener<PriceStream<Bond>>
{
public:
	static BondHistoricalStreamingServiceListener* Generate_Instance()
	{
		static BondHistoricalStreamingServiceListener instance;
		return &instance;
	}

	// function overloading
	void ProcessAdd(PriceStream<Bond> &data)
	{
		_bondHistoryStreamingService->OnMessage(data);
		_bondHistoryStreamingService->PersistData(data.GetProduct().GetProductId(), data); // to write.
	}
	void ProcessRemove(PriceStream<Bond> &data) {}
	void ProcessUpdate(PriceStream<Bond> &data) {}

private:
	BondHistoricalStreamingService * _bondHistoryStreamingService;

	BondHistoricalStreamingServiceListener() { _bondHistoryStreamingService = BondHistoricalStreamingService::Generate_Instance(); }

};


/*allinquiry.txt*/
class BondHistoricalInquiryConnector : public Connector<Inquiry<Bond>>
{
public:
	static BondHistoricalInquiryConnector* Generate_Instance()
	{
		static BondHistoricalInquiryConnector instance;
		return &instance;
	}

	// function overloading
	void Publish(Inquiry<Bond>& data)
	{
		ofstream os("output/allinquiries.txt", ios_base::app);
		std::string msg;
		msg += "inquiry id is: " + data.GetInquiryId();
		data.GetSide() == Side::BUY ? msg += "; BUY " : msg += "; SELL ";
		msg += data.GetProduct().GetProductId() + " for ";
		msg += std::to_string(data.GetQuantity()) + " quantity, at ";
		msg += std::to_string(data.GetPrice()) + " price.";
		os << msg << endl;
	}

	void Subscribe() {}  // implement nothing, publish-only

private:
	BondHistoricalInquiryConnector() {}

};

class BondHistoricalInquiryService : public HistoricalDataService<Inquiry<Bond> >
{
public:
	static BondHistoricalInquiryService* Generate_Instance()
	{
		static BondHistoricalInquiryService instance;
		return &instance;
	}

	// function overloading
	Inquiry<Bond> & GetData(string persistKey)
	{
		return _inquriyData.at(persistKey);
	}
	void OnMessage(Inquiry<Bond> &b)
	{
		auto persistKey = b.GetProduct().GetProductId();
		_inquriyData[persistKey] = b;
		std::cout << "flow the data from BondHistoricalInquiryService to the listener." << std::endl;
		for (auto& lp : _listeners) lp->ProcessAdd(b); // notify listeners
	}

	void AddListener(ServiceListener<Inquiry<Bond> > *listener)
	{
		_listeners.push_back(listener);
	}
	const vector<ServiceListener<Inquiry<Bond> > *>& GetListeners() const
	{
		return _listeners;
	}
	void PersistData(string persistKey, Inquiry<Bond>& data)
	{
		_bondHistoricalInquiryConnector->Publish(data);
	}

private:
	vector<ServiceListener<Inquiry<Bond> >*> _listeners;      // member data for listeners
	map<string, Inquiry <Bond> > _inquriyData;                       // store the type data to persist
	BondHistoricalInquiryConnector* _bondHistoricalInquiryConnector; // call connector to write

	BondHistoricalInquiryService() { _bondHistoricalInquiryConnector = BondHistoricalInquiryConnector::Generate_Instance(); }
};

class BondHistoricalInquiryServiceListener : public ServiceListener<Inquiry<Bond>>
{
public:
	static BondHistoricalInquiryServiceListener* Generate_Instance()
	{
		static BondHistoricalInquiryServiceListener instance;
		return &instance;
	}

	// function overloading
	void ProcessAdd(Inquiry<Bond> &data)
	{
		_bondHistoryInquiryService->OnMessage(data);
		_bondHistoryInquiryService->PersistData(data.GetProduct().GetProductId(), data); // to write.
	}
	void ProcessRemove(Inquiry<Bond> &data) {}
	void ProcessUpdate(Inquiry<Bond> &data) {}

private:
	BondHistoricalInquiryService * _bondHistoryInquiryService;

	BondHistoricalInquiryServiceListener() { _bondHistoryInquiryService = BondHistoricalInquiryService::Generate_Instance(); }

};


#endif
