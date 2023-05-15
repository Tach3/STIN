# Set the base image
FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    g++ \
    libboost-all-dev \
    libcurl4-openssl-dev \
    build-essential \
    libasio-dev



# Set working directory
WORKDIR /app

# Copy application files to the container
COPY . .

# Build the application
RUN g++ -I/usr/include/curl/ main.cpp -o main -lcurl

# Set the default command to run the application
CMD ["./main"]

# Expose the port that the application is running on
EXPOSE 18080


#docker build -t crow:2.0 .
#docker run -d -p 80:18080 -e USRNM=%USRNM% -e PSSWD=%PSSWD% --name webserver crow:2.0