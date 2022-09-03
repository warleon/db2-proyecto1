FROM ubuntu
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update
RUN apt upgrade -y
RUN apt install g++ -y
RUN apt install cmake -y
RUN apt install make -y
#COPY data /data