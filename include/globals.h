#ifndef globals_h
#define globals_h

    #include <Arduino.h>
    #include <ArduinoJson.h>
    #include <StreamUtils.h>
    #include <map>
    #include <WiFi.h>
    #include <WiFiAP.h>
    #include <sstream>
    #include <iostream>
    #include <servicesDef.h>

    typedef String (*SDLEventFunction)(const StaticJsonDocument<512> &doc);
    std::map<int,SDLEventFunction> functions;
    std::map<String,String> functionalitiesParameters;

    EepromStream eepromStream(0, 700);
    StaticJsonDocument<700> globaljson;

    void loadLastSectionConfig(){
        if(EEPROM.begin(1024)){
            DeserializationError error = deserializeJson(globaljson, eepromStream);
            //if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                std::stringstream functionalities;
                functionalities << " {" ;
                for(std::map<String,String>::iterator iter = functionalitiesParameters.begin(); iter != functionalitiesParameters.end(); ++iter)
                    functionalities << "\"" << iter->first.c_str() << "\":" << functionalitiesParameters[iter->first.c_str()].c_str() << ",";
                functionalities.seekp(-1, std::ios_base::end);
                functionalities.seekp(-1, std::ios_base::end);
                functionalities << "}";
                deserializeJson(globaljson, functionalities.str().c_str());
                serializeJson(globaljson, eepromStream);
                eepromStream.flush();
            //}
            std::cout << globaljson.as<String>().c_str() << "\n";
        }
    }

    // void saveJsonEEpromIfNeeded(){
    //     if(globaljson.as<String>()){

    //     }
        
    // }

    // JsonVariant findNestedKey(JsonObject obj, const char* key) {
    //     JsonVariant foundObject = obj[key];
    //     if (!foundObject.isNull())
    //         return foundObject;

    //     for (JsonPair pair : obj) {
    //         JsonVariant nestedObject = findNestedKey(pair.value(), key);
    //         if (!nestedObject.isNull())
    //             return nestedObject;
    //     }

    //     return JsonVariant();
    // }


    void updateLastSectionIfDifferent(const StaticJsonDocument<512> &doc){
        JsonObject root = globaljson.as<JsonObject>();
        for (JsonPair kv : root) {
            kv.key().c_str();
            JsonObject foundNestedObject = kv.value();
            if(uint16_t(foundNestedObject["op"]) == uint16_t(doc["op"])){
                for (JsonPair _kv : foundNestedObject) {
                    foundNestedObject[_kv.key()] = doc[_kv.key()];
                }
                serializeJson(globaljson, eepromStream);
                eepromStream.flush(); 
                return;
            }
            
        }
        //std::cout << globaljson.as<String>() << "\n";
    }

#endif