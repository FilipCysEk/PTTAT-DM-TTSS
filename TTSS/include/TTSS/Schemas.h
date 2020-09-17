//
// Created by krzysiu on 09.07.2020.
//

#ifndef TRAMHOLDUP_SCHEMAS_H
#define TRAMHOLDUP_SCHEMAS_H

#include <string>

using namespace std;

namespace TTSSdata {
    static const string stopsSchema = R"json(
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "type": "object",
    "properties": {
        "stops": {
            "type": "array",
            "items": {
                "type": "object"
            },
            "patternProperties": {
                "^.*$": {
                    "type":"array",
                    "items": {
                        "type": "array",
                        "description": "Latitude and longitude",
                        "items": {
                            "type": "number"
                        },
                        "minItems": 2,
                        "maxItems": 2
                    }
                }
            }

        }
    },
    "required": [ "stops" ]
}
            )json";

    static const string linesSchema = R"json({
  "$schema": "http://json-schema.org/draft-07/schema#",
  "type": "object",
  "required": [
    "routes"
  ],
  "properties": {
    "routes": {
      "type": "array",
      "items": {
        "type": "object",
        "required": [
          "id",
          "name",
          "authority"
        ],
        "properties": {
          "id": {
            "type": "string",
            "description": "Id of line"
          },
          "name": {
            "type": "string",
            "description": "Number of line"
          },
          "directions": {
            "type": "array",
            "items": {
              "type": "string"
            }
          },
          "authority": {
            "type": "string"
          }
        }
      }
    }
  }
}
            )json";

    static const string lineStopsSchema = R"json({
  "$schema": "http://json-schema.org/draft-07/schema#",
  "type": "object",
  "required": [
    "route",
    "stops"
  ],
  "properties": {
    "route": {
      "type": "object",
      "required": [
        "id",
        "name",
        "directions",
        "authority"
      ],
      "properties": {
        "id": {
          "type": "string",
          "description": "Id of line"
        },
        "name": {
          "type": "string",
          "description": "Number of line"
        },
        "directions": {
          "type": "array",
          "items": {
            "type": "string"
          }
        },
        "authority": {
          "type": "string"
        }
      }
    },
    "stops": {
      "type": "array",
      "items": {
        "type": "object",
        "required": ["id", "name", "number"],
        "properties": {
          "id": {
            "type": "string"
          },
          "name": {
            "type": "string"
          },
          "number": {
            "type": "string"
          }
        }
      }
    }
  }
}
            )json";

    static const string lineShapeSchema = R"json({
  "$schema": "http://json-schema.org/draft-07/schema#",
  "type": "object",
  "required": [
    "paths"
  ],
  "properties": {
    "paths": {
      "type": "array",
      "items": {
        "type": "object",
        "required": [
          "wayPoints"
        ],
        "properties": {
          "wayPoints": {
            "type": "array",
            "items": {
              "type": "object",
              "required": [
                "lat",
                "lon",
                "seq"
              ],
              "properties": {
                "lat": {
                  "type": "number"
                },
                "lon": {
                  "type": "number"
                },
                "seq": {
                  "type": "string"
                }
              }
            }
          }
        }
      }
    }
  }
}
)json";

    static const string vehiclePositionsSchema = R"json(
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "type": "object",
  "required": ["lastUpdate", "vehicles"],
  "properties": {
    "lastUpdate": {
      "type": "number"
    },
    "vehicles": {
      "type": "array",
      "items": {
        "type": "object",
        "properties": {
          "latitude": {
            "type": "number"
          },
          "longitude": {
            "type": "number"
          },
          "name": {
            "type": "string"
          },
          "isDeleted": {
            "type": "boolean"
          }
        }
      }
    }
  }
}
)json";
}

#endif //TRAMHOLDUP_SCHEMAS_H
