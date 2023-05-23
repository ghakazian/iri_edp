// util.c Implementation file for utility functions needed to generate EDP chart.

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "util.h"

/// @brief Read a string frin STDIN, allocating a buffer on the heap large enough for the input.
/// @return The string buffer containing the input string. This buffer must be deallocated with free().
char* read_string() {
    char* buffer = 0;
    size_t buffer_size = 0;
 
    ssize_t count = getline(&buffer, &buffer_size, stdin);

    // Get rid of captured trailing newline
    if (buffer[count-1] == '\n') {
        buffer[count-1] = '\0';
    }
    return buffer;
}

/// @brief Read a float value from STDIN
/// @param value Address of the variable receiving the read-in value
/// @return TRUE if value successfully read in, otherwise FALSE
int read_float(float* value) {
    char* buffer = read_string(); 
    if (buffer == NULL) {
        return 0;
    }

    *value = strtod(buffer, NULL);
    return true;
 }

/// @brief Parse a date/time string into an IRI date struct
/// @param buffer The string buffer to parse
/// @param date The address of the IRI_date struct to parse the date into
/// @return TRUE if parse is successful, otherwise FALSE
int parse_iri_date(char* buffer, IRI_date *date) {
    if (date == 0) {
        printf("ERROR: IRI date struct pointer is NULL.\n");
        return 0;
    }
    struct tm tm;
    if (strptime(buffer, "%Y-%m-%d %H:%M", &tm) == 0) {
        printf("ERROR: date/time string is not in the expected format.\n");
        return 0;
    }

    date->yyyy = tm.tm_year + 1900;

    date->mmdd = (tm.tm_mon + 1) * 100;
    date->mmdd += tm.tm_mday;

    date->hour = tm.tm_hour;
    date->hour += tm.tm_min / 60.0;
    return 1;
}

/// @brief Set default options for IRI
/// @param options An array of 50 flags used to configure options on the IRI model
void set_default_options(ft_logical *options) {
    int len = 50;
    for (int i = 0; i < len; i++) {
        options[i] = 1;
    }
    options[3] = 0;
    options[4] = 0;
    options[5] = 0;
    options[20] = 0;
    options[22] = 0;
    options[27] = 0;
    options[28] = 0;
    options[29] = 0;
    options[32] = 0;
    options[34] = 0;
    options[38] = 0;
    options[39] = 0;
    options[46] = 0;
}


/// @brief Get electron density profile (EDP) data for the specified date and location.
/// @param IRI_date The IRI date information
/// @param IRI_location The IRI location information
/// @param length Pointer to location to receive length od data, or zero if error
/// @return Pointer to array of datapoints, or NULL if error getting data
IRI_datapoint* get_edp_data(IRI_date date, IRI_location location, int *length) {
    ft_integer jmag = 0;
    ft_logical jf[50];
    ft_integer iut = 1;
    ft_real height = 80.0;
    ft_real h_tec_max = 0.0;
    ft_integer ivar = 1;
    ft_real vbeg = 80.0;
    ft_real vend = 600.0;
    ft_real vstp = 1.0;
    ft_real *a = malloc(20000 * sizeof(ft_real));
    ft_real *b = malloc(100000 * sizeof(ft_real));
    set_default_options(jf);
    printf("Default options set successfully.\n");

    iri_web_for_c(&jmag, jf, &location.latitude, &location.longitude, &date.yyyy, &date.mmdd, &iut, &date.hour, &height, &h_tec_max, &ivar, &vbeg, &vend, &vstp, a, b);
    printf("Call to Fortran subroutine iri_web completed successfully.\n");

    int dataPoints = ((vend - vbeg) / vstp) + 1;
    IRI_datapoint* data = malloc(dataPoints * sizeof(IRI_datapoint));
    if (data == NULL) {
        *length = 0;
        return NULL;
    }
    *length = dataPoints;

    ft_real altitude = vbeg;
    int index = 0;
    int a_index = 0;
    ft_real edp;
    while (altitude <= vend) {
        data[index].altitude = altitude;
        data[index].electron_density = a[a_index];
        a_index += 20;
        index++;
        altitude += vstp;
    }
    return data;
}

/// @brief Write electron data profile (EDP) data to a temporary file for gnuplot
/// @param data Pointer to buffer holding EDP data
/// @param length length of datapoints in EDP data
/// @param filename char buffer to receive the temporary file name (must be L_tmpname long)
/// @return true if file was successfully written, otherwise false
bool writeDataFile(IRI_datapoint* data, int length, char *filename) {
    filename = tmpnam(filename);
    if (filename == NULL) {
        return false;
    }

    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        return false;
    }

    for (int i=0; i < length; i++) {
        fprintf(fp, "%f\t%f\n", data[i].altitude, data[i].electron_density);
    }

    fclose(fp);
    return true;
}



/// @brief Generate a GNUPLOT chart in PDF format  utilizing the data file written previously.
/// @param pdfFile The path and name of the PDF file to write the chart to.
/// @param dataFile The path and name of the temp file that contains the raw data to chart.
/// @param date The date/time target of this chart.
/// @return True if the chart was successfully created, otherwise false.
bool generateEdpPdf(char* pdfFile, char* dataFile, char* date) {
    FILE* pipe = popen("gnuplot", "w");
    if (pipe == NULL) {
        return false;
    }
 
    fprintf(pipe, "set title 'Electron Density Profile for %s' \n", date);
    fprintf(pipe, "set ylabel 'Altitude (km)' \n");
    fprintf(pipe, "set xlabel 'Electron Density (e/m3)' \n");
    fprintf(pipe, "set terminal pdf \n");
    fprintf(pipe, "set output '%s' \n", pdfFile);
    fprintf(pipe, "plot '%s' using 2:1 with lines title ''\n", dataFile);

    fflush(pipe);
    fclose(pipe);
    pipe = NULL;
    return true;
}