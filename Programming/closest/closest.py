'''
THIS PROGRAM CONNECTS TO 'https://geocoding.geo.census.gov' USING ITS API. IT SENDS A BATCH OF ADDRESSES VIA A .csv FILE AND
PARSES THE RESPONSE TO BE WORKED WITH. ANY MISSED ENTRIES WILL GET TO WRITTEN TO "missed_batch.csv", AND THEN BE RESUBMITTED 
TO SEE IF THE MISS WAS DUE TO A SERVER ERROR. (THIS WAS THE ADVISED PROCESS FOR SENDING BATCHES THAT RECIEVE A MISS). AS WE 
PARSE OUR DATA INTO USABLE DATA TYPES, WE WILL CALCULATE AND COMPARE THE DISTANCE TO THE POINT GIVEN. THE CORRESPONDING VARIABLES
THAT HOLD THE "NEAREST-ENTRY" DATA WILL BE SAVED. ONE LAST SEARCH THROUGH THE ORIGINAL ".csv" WILL OCCUR TO FIND THE NAME OF 
THE ENTRY THAT WAS CLOSEST TO THE DESTINATION.

6 MAIN SECTIONS:
1. READ "PATH_TO_ADDRESS_FILE" INTO MEMORY & WRITE TO "PATH_TO_BATCH_FILE" IN API FORMAT
    - AS WE READ IN THE DATA WE WILL APPROPRIATELY PARSE IT, SKIPPING OVER "NONE" ENTRIES
2. SEND POST REQUEST
    - SEND THE BATCH FILE TO THE GEOCODER
3. PARSE RESPONSE & COMPARE VALUES 
    - RESPONSES COME BACK AS A STRING IN WHICH WE WILL SPLIT ACCORDING TO ROW, AND USE THE "Match", "Lat" & "Long" VALUES
4. BUILD AND RESEND BATCH "PATH_TO_MISSED_BATCH_FILE"
    - BUILD A ".csv" WITH APPROPRIATE FORMATTING TO RESEND TO THE GEOCODER. THIS IS ADVISED IN THE GEOCODER FAQ.
5. PARSE RESPONSE & COMPARE VALUES FROM MISSED BATCH
    - RESPONSES COME BACK AS A STRING IN WHICH WE WILL SPLIT ACCORDING TO ROW, AND USE THE "Match", "Lat" & "Long" VALUES
6. SEARCH FOR UNIQUE ID IN "PATH_TO_ADDRESS_FILE"
    - USING THE UNIQUE ID THAT IS SYNCED TO THE ROW NUMBER, WE CAN EASILY FIND THE CORRESPONDING NAME


REQUIRED FILES: -  NOTE THAT ALL FILES WILL BE CONTAINED IN THE CURRENT DIRECTORY
    - "addresses.csv"
        - REQUIRED FOR OUR INITIAL DATA REQUESTS
    - "batch.csv"
        - GENERATED AT RUNTIME AND OVERWRITTEN
    - "missed_batch.csv"
        - GENERATED AT RUNTIME AND OVERWRITTEN

help used:
    - https://www2.census.gov/geo/pdfs/maps-data/data/Census_Geocoder_FAQ.pdf
    - https://stackoverflow.com/questions/31779617/using-census-bulk-geocoder-with-python-requests-library
    - https://www.youtube.com/watch?v=nIdmmEfW3R0  -  used after the fact
'''
import csv
import requests 
import json
import math

DESTINATION = [37.065984, -79.601172]
PATH_TO_ADDRESS_FILE = "addresses.csv"
PATH_TO_BATCH_FILE = "batch.csv"
PATH_TO_MISSED_BATCH_FILE = "missed_batch.csv"
BATCH_URL = 'https://geocoding.geo.census.gov/geocoder/locations/addressbatch'


#----------------- SECTION 1. READ "PATH_TO_ADDRESS_FILE" INTO MEMORY & WRITE TO "PATH_TO_BATCH_FILE" IN API FORMAT---------------------
closest_row = 0 
closest_point = [0, 0]
closest_distance = 0

# THIS OPENS THE ADDRESS.CSV FILE AND WRITES TO 'PATH_TO_BATCH_FILE' IN THE PROPER API FORMAT AND SKIPS OVER 'none' DATA LINES
with open(PATH_TO_ADDRESS_FILE,'r') as file:
    cfile = csv.reader(file)
    with open(PATH_TO_BATCH_FILE, 'w') as batch:
        batch_writer = csv.writer(batch)

        id = 0
        for row in cfile:
            # note that id keeps track of which row even if the row is a "None"!
            if row[0] != "none":
                batch_writer.writerow([id, row[1], row[2], "null", "null"])
            id += 1



# #-----------------------------------------------------------SECTION 2. SEND POST REQUEST---------------------------------------------------------------

# PREPPING FORMAT FOR OUR POST REQUEST
files = {'addressFile': (PATH_TO_BATCH_FILE, open(PATH_TO_BATCH_FILE, 'rb'), 'text/csv')}
data = {'benchmark':'Public_AR_Current', 'format':'json'}
print("Sending {} to {}...".format(files, BATCH_URL))
r = requests.post(burl, files=files, data=data)
print("Response recieved! Code: {}".format(r))

returned_list = r.text.split('\n')
returned_list = returned_list[:-1] # removes the last(empty) element1


# ----------------------------------------------------------SECTION 3. PARSE RESPONSE & COMPARE VALUES -----------------------------------------------------------

# SAVES THE MISSED ENTRIES SO THAT WE CAN CHECK AGAIN
with open(PATH_TO_MISSED_BATCH_FILE,'w') as missed:
    missed_writer = csv.writer(missed)
    # parses each row's data into proper format, we only care about lat, long, and "Match" 
    for entry in range(len(returned_list)):
        returned_list[entry] = returned_list[entry].split(',') # splits each row by the commas, note that this makes the quotes mess up some data points!

        # FIRST PARSE ONLY THE ID AND THE "Match" COLUMNS, WE ONLY PARSE THE REST IF THERE IS A "Match"
        returned_list[entry][0] = int(returned_list[entry][0][1:-1])
        returned_list[entry][5] = returned_list[entry][5][1:-1]

        # COMPARES EACH DISTANCE ONLY IF THE DATA HAD FOUND A MATCH
        if returned_list[entry][5] == "Match":
            
            # PARSE THE LAT & LONG VALUES
            returned_list[entry][11] = float(returned_list[entry][11][1:])
            returned_list[entry][12] = float(returned_list[entry][12][:-1])

            # THESE ARE TO PARSE THE REST OF THE DATA POINTS IF NEEDED
            # returned_list[entry][1] = returned_list[entry][1][1:]
            # returned_list[entry][2] = returned_list[entry][2][1:] 
            # returned_list[entry][3] = returned_list[entry][3][1:]
            # returned_list[entry][4] = returned_list[entry][4][1:-1]
            # returned_list[entry][6] = returned_list[entry][6][1:-1]
            # returned_list[entry][7] = returned_list[entry][7][1:]
            # returned_list[entry][8] = returned_list[entry][8][1:]
            # returned_list[entry][9] = returned_list[entry][9][1:]
            # returned_list[entry][10] = returned_list[entry][10][1:-1]
            # returned_list[entry][13] = returned_list[entry][13][1:-1]
            # returned_list[entry][14] = returned_list[entry][14][1:-1]

            if entry == 0:
                closest_point = [ returned_list[entry][12], returned_list[entry][11]]
                closest_distance = math.dist(closest_point, DESTINATION)
                closest_id = returned_list[entry][0]
                # print("Data: {} -- {} -- {}".format(closest_point, closest_distance, closest_id))

            new_point = [ returned_list[entry][12], returned_list[entry][11]]
            new_distance = math.dist(new_point, DESTINATION)
            # print("Comparing: {} to {}".format(new_distance, closest_distance))
            if new_distance < closest_distance:
                closest_point = new_point
                closest_distance = new_distance
                closest_id = returned_list[entry][0]
            
        else:
            returned_list[entry][1] = returned_list[entry][1][3:-1]
            returned_list[entry][2] = returned_list[entry][2][1:]
            returned_list[entry][3] = returned_list[entry][3][1:]
            returned_list[entry][4] = returned_list[entry][4][1:-1]
            missed_writer.writerow(returned_list[entry][:-1]) # this writes the uniqueID and the address line

# --------------------------------------------------- SECTION 4. BUILD AND RESEND BATCH "PATH_TO_MISSED_BATCH_FILE"-------------------------------------------------

# WE WILL NOW RESEND A BATCH OF MISSED DATA AS THE API IS KNOWN TO HAVE INCONSISTENT MISSES, AND SUGGESTS TO SEND ANOTHER SEPARATE BATCH OF MISSED ENTRIES
files = {'addressFile': (PATH_TO_MISSED_BATCH_FILE, open(PATH_TO_MISSED_BATCH_FILE, 'rb'), 'text/csv')}
data = {'benchmark':'Public_AR_Current', 'format':'json'}
print("Sending {} to {}...".format(files, BATCH_URL))
r = requests.post(burl, files=files, data=data)
print("Response recieved! Code: {}".format(r))

returned_list = r.text.split('\n')
returned_list = returned_list[:-1] # removes the last(empty) element1

# --------------------------------------------------- SECTION 5. PARSE RESPONSE & COMPARE VALUES FROM MISSED BATCH -------------------------------------------------

# parses each row's data into proper format, we only care about lat, long, and "Match" 
for entry in range(len(returned_list)):
    returned_list[entry] = returned_list[entry].split(',') # splits each row by the commas, note that this makes the quotes mess up some data points!

    # FIRST PARSE ONLY THE ID AND THE "Match" COLUMNS, WE ONLY PARSE THE REST IF THERE IS A "Match"
    returned_list[entry][0] = int(returned_list[entry][0][1:-1])
    returned_list[entry][5] = returned_list[entry][5][1:-1]

    # COMPARES EACH DISTANCE ONLY IF THE DATA HAD FOUND A MATCH
    if returned_list[entry][5] == "Match":
        
        # PARSE THE LAT & LONG VALUES
        returned_list[entry][11] = float(returned_list[entry][11][1:])
        returned_list[entry][12] = float(returned_list[entry][12][:-1])

        # THESE ARE TO PARSE THE REST OF THE DATA POINTS IF NEEDED
        # returned_list[entry][1] = returned_list[entry][1][1:]
        # returned_list[entry][2] = returned_list[entry][2][1:]
        # returned_list[entry][3] = returned_list[entry][3][1:]
        # returned_list[entry][4] = returned_list[entry][4][1:-1]
        # returned_list[entry][6] = returned_list[entry][6][1:-1]
        # returned_list[entry][7] = returned_list[entry][7][1:]
        # returned_list[entry][8] = returned_list[entry][8][1:]
        # returned_list[entry][9] = returned_list[entry][9][1:]
        # returned_list[entry][10] = returned_list[entry][10][1:-1]
        # returned_list[entry][13] = returned_list[entry][13][1:-1]
        # returned_list[entry][14] = returned_list[entry][14][1:-1]

        new_point = [ returned_list[entry][12], returned_list[entry][11]]
        new_distance = math.dist(new_point, DESTINATION)
        # print("Comparing: {} to {}".format(new_distance, closest_distance))
        if new_distance < closest_distance:
            closest_point = new_point
            closest_distance = new_distance
            closest_id = returned_list[entry][0]
        
    else:
        print("No record found for {}".format(returned_list[entry]))
        

# --------------------------------------------------- SECTION 6. SEARCH FOR UNIQUE ID IN "PATH_TO_ADDRESS_FILE"-------------------------------------------------

# FIND THE NAME FROM 'addresses.csv'
with open(PATH_TO_ADDRESS_FILE,'r') as file:
    cfile = csv.reader(file) 
    counter = 0 
    for row in cfile:
        if counter == closest_id:
            print("The nearest person's name is: {}".format(row[0]))
            break
        counter += 1
