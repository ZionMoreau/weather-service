Weather Service:

Description:
This microservice communicates with a client program through a ZeroMQ req/rep socket. The service accepts either a zip code or a city name from the client, as well as a boolean whether the given location was a zipcode or not, the final paramater is the name of a the file that the service should
write the weather data to. After recieving this information from the client the service makes a call to the OpenWeather API and writes the current forecast to the file given by the client. 

Request Data:
To request data from the microservice the client should send 3 things through the ZeroMQ req socket in Json format. First they must send a location that they would like to get weather information of (this can either be a city name or a zip code), then the client must
send a boolean (either true or false) that should be true if the location was a zip code and false otherwise, finally the client must send a name of a file that the weather data should be written to, this should be a json file.

Example Call:

socket = context.socket(zmq.REQ)

socket.connect("tcp://localhost:5000")

data = {
                "location": location,
                "isZip": isZip,
                "fileName": "weatherData.json"
                }
              

socket.send_json(data)

Recieve Data:
To recieve data from the microservice the cleint should wait for a response from the service, this response will state if the weather data was gathered successfully or resulted in failure. Then the client should open the file with the name they supplied to parse through the json representing the 
weather data

Example Call:

message = socket.recv_string()

if message == "Success":

data_file = open("weatherData.json", "r")

UML Sequence Diagram:

<img width="920" height="828" alt="Screenshot 2026-02-23 143339" src="https://github.com/user-attachments/assets/942da90e-a771-4068-8c40-f464eff594f6" />

