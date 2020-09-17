//
// Created by cysiu on 09.01.2020.
//

#ifndef TRAMHOLDUPAJIO_TTSSDATADOWNLOADER_H
#define TRAMHOLDUPAJIO_TTSSDATADOWNLOADER_H

#include <string>
#include "Logger.h"
#include "downloaderLib/Downloader.h"
#include "fileLib/FileLib.h"
#include "include/Exception.h"
#include "GenericException.h"
#include "TTSScorrection.h"

using namespace std;

namespace TTSSdata {
    class TTSSdataDownloader {
    private:
        shared_ptr<spdlog::logger> loggerMain;
        string mainUrl;
        Downloader downloader;
        DownloaderRAM downloaderRam;

    public:
        TTSSdataDownloader(string mainUrl);

        void downloadRoutes(string downloadName);

        void downloadAllStops(string downloadName);

        stringstream *downloadLineStops(const string& routeId);

        stringstream * downloadLineShapes(const string& routeId);

        void downloadVehiclePositions(string downloadName);
    };
}


#endif //TRAMHOLDUPAJIO_TTSSDATADOWNLOADER_H
