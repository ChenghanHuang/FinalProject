#ifndef STREAMING_SERVICE_LISTENER_HPP
#define STREAMING_SERVICE_LISTENER_HPP

#include <iostream>
#include "soa.hpp"
#include "marketdataservice.hpp"
#include "pricingservice.hpp"
#include "streamingservice.hpp"
#include "riskservice.hpp"
#include "historicaldataservice.hpp"

template<typename T>
class StreamingServiceListener : public ServiceListener<PriceStream<T>>
{
public:
	static StreamingServiceListener<T>* Generate_Instance()
	{
		static StreamingServiceListener<T> instance;
		return &instance;
	}

	void ProcessAdd(PriceStream<T> &data)
	{
		_bondHistoryStreamingService->OnMessage(data);
		_bondHistoryStreamingService->PersistData(data.GetProduct().GetProductId(), data);
	}

	void ProcessRemove(PriceStream<T> &data) {}
	void ProcessUpdate(PriceStream<T> &data) {}

	StreamingService<T>* GetService()
	{
		return _bondStreamingService;
	}

private:
	StreamingService<T>* _bondStreamingService;
	BondHistoricalStreamingService* _bondHistoryStreamingService;

	StreamingServiceListener()
	{
		_bondStreamingService = StreamingService<T>::Generate_Instance();
		_bondHistoryStreamingService = BondHistoricalStreamingService::Generate_Instance();
	}
};

#endif