# Set the base image
FROM ubuntu:latest

# Install required packages
RUN apt-get update
RUN apt-get install -y \
    g++ \
    libboost-all-dev \
    libcurl4-openssl-dev \
    build-essential \
    libasio-dev



# Set working directory
WORKDIR /app

# Copy application files to the container
COPY . .

CD crow

# Build the application
RUN g++ -I/usr/include/curl/ main.cpp -o main -lcurl

# Set the default command to run the application
CMD ["./main"]

# Expose the port that the application is running on
EXPOSE 18080
