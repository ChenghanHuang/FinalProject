#include "products.hpp"
#include "tradebookingservice.hpp"
#include "positionservice.hpp"
#include "positionservicelistener.hpp"
#include "riskservice.hpp"
#include "riskservicelistener.hpp"
#include "inquiryservice.hpp"
#include "pricingservice.hpp"
#include "pricingservicelistener.hpp"
#include "marketdataservice.hpp"
#include "marketdataservicelistener.hpp"
#include "historicaldataservice.hpp"
#include "executionservice.hpp"
#include "executionservicelistener.hpp"
#include "algostreamingservice.hpp"
#include "algostreamingservicelistener.hpp"
#include "algoexecutionservice.hpp"
#include "algoexecutionservicelistener.hpp"
#include "streamingservice.hpp"
#include "streamingservicelistener.hpp"
#include "DataGenerator.hpp"
#include <fstream>
#include <iostream>


using namespace std;

class Bond;

int main()
{
    //Generate data and print them into the input folder
    GenerateData();

    //Calculate corresponding data and print them into the output folder
    // priceservice ->algostreaming ->streaming ->historicaldataservice
    auto BondPricingServiceConnector = PricingServiceConnector::Generate_Instance();
    auto BondPricingService = BondPricingServiceConnector->GetService();
    auto BondPricingServiceListener = PricingServiceListener<Bond>::Generate_Instance();
    BondPricingService->AddListener(BondPricingServiceListener);
    auto BondAlgoStreamingServiceListener = AlgoStreamingServiceListener<Bond>::Generate_Instance();
    auto BondAlgoStreamingService = BondAlgoStreamingServiceListener->GetService();
    BondAlgoStreamingService->AddListener(BondAlgoStreamingServiceListener);
    auto BondStreamingServiceListener = StreamingServiceListener<Bond>::Generate_Instance();
    auto BondStreamingService = BondStreamingServiceListener->GetService();
    BondStreamingService->AddListener(BondStreamingServiceListener);
    // read the data and output stream.txt
    BondPricingServiceConnector->Subscribe();

    // marketdataservice ->algoexecution -> execution -> historicaldataservice
	auto BondMarketDataServiceConnector = MarketDataConnector<Bond>::Generate_Instance();
	auto BondMarketDataService = BondMarketDataServiceConnector->GetService();
	auto BondMarketDataServiceListener = MarketDataServiceListener<Bond>::Generate_Instance();
    BondMarketDataService->AddListener(BondMarketDataServiceListener);
	auto BondAlgoExecutionServiceListener = AlgoExecutionServiceListener<Bond>::Generate_Instance();
	auto BondAlgoExecutionService = BondAlgoExecutionServiceListener->GetService();
    BondAlgoExecutionService->AddListener(BondAlgoExecutionServiceListener);
	auto BondExecutionServiceListener = ExecutionServiceListener<Bond>::Generate_Instance();
	auto BondExecutionService = BondExecutionServiceListener->GetService();
    BondExecutionService->AddListener(BondExecutionServiceListener);
	// read the data and output executions.txt
    BondMarketDataServiceConnector->Subscribe();

    // tradingbookingservice -> positionservice -> riskservice -> historicaldataservice
    auto BondTradeBookingServiceConnector = TradeBookingConnector::Generate_Instance();
    auto BondTradeBookingService = BondTradeBookingServiceConnector->GetService();
    auto BondTradeBookingListener = TradeBookingServiceListener<Bond>::Generate_Instance();
    BondTradeBookingService->AddListener(BondTradeBookingListener);
    auto BondPositionServiceListener = PositionServiceListener<Bond>::Generate_Instance();
    auto BondPositionService = BondPositionServiceListener->GetService();
    BondPositionService->AddListener(BondPositionServiceListener);
    auto BondRiskServiceListener = RiskServiceListener<Bond>::Generate_Instance();
    auto BondRiskService = BondRiskServiceListener->GetService();
    BondRiskService->AddListener(BondRiskServiceListener);
    // read the data and output risk.txt
    BondTradeBookingServiceConnector->Subscribe();

	// inquiryservice -> historicaldataservice
	auto BondInquiryServiceConnector = InquiryConnector<Bond>::Generate_Instance();
	auto BondInquiryService = BondInquiryServiceConnector->GetService();
	auto BondInquriyServiceListener = InquiryServiceListener<Bond>::Generate_Instance();
	auto BondHistoricalInquriyServiceListener = BondHistoricalInquiryServiceListener::Generate_Instance();
    BondInquiryService->AddListener(BondInquriyServiceListener);
    BondInquiryService->AddListener(BondHistoricalInquriyServiceListener);
	// read the data and output inquiry.txt
    BondInquiryServiceConnector->Subscribe();

    return 0;
}
