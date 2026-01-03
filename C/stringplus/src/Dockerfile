FROM alpine:latest
LABEL dev="rootpokedaemon@gmail.com"

RUN apk add --no-cache build-base bash git gcompat
RUN apk add --no-cache check
RUN apk add check-dev
RUN apk add lcov gzip

WORKDIR /school
COPY . /school

