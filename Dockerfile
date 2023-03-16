# get baseimage
FROM ubuntu:latest

RUN apt-get update -y
RUN apt-get upgrade -y
# reinstall certificates, otherwise git clone command might result in an error
RUN apt-get install --reinstall ca-certificates -y

# install developer dependencies
RUN apt-get install -y git build-essential cmake --no-install-recommends

# install vcpkg package manager
RUN git clone https://github.com/microsoft/vcpkg
RUN apt-get install -y curl zip
RUN vcpkg/bootstrap-vcpkg.sh

# install crow package
RUN /vcpkg/vcpkg install crow

# copy files from local directory to container
COPY . /project

# define working directory from container
WORKDIR /build

# compile with CMake 
RUN bash -c "cmake ../project && cmake --build ."

# run executable (name has to match with CMakeLists.txt file)
CMD [ "./app" ]