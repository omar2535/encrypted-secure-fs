FROM ubuntu:22.04

LABEL maintainer="omar2535@alumni.ubc.ca"
LABEL version="1.0"
LABEL description="Custom dockerfile for encrypted secure filesystem build"

RUN apt update
RUN apt upgrade -y

RUN apt-get update && apt-get install -y build-essential git cmake libssl-dev


# Project setup
RUN mkdir -p /efs
WORKDIR /efs
COPY . /efs

RUN git submodule update --init --recursive
RUN mkdir -p /efs/build
RUN cmake . -B build
RUN cmake --build build


# Infinite entrypoint
ENTRYPOINT ["tail", "-f", "/dev/null"]
