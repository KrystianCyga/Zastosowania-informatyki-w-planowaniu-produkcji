FROM gcc:latest

RUN apt-get update && apt-get install -y \
    cmake \
    libboost-system-dev \
    libboost-thread-dev \
    git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Install cpp-httplib
RUN git clone https://github.com/yhirose/cpp-httplib.git \
    && cp cpp-httplib/httplib.h /usr/local/include/ \
    && rm -rf cpp-httplib

# Install nlohmann/json
RUN git clone https://github.com/nlohmann/json.git \
    && mkdir -p /usr/local/include/nlohmann \
    && cp json/single_include/nlohmann/json.hpp /usr/local/include/nlohmann/ \
    && rm -rf json

COPY . .

RUN g++ -std=c++17 -o server main.cpp -pthread -lboost_system -lboost_thread -I/usr/local/include

EXPOSE 8080

CMD ["./server"]