#ifndef ALGOSTREAMING_SERVICE_LISTENER_HPP
#define ALGOSTREAMING_SERVICE_LISTENER_HPP

#include "soa.hpp"
#include "streamingservice.hpp"
#include "algostreamingservice.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

template<typename T>
class AlgoStreamingServiceListener : public ServiceListener<AlgoPriceStream<T>>
{
public:
	static AlgoStreamingServiceListener<T>* Generate_Instance()
	{
		static AlgoStreamingServiceListener<T> instance;
		return &instance;
	}

	void ProcessAdd(AlgoPriceStream<T> &data)
	{
		PriceStream<T> pricestream = data.GetPriceStream();
		_bondStreamingService->PublishPrice(pricestream);
	}

	void ProcessRemove(AlgoPriceStream<T> &data) {}
	void ProcessUpdate(AlgoPriceStream<T> &data) {}

	AlgoStreamingService<T>* GetService()
	{
		return _bondAlgoStreamingService;
	}

private:
	StreamingService<T>* _bondStreamingService;
	AlgoStreamingService<T>* _bondAlgoStreamingService;

	AlgoStreamingServiceListener()
	{
		_bondStreamingService = StreamingService<T>::Generate_Instance();
		_bondAlgoStreamingService = AlgoStreamingService<T>::Generate_Instance();
	}
};

#endif