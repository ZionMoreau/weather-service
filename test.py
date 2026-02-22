import zmq


def test_weatherservice():
    
    context = zmq.Context()

    # Socket to talk to server
    print("Connecting to weather microservice...")
    socket = context.socket(zmq.REQ)
    socket.connect("tcp://localhost:5000")

    while True:
        choice = input("Please enter 1 to submit a location, or 2 to exit: ")
        if choice == "1":
            location = input("Please enter a location: ")
            if isinstance(location, int):
                isZip = True
            else:
                isZip = False

            # Send a request 
            data = {
                "location": location,
                "isZip": isZip,
                "fileName": "weatherData.json"
                }
            print(f"Requesting weather data for your location.")
            socket.send_json(data)

            # Get the reply
            message = socket.recv_string()
            print(f"Received reply: {message}")

        elif choice == "2":
            print("Exiting...")
            break
        
        else:
            print("Error: Please enter either 1 or 2.")

    # Close socket and terminate context
    socket.close()
    context.term()

if __name__ == "__main__":
    test_weatherservice()
