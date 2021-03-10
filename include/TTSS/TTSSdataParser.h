//
// Created by krzysiu on 09.07.2020.
//

#ifndef TRAMHOLDUP_TTSSDATAPARSER_H
#define TRAMHOLDUP_TTSSDATAPARSER_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <string>
#include "Logger.h"
#include <fstream>
#include <sstream>
#include "rapidjson_pch.h"
#include "DataManager/DataContainer.h"
#include "TTSScorrection.h"
#include "GenericException.h"
#include "Schemas.h"
#include "DataManager/Stop.h"
#include "DataManager/DataAccess.h"

using namespace std;
using namespace rapidjson;

namespace TTSSdata {
    class TTSSdataParser {
    private:
        shared_ptr<spdlog::logger> loggerMain;
        DataManagerStructure::DataContainer *dataContainer;
        DataAccess *dataAccess;

        Document *jsonParseFile(const string &fileName, const string &schemaDoc);

        Document *jsonParseStringStream(string *data, const string &schemaDoc, const string fileName = "");

        void validateJson(Document *doc, const string &schemaDoc, const string fileName = "");

        void deleteDocument(Document *doc);

        void throwParsingError(string fileName, Document *doc);

    public:
        TTSSdataParser(DataManagerStructure::DataContainer *dataContainer, DataAccess *dataAccess);

        void parseStops(const string &filePath);

        void parseLines(const string &filePath);

        void parseLineStops(string *data, Trip *trip);

        void parseLineShapes(string *data, Trip *trip, unsigned directionNum);

        bool parseVehiclePositions(const string &filePath, time_t lastVehicleTime);
    };
}


#endif //TRAMHOLDUP_TTSSDATAPARSER_H
