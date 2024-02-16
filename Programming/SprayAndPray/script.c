/* THIS IS THE SCRIPT FOR SPRAY_AND_PRAY PROBLEM. WE ARE TO CALCULATE THE RADIUS OF MULTIPLE CIRCLES WITHIN A 4X4 GRID GIVEN THE TOTAL AREA AND A UNIFORM DISTRIBUTION.
HERE'S WHAT WE ARE GIVEN:
- A UNIFORM DISTRIBUTION OF DATA ACROSS THE WHOLE TARGET IN "DATA.CSV"
- TOTAL AREA OF THE SQUARE IS 4X4 = 16 UNITS
WE WILL NEED TO PARSE THE "DATA.CSV" FILE AND RECORD HOW MANY TIMES WE SEE EACH NUMBER. THEN WE WILL PERFORM  SOME SIMPLE ALGEBRA:
- 16 (TOTAL AREA) / TOTAL_DISTRIBUTION = AREA_PER_POINT
- SPECIFIC_DISTRIBUTION * AREA_PER_POINT = SPECIFIC_AREA
- (SPECIFIC_AREA/(PI))^(1/2) = SPECIFIC_RADIUS

THIS IS NOTHING MORE THAN FIRST READING INTO A DYNAMIC ARRAY, THEN SCANNING AND INCREMENTING THE CORRESPONDING NUMBERS, THEN EXECUTING THE ALEGEBRA STATED ABOVE.

REFERENCE:
    -   https://www.youtube.com/watch?v=W3dtyZr8rcY&t=948s
    - https://man7.org/linux/man-pages/man3/strtok_r.3.html
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include<ctype.h>
#include "header.h"

/* THIS FUNCTION IS USED TO PARSE EACH TOKEN FROM THE FILE. MORE INFORMATION AT DEFINITION*/
int parseValidToken(char *token);

/******************************************************************************************************************************************************
 * main()
 * 
 * 4 MAIN SECTIONS:
 * 1. READ INTO MEMORY:
 *  - THE .csv  FILE HAS ONLY ONE LINE OF DATA AS STATED IN THE README. WE WILL USE GETLINE()
 * 2. TOKENIZE THE DATA:
 *  - EACH BYTE OF DATA WILL BE TOKENIZED AND A CORRESPONDING INTEGER WILL BE INCREMENTED FOR EACH VALUE FOUND
 *  - EACH TOKEN IS PROPERLY PARSED VIA parseValidToken() READ MORE ABOUT IT AT ITS DEFINITION
 * 3. CALCULATIONS:
 *  - ALL CALCULATIONS MADE REGARDING THE DATA ARE DONE HERE. FORMULAS FOLLOW THOSE STATED ABOVE.
 * 4. FREE MEMORY:
 *  - BUFFERS, AND MEMORY GETS FREED AS PER STANDARD PRACTICE
*******************************************************************************************************************************************************/
int main(void) {

    /* SECTION 1: READ INTO MEMORY*/
    FILE *fp = NULL;
    char *lineptr = NULL;
    char *token = NULL;
    int buffer = 0;
    int line_size;
    const char delimiter = ',';

    if ((fp = fopen(FILE_NAME, "r")) == NULL) {
        perror("Error opening the file...");
        exit(1);
    }
    if ((line_size = getline(&lineptr, &buffer, fp)) == -1){
        perror("Error reading data from file, failure to read line");
        exit(2);
    }


    /* SECTION 2: TOKENIZE THE DATA*/
    int parsed_token;
    int total_distribution, five_distribution, four_distribution, three_distribution, two_distribution;
    double area_per_point, five_area, four_area, three_area, two_area;
    double five_radius, four_radius, three_radius;
    
    total_distribution = five_distribution = four_distribution = three_distribution = two_distribution = 0;

    token  = strtok(lineptr, &delimiter);
    // printf("Token #1 %d\n", atoi(token));
    while (token != NULL) {
        parsed_token = parseValidToken(token); 
        switch (parsed_token){
            case 2: 
                two_distribution++;
                break;
            case 3:
                three_distribution++;
                break;
            case 4:
                four_distribution++;
                break;
            case 5:
                five_distribution++;
                break;
            default:
                printf("Invalid data with: %d\n", parsed_token);
                printf("Please input values only from %d through %d\n", MIN_ACCEPTABLE_VALUE, MAX_ACCEPTABLE_VALUE);
                exit(3);
        }

        token = strtok(NULL, &delimiter);
    }
    total_distribution = five_distribution + four_distribution + three_distribution + two_distribution;
    // PRINT TOTALS
    printf("Total: %d, Distributions - 5: %d, 4: %d, 3: %d, 2: %d\n", total_distribution, five_distribution, four_distribution, three_distribution, two_distribution);

    /* SECTION 3: CALCULATIONS*/
    area_per_point = TOTAL_AREA/total_distribution;
    five_area = five_distribution * area_per_point;
    four_area = (five_distribution + four_distribution) * area_per_point;
    three_area = (five_distribution + four_distribution + three_distribution) * area_per_point;

    printf("ApD: %lf, 5: %lf, 4: %lf, 3: %lf\n", area_per_point, five_area, four_area, three_area);

    five_radius = sqrt(five_area/(M_PI));
    four_radius = sqrt(four_area/(M_PI));
    three_radius = sqrt(three_area/(M_PI));
    printf("5-radius: %lf, 4-radius: %lf, 3-radius: %lf\n\n", five_radius, four_radius, three_radius);
    printf("flag{%.1lf_%.1lf_%.1lf}", five_radius*12, four_radius*12, three_radius*12);


    /* SECTION 4: FREE MEMORY */
    if(lineptr) free(lineptr);
    return 0;
}

/**********************************************************************************************************************************************
 * parseValidToken()
 * 
 * Checks each token for validity. Will only accept tokens that have:
 *  - leading/trailing spaces : ________47__________
 *  - leading zeros : 0000047
 * It will reject tokens with:
 *  - Negative value
 *  - values that are not digits
 *  - decimals
 * 
 * Returns the appropriately parsed integer value, if incorrect format: it will exit
***********************************************************************************************************************************************/

int parseValidToken(char *token){
    int i = 0;
    int length = strlen(token);
    char token_buffer[TOKEN_BUFFER_SIZE];
    int num_bytes = 0;

    // move through any leading whitespace
    while (isspace(token[i])) i++;
    if (length == i) { // invlaid byte found
        printf("Invalid input found, please input only valid integers.\n");
        exit(3);
    }

    // each value should now be a digit until we hit whitespace
    while (i < length && !isspace(token[i]))
    {
        if (!isdigit(token[i])){ // invlaid byte found
            printf("Invalid input found, please input only valid integers. invalid char: %c\n", token[i]);
            exit(3);    
        }
        token_buffer[num_bytes] = token[i];
        num_bytes++;
        i++;
    }
    // the integer should be complete
    token_buffer[num_bytes] = '\0';

    // catch trailing spaces
    while (isspace(token[i])) i++;
    
    if (token[i] != '\0'){
        printf("Invalid input found, please input only valid integers.\n");
        exit(3);
    }

    return atoi(token_buffer);
}
