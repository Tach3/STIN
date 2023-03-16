FROM gcc:latest

RUN apt-get update && apt-get upgrade -y && apt-get autoremove -y

# Install necessary dependencies
RUN apt-get update && \
    apt-get install -y cmake && \
    apt-get install -y libboost-all-dev && \
    apt-get install -y git

# Clone and build Crow
RUN git clone https://github.com/ipkn/crow.git && \
    cd crow && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

# Copy your source code into the container
COPY main.cpp /app/main.cpp

# Compile your source code
WORKDIR /app
RUN g++ -std=c++11 -I/usr/local/include main.cpp -o main -L/usr/local/lib -lboost_system -lboost_thread -lboost_chrono -lpthread

# Expose the port your app listens on
EXPOSE 18080

# Start your app when the container starts
CMD ["./main"]
