//
// Created by krzysiu on 09.07.2020.
//

#include "TTSS/TTSSdataParser.h"

TTSSdata::TTSSdataParser::TTSSdataParser(DataManagerStructure::DataContainer *dataContainer, DataAccess *dataAccess)
        : dataContainer(dataContainer), dataAccess(dataAccess) {
    loggerMain = Logger::getLogger("loggerMain");
}

Document *TTSSdata::TTSSdataParser::jsonParseFile(const string &fileName, const string &schemaDoc) {
    ifstream json;
    json.open(fileName);

    if (json.is_open()) {
        IStreamWrapper isw(json);
        auto *doc = new Document();
        doc->ParseStream(isw);

        validateJson(doc, schemaDoc, fileName);

        json.close();
        return doc;
    } else {
        loggerMain->error("Can't open file: {0}", fileName);
        throwParsingError(fileName + " can't open file!", nullptr);
        return nullptr;
    }
}

Document *TTSSdata::TTSSdataParser::jsonParseStringStream(string *data, const string &schemaDoc,
                                                          const string fileName) {
    auto *doc = new Document();
    doc->Parse(*data);

    validateJson(doc, schemaDoc, fileName);

    return doc;
}

void TTSSdata::TTSSdataParser::validateJson(Document *doc, const string &schemaDoc, const string fileName) {
    Document schemaDocument;
    schemaDocument.Parse(schemaDoc);
    SchemaDocument schema(schemaDocument);

    SchemaValidator validator(schema);
    if (!doc->Accept(validator)) {
        StringBuffer sb;
        validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
        loggerMain->error("Error file {1} is incorrect. Schema error: \"{0}\" ({2}, {3})",
                          sb.GetString(), fileName, validator.GetInvalidSchemaKeyword(),
                          validator.GetInvalidSchemaPointer().GetParseErrorOffset());
        throwParsingError(fileName, doc);
    }
}

void TTSSdata::TTSSdataParser::throwParsingError(string fileName, Document *doc) {
    loggerMain->error("Error on parsing downloaded {0}!", fileName);
    deleteDocument(doc);
    throw GenericException("Error on parsing downloaded json file:" + fileName, GenericException::ERROR_TRY_REPEAT);
}

void TTSSdata::TTSSdataParser::deleteDocument(Document *doc) {
    doc->SetObject();
    delete doc;
}

void TTSSdata::TTSSdataParser::parseLines(const string &filePath) {
    loggerMain->trace("Start parsing routes.");
    Document *doc = jsonParseFile(filePath, linesSchema);

    for (auto &stopIt : (*doc)["routes"].GetArray()) {
        auto lineIt = stopIt.GetObject();
        string authority = lineIt["authority"].GetString();
        if (authority == "MPK") {
            if (lineIt.HasMember("directions")) {
                DataManagerStructure::Line *linePtr = dataContainer->addLine(DataManagerStructure::Line(
                        lineIt["id"].GetString(),
                        lineIt["name"].GetString()
                ));

                for (int directionId = 0; directionId < lineIt["directions"].GetArray().Size(); ++directionId) {
                    string tripId = lineIt["name"].GetString();
                    tripId += lineIt["directions"].GetArray()[directionId].GetString();
                    linePtr->tripList.emplace_back(
                            DataManagerStructure::Trip(tripId, lineIt["directions"].GetArray()[directionId].GetString(),
                                                       linePtr));
                }
            }
        }
    }
    deleteDocument(doc);
    loggerMain->trace("End parsing routes.");
}

void TTSSdata::TTSSdataParser::parseStops(const string &filePath) {
    loggerMain->trace("Start parsing stops.");
    Document *doc = jsonParseFile(filePath, stopsSchema);

    for (auto &stopIt : (*doc)["stops"].GetArray()) {
        DataManagerStructure::Stop stop(
                stopIt["id"].GetString(),
                stopIt["name"].GetString(),
                DataManagerStructure::Position(stopIt["latitude"].GetInt(), stopIt["longitude"].GetInt())
        );

        dataContainer->addStop(stop);
    }
    deleteDocument(doc);
    loggerMain->trace("End parsing stops.");
}


void TTSSdata::TTSSdataParser::parseLineStops(string *data, Trip *trip) {
    Document *doc = jsonParseStringStream(data, lineStopsSchema, "stringstream(Line stops)");

    DataManagerStructure::Stop *findStop;
    for (auto &stopIt : (*doc)["stops"].GetArray()) {
        findStop = dataAccess->getStopById(stopIt["id"].GetString());

        if (findStop == nullptr) {
            loggerMain->warn("Not found stop with Id: {0}. It has name: {1}. Skipped!",
                             stopIt["id"].GetString(), stopIt["name"].GetString());
        }

        trip->stopList.push_back(findStop);
    }
    deleteDocument(doc);
}

void TTSSdata::TTSSdataParser::parseLineShapes(string *data, Trip *trip, unsigned directionNum) {
    Document *doc = jsonParseStringStream(data, lineShapeSchema, "stringstream(Line shape)");

    if ((*doc)["paths"].GetArray().Size() < directionNum) {
        loggerMain->error("Passed direction num is grater than shape directions. Skip shapes for this line");
        return;
    }

    for (auto &shapeIt : (*doc)["paths"].GetArray()[directionNum]["wayPoints"].GetArray()) {
        trip->shape.emplace_back(DataManagerStructure::Position(locationCorrect(shapeIt["lat"].GetInt()),
                                                                locationCorrect(shapeIt["lon"].GetInt())));
    }

    deleteDocument(doc);
}

bool TTSSdata::TTSSdataParser::parseVehiclePositions(const string &filePath, time_t lastVehicleTime) {
    Document *doc = jsonParseFile(filePath, vehiclePositionsSchema);
    string vName, lineNumber, lineDirection, vehId;
    DataManagerStructure::Trip *tripPtr;
    time_t lastUpdate = (((*doc)["lastUpdate"].GetInt64()) / 1000);
    loggerMain->trace("LastUpdate time {0}", ctime(&lastUpdate));

    if (lastUpdate <= lastVehicleTime) {
        deleteDocument(doc);
        return false;
    }

    vector<DataManagerStructure::VehicleData> vehicleList;

    for (auto &veh: (*doc)["vehicles"].GetArray()) {
        if (veh.HasMember("isDeleted")) {
            if (veh["isDeleted"].GetBool()) {
                continue;
            }
        }

        vName = veh["name"].GetString();
        int i = 0;
        lineNumber = string();
        for (; i < vName.size(); ++i) {
            if (vName[i] >= '0' && vName[i] <= '9') {
                lineNumber += vName[i];
            } else {
                break;
            }
        }

        ++i;

        lineDirection = string();
        for (; i < vName.size(); ++i) {
            lineDirection += vName[i];
        }

        tripPtr = dataAccess->getTrip(lineNumber, lineDirection);

        vehId = veh["id"].GetString();

        if (tripPtr == nullptr) {
            loggerMain->warn("Vehicle {0} not found line. Has coded: {1}", vehId, vName);
        }

        vehicleList.emplace_back(DataManagerStructure::VehicleData(chrono::system_clock::from_time_t(lastUpdate), vehId,
                                                                   DataManagerStructure::Position(
                                                                           locationCorrect(veh["latitude"].GetInt()),
                                                                           locationCorrect(veh["longitude"].GetInt())),
                                                                   tripPtr));
    }

    dataContainer->addVehicleList(vehicleList, 0);

    deleteDocument(doc);
    return true;
}




