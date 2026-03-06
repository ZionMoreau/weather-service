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
//api key stored as an environment variable on windows because github gave me a warning for hardcoding it
const string API_KEY = std::getenv("OPENWEATHER_API_KEY");

//function to call openweather api and returns the json from the api, or an error if the api call failed
json getWeather(const std::string& location, const bool isZip){
    string url = "http://api.openweathermap.org/data/2.5/weather?";
    //if the user chose to use a zipcode, add the zipcode field to the api call
    if(isZip) {
        url += "zip=" + location;
    }
    //if the user did not use a zip code, then add the city name field to the api call
    else {
        url += "q=" + location;
    }
    //add the api key to the api call
    url += "&appid=" + API_KEY + "&units=imperial";

    //if the api call was successful return the parsed json file
    cpr::Response r = cpr::Get(cpr::Url{url});
    if(r.status_code == 200) {
        return json::parse(r.text);
    }
    //if the call was not successful return the error with the error code and message
    else {
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
    string fileName = jsonRequest["filename"];

    json weather = getWeather(location, isZip);

    ofstream resultFile(fileName);
    std::string resultString = "Failure";
    if (resultFile.is_open()) {
        // Use dump(4) for proper indentation to make JSON file readable
        resultFile << weather.dump(4);
        resultString = "Success";
    }

    resultFile.close();
    sendZMQResponse("Success", socket);
}


int main() {
    //creates the zmq req/rep socket
    zmq::context_t zmqContext(1);
    zmq::socket_t socket(zmqContext, ZMQ_REP);
    socket.bind("tcp://localhost:5000");
    //while true loop to wait for calls from the client
    while (true) {
        try {
            zmq::message_t request;
            zmq::recv_result_t result = socket.recv(request);
            string requestString = request.to_string();

            handleZMQRequest(requestString, socket);
        } catch (const std::exception& e) {
            sendZMQResponse("Failure", socket);
        }
    }

    return 0;
}
