# Set the base image
FROM ubuntu:latest

# Install required packages
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    libboost-all-dev \
    libssl-dev \
    libcrypto++-dev

# Set working directory
WORKDIR /app

# Copy application files to the container
COPY . .

# Build the application
RUN cmake .
RUN make

# Set the default command to run the application
CMD ["./app"]

# Expose the port that the application is running on
EXPOSE 18080
