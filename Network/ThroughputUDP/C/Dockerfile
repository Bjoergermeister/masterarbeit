FROM ubuntu:focal

WORKDIR /
RUN mkdir /Results
RUN apt update && apt install iproute2 -y && apt clean
COPY ./client .