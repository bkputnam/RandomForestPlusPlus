
#include <csv_parser/csv_parser.hpp>

/**
 * Example Usage of libcsv_parser++
 *
 * These are some of the characters you may use in this program
 *
 * @li DEC is how it would be represented in decimal form (base 10)
 * @li HEX is how it would be represented in hexadecimal format (base 16)
 *
 * @li	DEC	HEX		Character Name
 * @li	0	0x00	null
 * @li	9	0x09	horizontal tab
 * @li	10	0x0A	line feed, new line
 * @li	13	0x0D	carriage return
 * @li	27	0x1B	escape
 * @li	32	0x20	space
 * @li	33	0x21	double quote
 * @li	39	0x27	single quote
 * @li	44	0x2C	comma
 * @li	92	0x5C	backslash
 */

/**
 * Example Program - showing usage of the csv_parser class
 *
 * In this example, we include the csv_parser header file as <csv_parser/csv_parser.hpp>
 *
 * Then we declare the variables we are going to use in the program
 *
 * @li The name of the input file is "example_input.csv"
 * @li The field terminator is the comma character
 * @li The record terminator is the new line character 0x0A
 * @li The field enclosure character is the double quote.
 *
 * In this example we will be parsing the document as the fields are optionally enclosed.
 *
 * The first record in the CSV file will be skipped.
 *
 * Please view the source code of this file more closely for details.
 *
 * @todo Add more examples using different parsing modes and different enclosure and line terminator characters.
 * @toto Include an example where the filename, field_terminator char, line_terminator char, enclosure_char and other program variables are loaded from a file.
 *
 * @param int The number of arguments passed
 * @param argv The array of arguements passed to the main function
 * @return int The status of the program returned to the operating system upon termination.
 *
 * @see csv_parser
 *
 * @author Israel Ekpo <israel.ekpo@israelekpo.com>
 */
int main(int argc, char ** argv)
{
    // const char * filename = argv[1];
    
    /* Declare the variables to be used */
    const char * filename = "example_input.csv";
    const char field_terminator = ',';
    const char line_terminator  = '\n';
    const char enclosure_char   = '"';
    
    csv_parser file_parser;
    
    /* Define how many records we're gonna skip. This could be used to skip the column definitions. */
    file_parser.set_skip_lines(1);
    
    /* Specify the file to parse */
    file_parser.init(filename);
    
    /* Here we tell the parser how to parse the file */
    file_parser.set_enclosed_char(enclosure_char, ENCLOSURE_OPTIONAL);
    
    file_parser.set_field_term_char(field_terminator);
    
    file_parser.set_line_term_char(line_terminator);
    
    unsigned int row_count = 1U;
    
    /* Check to see if there are more records, then grab each row one at a time */
    while(file_parser.has_more_rows())
    {
        unsigned int i = 0;
        
        /* Get the record */
        csv_row row = file_parser.get_row();
        
        /* Print out each column in the row */
        for (i = 0; i < row.size(); i++)
        {
            printf("COLUMN %02d : %s\n", i + 1U, row[i].c_str());
        }
        
        printf("====================================================================\n");
        printf("END OF ROW %02d\n", row_count);
        printf("====================================================================\n");
        
        row_count++;
    }
    
    return 0;
}
