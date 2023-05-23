FROM ubuntu as builder
RUN apt-get update && apt-get -y install build-essential gfortran wget
RUN mkdir /app
COPY Makefile /app
COPY src /app/src
WORKDIR /app
RUN make

FROM ubuntu
RUN apt-get update && apt-get -y install libgfortran5 gnuplot && apt-get clean
RUN mkdir /app
COPY --from=builder /app/build/iri_edp /app/build/*.dat /app/build/*.asc /app/
WORKDIR /app
CMD [ "/app/iri_edp" ]

