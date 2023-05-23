//  util.h Header file for utility methods and types needed to generate the EDP chart.

#include "irisub.h"

/// @brief Structure used to store date/time information as needed by the IRI model.
typedef struct _IRI_date {
    ft_integer yyyy;
    ft_integer mmdd;
    ft_real hour;
} IRI_date;

/// @brief Structure used to store location information as needed by the IRI model.
typedef struct _IRI_location {
    ft_real latitude;
    ft_real longitude;
} IRI_location;

/// @brief Structure containing the altitude and electron density information as a single data point
typedef struct _IRI_datapoint {
    ft_real altitude;
    ft_real electron_density;
} IRI_datapoint;

/// @brief Read a string from STDIN, allocating a buffer on the heap large enough for the input.
/// @return The string buffer containing the input string. This buffer must be deallocated with free().
char* read_string();

/// @brief Read a float value from STDIN
/// @param value Address of the variable receiving the read-in value
/// @return TRUE if value successfully read in, otherwise FALSE
int read_float(float* value);

/// @brief Parse a date/time string into an IRI date struct
/// @param buffer The string buffer to parse
/// @param date The address of the IRI_date struct to parse the date into
/// @return TRUE if parse is successful, otherwise FALSE
int parse_iri_date(char* buffer, IRI_date *date);

/// @brief Set default options for IRI
/// @param options An array of 50 flags used to configure options on the IRI model
void set_default_options(ft_logical *options);

/// @brief Get electron density profile (EDP) data for the specified date and location.
/// @param IRI_date The IRI date information
/// @param IRI_location The IRI location information
/// @param length Pointer to location to receive length od data, or zero if error
/// @return Pointer to array of datapoints, or NULL if error getting data
IRI_datapoint* get_edp_data(IRI_date date, IRI_location location, int *length);

/// @brief Write electron data profile (EDP) data to a temporary file for gnuplot
/// @param data Pointer to buffer holding EDP data
/// @param length length of datapoints in EDP data
/// @param filename char buffer to receive the temporary file name (must be L_tmpname long)
/// @return true if file was successfully written, otherwise false
bool writeDataFile(IRI_datapoint* data, int length, char *filename);

/// @brief Generate a GNUPLOT chart in PDF format  utilizing the data file written previously.
/// @param pdfFile The path and name of the PDF file to write the chart to.
/// @param dataFile The path and name of the temp file that contains the raw data to chart.
/// @param date The date/time target of this chart.
/// @return True if the chart was successfully created, otherwise false.
bool generateEdpPdf(char* pdfFile, char* dataFile, char* date);