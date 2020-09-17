//
// Created by krzysiu on 06.07.2020.
//

#ifndef TRAMHOLDUP_TTSSDIRECTOR_H
#define TRAMHOLDUP_TTSSDIRECTOR_H

#include <chrono>
#include <thread>
#include <boost/config.hpp>
#include "DataManager/DataManager.h"
#include "TTSSdataDownloader.h"
#include "TTSSdataParser.h"
#include "DataManager/PointsFunctions.h"

using namespace PFunctions;

namespace TTSSdata {

    class TTSSdirector : public DataManager {
    private:
        string basicUrl;
        string downloadPlace;
        TTSSdataDownloader *downloader;
        TTSSdataParser parser = TTSSdataParser(nullptr, nullptr);

        void updateDbBasics(unsigned maxTry = 5);
        void updateDbDetails(unsigned maxTry = 5);
        void downloadVehiclePositions(string &fileName);

        void eliminatedPositionsChecking();
        void isInEliminated(unsigned int start, unsigned int end, vector<VehicleData> *listVehicles);
    public:
        TTSSdirector(ConfigFile *config, unsigned short amountKeepOldVehiclePositions = 2);

        TTSSdirector();

        void initialize(ConfigFile *config, shared_ptr<spdlog::logger> &logger, unsigned short amountKeepOldVehiclePositions = 2);

        virtual ~TTSSdirector();

        void updateDb() override;

        void updateVehiclePositions() override;
    };

    extern "C" BOOST_SYMBOL_EXPORT TTSSdata::TTSSdirector dataManagerSL;
    TTSSdata::TTSSdirector dataManagerSL;
};


#endif //TRAMHOLDUPAJIO_TTSSDIRECTOR_H
