#ifndef INC_2_DATAGENERATOR_HPP
#define INC_2_DATAGENERATOR_HPP

#include "products.hpp"
#include "positionservice.hpp"
#include "riskservice.hpp"
#include "inquiryservice.hpp"
#include "pricingservice.hpp"
#include "marketdataservice.hpp"
#include "historicaldataservice.hpp"
#include "executionservice.hpp"
#include "streamingservice.hpp"

extern std::vector<std::string> CUSIP_CODE;

std::vector<std::string> CUSIP_CODE = {
        "3137EAED7", "3137EAEB1", "3137EAEC9",
        "3137EADB2", "3134A3U46", "3134A4KX1" };

void BondInformationGenerator();
void TradeDataGenerator();
void PriceDataGenerator();
void MarketDataGenerator();
void InquiriesDataGenerator();

void GenerateData()
{
    // Generate Data
    BondInformationGenerator();
    TradeDataGenerator();
    PriceDataGenerator();
    MarketDataGenerator();
    InquiriesDataGenerator();
}

void TradeDataGenerator()
{
    ofstream file_trade;
    file_trade.open("input/trades.txt", ios::out | ios::trunc);
    file_trade << "CUSIP,Trade_ID,Book,Price,Quantity,Side\n";
    for (int i = 1; i <= 6; ++i)
    {
        std::string CUS_IP = CUSIP_CODE[i - 1];
        for (int j = 1; j <= 10; ++j)
        {
            int num, num1, num2, num3, num4;
            std::string str1, str2, str3;
            num = rand() % (256 * 2 + 1);
            num1 = num / 256; num2 = num % 256;
            num3 = num2 / 8; num4 = num2 % 8;
            str1 = std::to_string(99 + num1) + "-";
            str2 = std::to_string(num3);
            str3 = std::to_string(num4);
            if (num4 == 4)	str3 = "+";
            if (num3 < 10) str2 = "0" + str2;
            file_trade << CUS_IP << ",T" << (i - 1) * 10 + j << ",TRSY" << 1 + rand() % 3
                       << "," << str1 + str2 + str3 << "," << (1 + rand() % 9) * 1000000 << ","
                       << (rand() % 2 == 1 ? "BUY" : "SELL") << std::endl;
        }
    }
}

void BondInformationGenerator() {
    std::vector<float> BondCoupon = {
            static_cast<float>(.8775 / 100.), static_cast<float>(.875 / 100.), static_cast<float>(1.125 / 100.),
            static_cast<float>(2.375 / 100.), static_cast<float>(6.75 / 100.), static_cast<float>(6.25 / 100.)};
    std::vector <date> BondMaturity = {date(2018, 10, 12), date(2019, 07, 19), date(2021, 11, 12),
                                       date(2022, 01, 13), date(2029, 11, 15), date(2032, 07, 15)};

    auto bondProductService = BondProductService::Generate_Instance();
    auto bondPositionService = PositionService<Bond>::Generate_Instance();
    auto bondRiskService = RiskService<Bond>::Generate_Instance();
    for (int i = 0; i < 6; ++i) {
        Bond bond_tmp(CUSIP_CODE[i], CUSIP, "T", BondCoupon[i], BondMaturity[i]);
        Position <Bond> position_tmp(bond_tmp);
        PV01 <Bond> pv01_tmp(bond_tmp, rand() % 1 / 100000., position_tmp.GetAggregatePosition());
        bondProductService->Add(bond_tmp);
        bondPositionService->AddPosition(position_tmp);
        bondRiskService->Add(pv01_tmp);
    }
}

void PriceDataGenerator()
{
    auto Price2String = [](int num)
    {
        int num1 = num / 256, num2 = num % 256,
                num3 = num2 / 8, num4 = num2 % 8;
        string str1 = std::to_string(99 + num1) + "-",
                str2 = std::to_string(num3), str3 = std::to_string(num4);
        if (num3 < 10) str2 = "0" + str2;
        if (num4 == 4)	str3 = "+";
        return str1 + str2 + str3;
    };
    ofstream file_price;
    file_price.open("input/prices.txt", ios::out | ios::trunc);
    file_price << "CUSIP,mid,bidofferspread\n";
    for (int j = 1; j <= 100; ++j)
    {
        for (int i = 1; i <= 6; ++i)
        {
            int mid_num = rand() % (256 * 2 - 8) + 4;
            int tmp = (rand() % 3 + 2); // mock th price ossilation
            std::string osc_str = "0-00" + (tmp == 4 ? "+" : std::to_string(tmp));
            file_price << CUSIP_CODE[i - 1] << "," << Price2String(mid_num) << ',' << osc_str << endl;
        }
    }
}

void MarketDataGenerator()
{
    auto Price2String = [](int num)
    {
        int num1 = num / 256, num2 = num % 256,
                num3 = num2 / 8, num4 = num2 % 8;
        string str1 = std::to_string(99 + num1) + "-",
                str2 = std::to_string(num3), str3 = std::to_string(num4);
        if (num3 < 10) str2 = "0" + str2;
        if (num4 == 4)	str3 = "+";
        return str1 + str2 + str3;
    };

    ofstream file_marketdata;
    file_marketdata.open("input/marketdata.txt", ios::out | ios::trunc);
    file_marketdata << "CUSIP,bidprice1,quantity,bidprice2,quantity,bidprice3,quantity,bidprice4,quantity,bidprice5,quantity,";
    file_marketdata << "offerprice1,quantity,offerprice2,quantity,offerprice3,quantity,offerprice4,quantity,offerprice5,quantity,\n";
    for (int j = 1; j <= 10; ++j)
    {
        for (int i = 1; i <= 6; ++i)
        {
            string cus_ip = CUSIP_CODE[i - 1];
            file_marketdata << cus_ip << ',';
            int mid_num = rand() % (256 * 2 + 1);

            // bid prices in descending order
            int bid_num = mid_num - 1;
            for (int k = 1; k <= 5; ++k)
            {
                int quantity = 1000000 * k;
                file_marketdata << Price2String(bid_num--) << ',' << quantity << ',';
            }
            // offer prices in ascending order
            int offer_num = mid_num + 1;
            for (int k = 1; k <= 5; ++k)
            {
                string offer_price = Price2String(offer_num++);
                int quantity = 1000000 * k;
                file_marketdata << offer_price << ',' << quantity << ',';
            }
            file_marketdata << endl;
        }
    }
}

void InquiriesDataGenerator()
{
    ofstream file_inquiries;
    file_inquiries.open("input/inquiries.txt", ios::out | ios::trunc);
    file_inquiries << "CUSIP, side, quantity, price, state\n";
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            file_inquiries << CUSIP_CODE[j] + ',' + (rand() % 2 == 0 ? "BUY" : "SELL") + ',' + std::to_string(rand() % 1000 * i) + ',' + "100" + ',' + "RECEIVED" << endl;
        }
    }
}

#endif //INC_2_DATAGENERATOR_HPP
