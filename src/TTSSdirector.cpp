//
// Created by krzysiu on 06.07.2020.
//

#include "TTSS/TTSSdirector.h"

TTSSdata::TTSSdirector::TTSSdirector(ConfigFile *config, unsigned short amountKeepOldVehiclePositions) : DataManager(
        config, amountKeepOldVehiclePositions) {
    basicUrl = conf->getValue("TTSSmainUrl");
    if (basicUrl[basicUrl.size() - 1] != '/')
        basicUrl += '/';
    downloader = new TTSSdataDownloader(basicUrl);

    //Preparing dir
    downloadPlace = conf->getValue("downloadPlace");
    downloadPlace = FileLib::getFullPath(downloadPlace);

    parser = TTSSdataParser(&dataContainer, &dataAccess);
}

TTSSdata::TTSSdirector::TTSSdirector() {}

void TTSSdata::TTSSdirector::initialize(ConfigFile *config, shared_ptr<spdlog::logger> &logger, unsigned short amountKeepOldVehiclePositions) {
    DataManager::initializeInterface(config, logger, amountKeepOldVehiclePositions);
    basicUrl = conf->getValue("TTSSmainUrl");
    if (basicUrl[basicUrl.size() - 1] != '/')
        basicUrl += '/';
    downloader = new TTSSdataDownloader(basicUrl);

    //Preparing dir
    downloadPlace = conf->getValue("downloadPlace");
    downloadPlace = FileLib::getFullPath(downloadPlace);

    parser = TTSSdataParser(&dataContainer, &dataAccess);
    loggerMain->trace("Initialized TTSSDownloader");
}

TTSSdata::TTSSdirector::~TTSSdirector() {
    delete downloader;

}

void TTSSdata::TTSSdirector::updateDb() {
    loggerMain->trace("Start updating database. Clearing actual data.");
    auto const maxTry = conf->getValue<unsigned>("maxRedownloadAttempts");
    clearData();

    loggerMain->info("Updating database");

    try {
        GenericException::tryRunFunctionCF([this]() {
           this->updateDbBasics();
           this->updateDbDetails();
       },
       [this](int tryCount, int maxTry) {
           this->clearData();

           unsigned sleepTime = random() % 100;
           this->loggerMain->info(
                   "Downloading error. Try again in {0}s. This is {1} try of max {2}.", sleepTime, tryCount, maxTry);

           this_thread::sleep_for(chrono::seconds(sleepTime));
       }, maxTry);
    } catch (GenericException &e) {
        if (e.getErrorLevel() == GenericException::ERROR) {
            loggerMain->critical("After {0} fails to download data app end!");
            throw GenericException("Can't update DB!", GenericException::CRITICAL_ERROR_END_PROGRAM);
        } else {
            loggerMain->critical("Critical Error when downloading! Close app! {0}", e.msg());
            throw GenericException("Error when downloading!", GenericException::CRITICAL_ERROR_END_PROGRAM);
        }
    }

    loggerMain->info("End Updating database");
}

void TTSSdata::TTSSdirector::updateVehiclePositions() {
    //TODO save vehicle positions to file
    string fileName = downloadPlace + "fp.json";
    fileName = FileLib::getFullPath(fileName);

    time_t lastVehicleTime = getTimeLastVehiclePositions();
    downloadVehiclePositions(fileName);

    if (!parser.parseVehiclePositions(fileName, lastVehicleTime)) {
        loggerMain->warn("Can't download actual vehicle positions. Downloaded positions is older or the same as last. Skip this iteration!");
        throw GenericException("Can't download actual vehicle position's. Skip this iteration!",
                               GenericException::ERROR_SKIP);
    }
    loggerMain->info("Downloaded and parsed {0} vehicle positions.", this->dataAccess.getActualVehiclesPositions()->size());

    eliminatedPositionsChecking();
}

void TTSSdata::TTSSdirector::updateDbBasics(unsigned maxTry) {
    try {
        GenericException::tryRunFunction([this]() {
            this->downloader->downloadAllStops(this->downloadPlace + "allStops.json");
            this->parser.parseStops(downloadPlace + "allStops.json");
        }, maxTry);
    } catch (GenericException &e) {
        if (e.getErrorLevel() == GenericException::ERROR) {
            throw GenericException("Downloading error!", GenericException::ERROR_TRY_REPEAT);
        } else {
            throw GenericException("Not typical Downloading Error!", GenericException::ERROR_CONTINUE);
        }
    }

    try {
        GenericException::tryRunFunction([this]() {
            this->downloader->downloadRoutes(downloadPlace + "lines.json");
            this->parser.parseLines(downloadPlace + "lines.json");
        }, maxTry);
    } catch (GenericException &e) {
        if (e.getErrorLevel() == GenericException::ERROR) {
            throw GenericException("Downloading error!", GenericException::ERROR_TRY_REPEAT);
        } else {
            throw GenericException("Not typical Downloading Error!", GenericException::ERROR_CONTINUE);
        }
    }
}

void TTSSdata::TTSSdirector::updateDbDetails(unsigned int maxTry) {
    loggerMain->trace("Start parsing line stops and shapes.");
    string stops, shape;
    unsigned directionNum;
    auto *lineList = dataAccess.getLines();
    for (auto &line: *lineList) {
        directionNum = 0;
        stops = string();

        try {
            stops = GenericException::tryRunFunction([this, line]() {
                return this->downloader->downloadLineStops(line.lineId)->str();
            }, maxTry);
        } catch (GenericException &e) {
            if (e.getErrorLevel() == GenericException::ERROR) {
                loggerMain->error("Downloading error on stops list for lineId:{0}! Try to repeat!", line.lineId);
                throw GenericException("Downloading error!", GenericException::ERROR_TRY_REPEAT);
            } else {
                loggerMain->error("Other error on stops list for lineId:{0}! Skipping!");
                throw GenericException("Not typical Downloading Error!", GenericException::ERROR_CONTINUE);
            }
        }

        try {
            shape = GenericException::tryRunFunction([this, line]() {
                return this->downloader->downloadLineShapes(line.lineId)->str();
            }, maxTry);
        } catch (GenericException &e) {
            if (e.getErrorLevel() == GenericException::ERROR) {
                loggerMain->error("Downloading error on shapes lineId:{0}! Try to repeat!", line.lineId);
                throw GenericException("Downloading error!", GenericException::ERROR_TRY_REPEAT);
            } else {
                loggerMain->error("Other error on shapes lineId:{0}! Skipping!", line.lineId);
                throw GenericException("Not typical Downloading Error!", GenericException::ERROR_CONTINUE);
            }
        }

        for(auto &trip: line.tripList) {
            parser.parseLineStops(&stops, &trip);
            parser.parseLineShapes(&shape, &trip, directionNum);
            ++directionNum;
        }
    }
    loggerMain->trace("End parsing line stops and shapes.");
}

void TTSSdata::TTSSdirector::downloadVehiclePositions(string &fileName) {
    loggerMain->trace("Start downloading vehicle positions");

    try {
        GenericException::tryRunFunction([fileName, this]() {
            this->downloader->downloadVehiclePositions(fileName);
        }, 3);
    } catch (GenericException &e) {
        if (e.getErrorLevel() == GenericException::ERROR) {
            loggerMain->error("3 unsuccessfully try to download vehicle position!");
            throw GenericException("Error when downloading vehicle positions!", GenericException::ERROR_SKIP);
        }
    }
    loggerMain->debug("Downloaded vehicle positions.");
}

void TTSSdata::TTSSdirector::eliminatedPositionsChecking() {
    thread *threadTab;
    vector<VehicleData> *listVehicles = dataAccess.getActualVehiclesPositions();

    unsigned threadAmount = listVehicles->size() / 12;
    if (threadAmount * 12 < listVehicles->size())
        ++threadAmount;

    threadTab = new thread[threadAmount];
    for (int i = 0; i < threadAmount; ++i) {
        if (i * 12 + 12 < listVehicles->size()) {
            threadTab[i] = thread(&TTSSdata::TTSSdirector::isInEliminated, this, i * 12,
                                  i * 12 + 12, listVehicles);
        } else {
            threadTab[i] = thread(&TTSSdata::TTSSdirector::isInEliminated, this, i * 12, listVehicles->size() - 1,
                                  listVehicles);
        }
    }

    for (int i = 0; i < threadAmount; ++i) {
        threadTab[i].join();
    }

    delete[] threadTab;
}

void TTSSdata::TTSSdirector::isInEliminated(unsigned int start, unsigned int end, vector<VehicleData> *listVehicles) {
    bool eliminated;
    for (; start < end; ++start) {
        eliminated = isInEliminatedPosition(listVehicles->at(start).position);

        if (!eliminated) {
            if (listVehicles->at(start).trip != nullptr && !listVehicles->at(start).trip->shape.empty()) {
                if (approximateDistanceInMeters(listVehicles->at(start).position,
                                                listVehicles->at(start).trip->shape[0]) <
                    eliminatedPositionDistanceOnBight ||
                    approximateDistanceInMeters(listVehicles->at(start).position,
                                                listVehicles->at(start).trip->shape[
                                                        listVehicles->at(start).trip->shape.size() - 1]) <
                    eliminatedPositionDistanceOnBight) {
                    eliminated = true;
                }
            }
        }

        listVehicles->at(start).isInEliminatedPosition = eliminated;
    }
}
