//
// Created by Zdmor on 2/18/2026.
//

#include "weatherService.h"
#include <iostream>
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

using json = nlohmann::json;
using namespace std;
const string API_KEY = "333fd08087c5187d1b35db0ecdedc8a4";

json getWeather(string location, bool isZip) {
    string url = "http://api.openweathermap.org/data/2.5/weather?";

    if(isZip) {
        url += "zip=" + location;
    }
    else {
        url += "q=" + location;
    }
    url += "&appid=" + API_KEY + "&units=imperial";

    cpr::Response r = cpr::Get(cpr::Url{url});
    if(r.status_code == 200) {
        return json::parse(r.text);
    }
    else {
        return {
            {"error", "Failed to retrieve weather data"},
            {"status_code", r.status_code},
            {"status_text", r.text}
        };

    }

}


int main() {
    return 0;
}
