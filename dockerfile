FROM ubuntu
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update
RUN apt upgrade -y
RUN apt install g++ -y
RUN apt install cmake -y
RUN apt install make -y
RUN apt install libboost-all-dev -y
RUN apt install build-essential libtcmalloc-minimal4 -y && ln -s /usr/lib/libtcmalloc_minimal.so.4 /usr/lib/libtcmalloc_minimal.so
#COPY data /data