//
// Created by cysiu on 09.01.2020.
//

#include "TTSS/TTSSdataDownloader.h"

void TTSSdata::TTSSdataDownloader::downloadRoutes(string downloadName) {
    if(downloadName.size() < 3) {
        loggerMain->critical("Passed name for file with download routes is too short! ({0})", downloadName);
        throw GenericException("Passed name for file with download routes is too short!",
                               GenericException::CRITICAL_ERROR_END_PROGRAM);
    }
    downloadName = FileLib::getFullPath(downloadName);
    if(FileLib::fileExist(downloadName))
        FileLib::removeFile(downloadName);

    try {
        downloader.setUrl(mainUrl + "services/routeInfo/route");
        downloader.setSavePath(downloadName);
        downloader.download();
    } catch (Exception &e){
        loggerMain->error("Throw error on downloading routes: {0}", e.msg());
        throw GenericException("Error when downloading routes!", GenericException::ERROR_TRY_REPEAT);
    }
}

stringstream * TTSSdata::TTSSdataDownloader::downloadLineStops(const string& routeId) {
    try {
        downloaderRam.getAdd("routeId", routeId);
        downloaderRam.setUrl(mainUrl + "services/routeInfo/routeStops");
        downloaderRam.downloadToRam();
        downloaderRam.getClear();
        return downloaderRam.getDownloadedData();
    } catch (Exception &e){
        loggerMain->error("Throw error on downloading route stops: {0}", e.msg());
        throw GenericException("Error when downloading stops for line!", GenericException::ERROR_TRY_REPEAT);
    }
}

stringstream * TTSSdata::TTSSdataDownloader::downloadLineShapes(const string& routeId) {
    try {
        downloaderRam.getAdd("id", routeId);
        downloaderRam.setUrl(mainUrl + "geoserviceDispatcher/services/pathinfo/route");
        downloaderRam.downloadToRam();
        downloaderRam.getClear();
        return downloaderRam.getDownloadedData();
    } catch (Exception &e){
        loggerMain->error("Throw error on downloading line shapes: {0}", e.msg());
        throw GenericException("Error when downloading shapes for line!", GenericException::ERROR_TRY_REPEAT);
    }
}

TTSSdata::TTSSdataDownloader::TTSSdataDownloader(string mainUrl) : mainUrl(move(mainUrl)) {
    downloader.setEndOnError(false);
    downloaderRam.setEndOnError(false);
    loggerMain = Logger::getLogger("loggerMain");

}

void TTSSdata::TTSSdataDownloader::downloadVehiclePositions(string downloadName) {
    if(downloadName.size() < 3) {
        loggerMain->critical("Passed name for file with downloaded vehicles positions is too short! ({0})",
                downloadName);
        throw GenericException("Passed name for file with downloaded vehicles positions is too short!",
                               GenericException::CRITICAL_ERROR_END_PROGRAM);
    }
    downloadName = FileLib::getFullPath(downloadName);
    if(FileLib::fileExist(downloadName))
        FileLib::removeFile(downloadName);

    try {
        downloader.getAdd("positionType", "CORRECTED");
        downloader.setUrl(mainUrl + "geoserviceDispatcher/services/vehicleinfo/vehicles");
        downloader.setSavePath(downloadName);
        downloader.download();
        downloader.getClear();
    } catch (Exception &e){
        loggerMain->error("Throw error on downloading vehicles positions: {0}", e.msg());
        throw GenericException("Error when downloading vehicle positions!", GenericException::ERROR_TRY_REPEAT);
    }
}

void TTSSdata::TTSSdataDownloader::downloadAllStops(string downloadName) {
    if(downloadName.size() < 3) {
        loggerMain->critical("Passed name for file with downloaded all stops is too short! ({0})", downloadName);
        throw GenericException("Passed name for file with downloaded all stops is too short!",
                               GenericException::CRITICAL_ERROR_END_PROGRAM);
    }
    downloadName = FileLib::getFullPath(downloadName);
    if(FileLib::fileExist(downloadName))
        FileLib::removeFile(downloadName);

    try {
        downloader.getAdd("left", to_string((int)(19.802285 * TTSSdata::locationCorrection)));
        downloader.getAdd("bottom", to_string((int)(49.993417 * TTSSdata::locationCorrection)));
        downloader.getAdd("right", to_string((int)(20.150429 * TTSSdata::locationCorrection)));
        downloader.getAdd("top", to_string((int)(50.130574 * TTSSdata::locationCorrection)));
        downloader.setUrl(mainUrl + "geoserviceDispatcher/services/stopinfo/stops");
        downloader.setSavePath(downloadName);
        downloader.download();
        downloader.getClear();
    } catch (Exception &e){
        loggerMain->error("Throw error on downloading all stops: {0}", e.msg());
        throw GenericException("Error when downloading all stops!", GenericException::ERROR_TRY_REPEAT);
    }
}
