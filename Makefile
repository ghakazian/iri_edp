# Makefile used to build the iri_edp utility

OUTPUT_PATH=build
F77_COMPILER=gfortran -std=legacy
C_COMPILER=cc
WGET=wget
LINKER=ld
COMMON_FILES_URL=https://irimodel.org/COMMON_FILES/00_ccir-ursi.tar
IG_RZ_DAT_URL=https://irimodel.org/indices/ig_rz.dat
APF107_DAT_URL=https://irimodel.org/indices/apf107.dat
object_files=${OUTPUT_PATH}/cira.o ${OUTPUT_PATH}/igrf.o ${OUTPUT_PATH}/iridreg.o ${OUTPUT_PATH}/iriflip.o ${OUTPUT_PATH}/irifun.o \
	${OUTPUT_PATH}/irisub.o ${OUTPUT_PATH}/iritec.o ${OUTPUT_PATH}/irisubc.o

$(shell mkdir -p ${OUTPUT_PATH})

all: ${OUTPUT_PATH}/iri_edp ${OUTPUT_PATH}/iritest common_files src_data ${OUTPUT_PATH}/apf107.dat ${OUTPUT_PATH}/ig_rz.dat

${OUTPUT_PATH}/cira.o: src/iri_2016/cira.for
	${F77_COMPILER} -c src/iri_2016/cira.for -o ${OUTPUT_PATH}/cira.o

${OUTPUT_PATH}/igrf.o: src/iri_2016/igrf.for
	${F77_COMPILER} -c src/iri_2016/igrf.for -o ${OUTPUT_PATH}/igrf.o

${OUTPUT_PATH}/iridreg.o: src/iri_2016/iridreg.for
	${F77_COMPILER} -c src/iri_2016/iridreg.for -o ${OUTPUT_PATH}/iridreg.o

${OUTPUT_PATH}/iriflip.o: src/iri_2016/iriflip.for
	${F77_COMPILER} -c src/iri_2016/iriflip.for -o ${OUTPUT_PATH}/iriflip.o

${OUTPUT_PATH}/irifun.o: src/iri_2016/irifun.for
	${F77_COMPILER} -c src/iri_2016/irifun.for -o ${OUTPUT_PATH}/irifun.o

${OUTPUT_PATH}/irisub.o: src/iri_2016/irisub.for
	${F77_COMPILER} -c src/iri_2016/irisub.for -o ${OUTPUT_PATH}/irisub.o

${OUTPUT_PATH}/iritec.o: src/iri_2016/iritec.for
	${F77_COMPILER} -c src/iri_2016/iritec.for -o ${OUTPUT_PATH}/iritec.o

${OUTPUT_PATH}/irisubc.o: src/irisubc.for
	${F77_COMPILER} -c src/irisubc.for -o ${OUTPUT_PATH}/irisubc.o

${OUTPUT_PATH}/util.o: src/util.c src/util.h
	${C_COMPILER} -c src/util.c -o ${OUTPUT_PATH}/util.o

${OUTPUT_PATH}/main.o: src/main.c src/util.h
	${C_COMPILER} -c src/main.c -o ${OUTPUT_PATH}/main.o

${OUTPUT_PATH}/iri_edp: ${object_files} ${OUTPUT_PATH}/main.o ${OUTPUT_PATH}/util.o
	${F77_COMPILER} ${object_files} ${OUTPUT_PATH}/main.o ${OUTPUT_PATH}/util.o -o ${OUTPUT_PATH}/iri_edp

${OUTPUT_PATH}/iritest: ${object_files} src/iri_2016/iritest.for
	${F77_COMPILER} ${object_files} src/iri_2016/iritest.for -o ${OUTPUT_PATH}/iritest

${OUTPUT_PATH}/common_files.tar:
	${WGET} ${COMMON_FILES_URL} -O ${OUTPUT_PATH}/common_files.tar

${OUTPUT_PATH}/ig_rz.dat:
	${WGET} ${IG_RZ_DAT_URL} -O ${OUTPUT_PATH}/ig_rz.dat

${OUTPUT_PATH}/apf107.dat:
	${WGET} ${APF107_DAT_URL} -O ${OUTPUT_PATH}/apf107.dat

src_data:
	cp src/iri_2016/*.dat ${OUTPUT_PATH}

common_files: ${OUTPUT_PATH}/common_files.tar
	tar -xvf ${OUTPUT_PATH}/common_files.tar -C ${OUTPUT_PATH}

clean:
	rm -rf ${OUTPUT_PATH}

docker:
	docker build -t iri .

docker-test: docker
	docker run --mount type=bind,source=$(shell pwd)/build,target=/data -i iri < input1.txt
	docker run --mount type=bind,source=$(shell pwd)/build,target=/data -i iri < input2.txt

.PHONY: all
.PHONY: common_files
.PHONY: src_data
.PHONY: docker
.PHONY: docker-test

