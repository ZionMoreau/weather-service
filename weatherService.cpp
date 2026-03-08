//
// Created by Zdmor on 2/18/2026.
//

#include "weatherService.h"
#include <iostream>
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <fstream>

using json = nlohmann::json;
using namespace std;
const char* env_key = std::getenv("OPENWEATHER_API_KEY");
const string API_KEY = (env_key != nullptr) ? string(env_key) : "";

//fetches weather data from openweather api. Returns results in JSON containing data or error
json getWeather(const std::string& location, const bool isZip){
    string url = "http://api.openweathermap.org/data/2.5/weather?";
    //if the user chose to use a zipcode, add the zipcode field to the api call
    if (isZip) {
        url += "zip=" + location;
    } else {
        url += "q=" + location;
    }
    //add the api key to the api call
    url += "&appid=" + API_KEY + "&units=imperial";

    //if the api call was successful return the parsed json file
    cpr::Response r = cpr::Get(cpr::Url{url});
    if (r.status_code == 200) {
        return json::parse(r.text);
    } else {
        return {
            {"error", "Failed to retrieve weather data"},
            {"status_code", r.status_code},
            {"status_text", r.text}
        };
    }
}

inline void sendZMQResponse(const std::string& response, zmq::socket_t& socket) {
    zmq::message_t reply(response.begin(), response.end());
    socket.send(reply);
}

/* Parse the JSON request from the client for Location Data,
 * filename, and properly respond to request
 */
void handleZMQRequest(const std::string& request, zmq::socket_t& socket) {
    json jsonRequest = json::parse(request);

    std::string location = jsonRequest["location"];
    bool isZip = jsonRequest["isZip"];
    string fileName = jsonRequest["fileName"];

    json weather = getWeather(location, isZip);

    ofstream resultFile(fileName);
    std::string resultString = "Failure";
    if (resultFile.is_open()) {
        // Use dump(4) for proper indentation to make JSON file readable
        resultFile << weather.dump(4);
        resultString = "Success";
    }

    resultFile.close();
    sendZMQResponse(resultString, socket);
}

int main() {
    zmq::context_t zmqContext(1);
    zmq::socket_t socket(zmqContext, ZMQ_REP);
    socket.bind("tcp://localhost:5000");
    while (true) {
        //try catch to catch any exceptions
        try {
            zmq::message_t request;
            zmq::recv_result_t result = socket.recv(request);
            string requestString = request.to_string();

            handleZMQRequest(requestString, socket);
        //if an exception was thrown send Failure to the client through the ZeroMQ socket
        } catch (const std::exception& e) {
            sendZMQResponse("Failure", socket);
        }
    }

    return 0;
}
