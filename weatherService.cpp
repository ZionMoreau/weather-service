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


int main() {
    //creates the zmq req/rep socket
    zmq::context_t zmqContext(1);
    zmq::socket_t socket(zmqContext, ZMQ_REP);
    socket.bind("tcp://localhost:5000");
    //while true loop to wait for calls from the client
    while (true) {
        zmq::message_t request;
        zmq::recv_result_t result = socket.recv(request);
        string requestString = request.to_string();

        //try catch to catch any exceptions
        try {
            //parsing request from the client for a string of the location, a boolean that determines if the location is a zip code or not
            // and a string that determines the file name that the results will be written to
            json jsonRequest = json::parse(requestString);
            string location = jsonRequest["location"];
            bool isZip = jsonRequest["isZip"];
            string fileName = jsonRequest["fileName"];

            //calls the get weather function and stores the results to a json that will be printed to the file.
            json weather = getWeather(location, isZip);

            //opens a file with the name specified by the client
            ofstream resultFile(fileName);
            //if the file opened properly print the weather data to it and send Success to the client over the zeroMQ socket then close the file
            if (resultFile.is_open()) {
                //uses .dump(4) for proper indentation to make the json file readable
                resultFile << weather.dump(4);
                resultFile.close();
                string response = "Success";
                zmq::message_t reply(response.size());
                memcpy(reply.data(), response.data(), response.size());
                socket.send(reply);
            }
            //if the file did not open properly. close it and send Failure to the zeroMQ socket
            else {
                resultFile.close();
                string response = "Failure";
                zmq::message_t reply(response.size());
                memcpy(reply.data(), response.data(), response.size());
                socket.send(reply);
            }
        //if an exception was thrown send Failure to the cliend through the ZeroMQ socket
        } catch (const std::exception& e){
            string response = "Failure";
            zmq::message_t reply(response.size());
            memcpy(reply.data(), response.data(), response.size());
            socket.send(reply);
        }

    }

    return 0;
}
