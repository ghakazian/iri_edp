// main.c Main entrypoint file of the IDP_EDP utility which drives the Fortran IRI model to generate an EDP chart for a given date/time and location.

#include<stdio.h>
#include<stdlib.h>
#include "util.h"

char* date=NULL;
IRI_datapoint* data=NULL;
char* pdfName=NULL;
void freeResources() {
    if (date != NULL) {
        free(date);
        date = NULL;
    }
    if (data != NULL) {
        free(data);
        data = NULL;
    }
    if (pdfName != NULL) {
        free(pdfName);
        pdfName = NULL;
    }
}

/// @brief This program drives the IRI Fortran model to produce an electron density profile for a given location and date/time.
/// The tool prompts for the date/time, location, and a desired filename to output an EDP chart in pdf format.
/// @return 0 for successful execution, otherwise -1.
int main() {
    printf("Welcome to the IRI EDP modeling program written by Matthew Brooks.\n");
    printf("\tThis tool demonstrates driving the IRI model Fortran code [http://irimodel.org] from C.\n");

    printf("\nEnter the date/time in format [YYYY-MM-DD HH:MM] where HH:MM is in UTC: ");
    date = read_string();

    // Prompt for the date/time and location information.
    // I have attempted to use a somewhat more friendly format than that employed by the Fortran test program.
    IRI_date iri_date = {0};
    IRI_location iri_location = {0};

    int success = parse_iri_date(date, &iri_date);
    if (!success) {
        freeResources();
        return -1;
    }
    printf("Enter the geographic latitude as a float in the range [-90.0 - 90.0]: ");
    success = read_float(&iri_location.latitude);
    if (!success) {
        freeResources();
        return -1;
    }
    if (iri_location.latitude < -90.0 || iri_location.latitude > 90.0) {
        printf("ERROR: the specified latitude is outside the specified range.\n");
        freeResources();
        return -1;
    }
    printf("Enter the geographic longitude as a float in the range [0.0 - 360.0 degrees East of Prime Meridian]: ");
    success = read_float(&iri_location.longitude);
    if (!success) {
        freeResources();
        return -1;
    }
    if (iri_location.latitude < 0.0 || iri_location.latitude > 360.0) {
        printf("ERROR: the specified longitude is outside the specified range.\n");
        return -1;
    }

    // Call a utility function which handles creating other required inputs of the IRI model
    //   and calling the compiled Fortran code.
    printf("Calling IRI model subroutine iri_web() to calculate EDP data...\n");
    int length;
    IRI_datapoint* data = get_edp_data(iri_date, iri_location, &length);
    if (data == NULL) {
        printf("ERROR: unable to generate EDP profile for the given inputs!\n");
        freeResources();
        return -1;
    }
    
    // Write the resulting data to a temporary file which will be utilized to generate a graph
    char filename[L_tmpnam];
    success = writeDataFile(data, length, filename);
    if (!success) {
        printf("ERROR writing data to temporary file!\n");
        freeResources();
        return -1;
    }
    printf("Data written to file: %s\n", filename);

    // Prompt for the PDF file name to store the graph
    printf("Enter filename to store PDF graph: ");
    pdfName = read_string();

    // Call gnuplot to generate graph
    printf("Generating PDF graph: %s\n", pdfName);
    success = generateEdpPdf(pdfName, filename, date);
    if (!success) {
        printf("ERROR generating PDF plot!\n");
        freeResources();
        return -1;
    }
 
    freeResources();
    return 0;
}