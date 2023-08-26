FROM ubuntu:focal

WORKDIR /
RUN apt-get update && apt-get install libapr1 libapr1-dev -y && apt-get install libgomp1 -y
RUN mkdir /Results
COPY binary-trees .
