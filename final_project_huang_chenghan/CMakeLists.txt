cmake_minimum_required(VERSION 3.9)
project(final_project_huang_chenghan)

set(CMAKE_CXX_STANDARD 11)

include_directories(/boost_1_65_1)

add_executable(final_project_huang_chenghan
        algoexecutionservice.hpp
        algoexecutionservicelistener.hpp
        algostreamingservice.hpp
        algostreamingservicelistener.hpp
        BondInformationGenerator.cpp
        DataGenerator.hpp
        executionservice.hpp
        executionservicelistener.hpp
        guiservice.hpp
        historicaldataservice.hpp
        inquiryservice.hpp
        main.cpp
        marketdataservice.hpp
        marketdataservicelistener.hpp
        positionservice.hpp
        positionservicelistener.hpp
        pricingservice.hpp
        pricingservicelistener.hpp
        products.hpp
        riskservice.hpp
        riskservicelistener.hpp
        soa.hpp
        streamingservice.hpp
        streamingservicelistener.hpp
        tradebookingservice.hpp)
