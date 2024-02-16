/* THIS IS THE SCRIPT FOR SPRAY_AND_PRAY PROBLEM. WE ARE TO CALCULATE THE RADIUS OF MULTIPLE CIRCLES WITHIN A 4X4 GRID GIVEN THE TOTAL AREA AND A UNIFORM DISTRIBUTION.
HERE'S WHAT WE ARE GIVEN:
- A UNIFORM DISTRIBUTION OF DATA ACROSS THE WHOLE TARGET IN "DATA.CSV"
- TOTAL AREA OF THE SQUARE IS 4X4 = 16 UNITS
WE WILL NEED TO PARSE THE "DATA.CSV" FILE AND RECORD HOW MANY TIMES WE SEE EACH NUMBER. THEN WE WILL PERFORM  SOME SIMPLE ALGEBRA:
- 16 (TOTAL AREA) / TOTAL_DISTRIBUTION = AREA_PER_POINT
- SPECIFIC_DISTRIBUTION * AREA_PER_POINT = SPECIFIC_AREA
- (SPECIFIC_AREA/(PI))^(1/2) = SPECIFIC_RADIUS

THIS IS NOTHING MORE THAN FIRST READING INTO A DYNAMIC ARRAY, THEN SCANNING AND INCREMENTING THE CORRESPONDING NUMBERS, THEN EXECUTING THE ALEGEBRA STATED ABOVE.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define TOTAL_AREA 16.0
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

int main(void) {
    FILE *fp = NULL;
    char *lineptr = NULL;
    char *token = NULL;
    int buffer = 0;
    int line_size;
    const char delimiter = ',';

    int parsed_token;
    int total_distribution, five_distribution, four_distribution, three_distribution, two_distribution;
    double area_per_point, five_area, four_area, three_area, two_area;
    double five_radius, four_radius, three_radius;
    

    total_distribution = five_distribution = four_distribution = three_distribution = two_distribution = 0;


    if ((fp = fopen("data.csv", "r")) == NULL) {
        perror("Error opening the file...");
        exit(1);
    }
    if ((line_size = getline(&lineptr, &buffer, fp)) == -1){
        perror("Error reading data from file, failure to read line");
        exit(2);
    }

    token  = strtok(lineptr, &delimiter);
    while (token != NULL) {
        parsed_token = (int)atof(token);
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
                perror("Invalid data!");
                exit(3);
        }

        token = strtok(NULL, &delimiter);
    }
    total_distribution = five_distribution + four_distribution + three_distribution + two_distribution;
    // Check 
    printf("Total: %d, 5: %d, 4: %d, 3: %d, 2: %d\n", total_distribution, five_distribution, four_distribution, three_distribution, two_distribution);


    //do math
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


    if(lineptr) free(lineptr);
    return 0;
}