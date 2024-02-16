'''
REQUIRED FILES:
    - "addresses.csv"
        - REQUIRED FOR OUR INITIAL DATA REQUESTS
    - "batch.csv"
        - GENERATED AT RUNTIME AND OVERWRITTEN
    - "missed_batch.csv"
        - GENERATED AT RUNTIME AND OVERWRITTEN

help used:
    - https://stackoverflow.com/questions/31779617/using-census-bulk-geocoder-with-python-requests-library
    - https://www.youtube.com/watch?v=nIdmmEfW3R0  -  used after the fact
'''
import csv
import requests 
import json
import math

closest_row = 0 
closest_point = [0, 0]
closest_distance = 0
destination = [37.065984, -79.601172]

burl = 'https://geocoding.geo.census.gov/geocoder/locations/addressbatch'

# THIS OPENS THE ADDRESS.CSV FILE AND WRITES TO 'batch.csv' IN THE PROPER API FORMAT AND SKIPS OVER 'none' DATA LINES
with open('addresses.csv','r') as file:
    cfile = csv.reader(file)
    with open('batch.csv', 'w') as batch:
        batch_writer = csv.writer(batch)

        id = 0
        for row in cfile:
            # note that id keeps track of which row even if the row is a "None"!
            if row[0] != "none":
                batch_writer.writerow([id, row[1], row[2], "null", "null"])
            id += 1

# PREPPING FORMAT FOR OUR POST REQUEST
files = {'addressFile': ('batch.csv', open('batch.csv', 'rb'), 'text/csv')}
data = {'benchmark':'Public_AR_Current', 'format':'json'}
print("Sending {} to {}...".format(files, burl))
r = requests.post(burl, files=files, data=data)
print("Response recieved! Code: {}".format(r))

returned_list = r.text.split('\n')
returned_list = returned_list[:-1] # removes the last(empty) element1

# SAVES THE MISSED ENTRIES SO THAT WE CAN CHECK AGAIN
with open('missed_batch.csv','w') as missed:
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
                closest_distance = math.dist(closest_point, destination)
                closest_id = returned_list[entry][0]
                # print("Data: {} -- {} -- {}".format(closest_point, closest_distance, closest_id))

            new_point = [ returned_list[entry][12], returned_list[entry][11]]
            new_distance = math.dist(new_point, destination)
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


# WE WILL NOW RESEND A BATCH OF MISSED DATA AS THE API IS KNOWN TO HAVE INCONSISTENT MISSES, AND SUGGESTS TO SEND ANOTHER SEPARATE BATCH OF MISSED ENTRIES
files = {'addressFile': ('missed_batch.csv', open('missed_batch.csv', 'rb'), 'text/csv')}
data = {'benchmark':'Public_AR_Current', 'format':'json'}
print("Sending {} to {}...".format(files, burl))
r = requests.post(burl, files=files, data=data)
print("Response recieved! Code: {}".format(r))

returned_list = r.text.split('\n')
returned_list = returned_list[:-1] # removes the last(empty) element1

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
        new_distance = math.dist(new_point, destination)
        # print("Comparing: {} to {}".format(new_distance, closest_distance))
        if new_distance < closest_distance:
            closest_point = new_point
            closest_distance = new_distance
            closest_id = returned_list[entry][0]
        
    else:
        print("No record found for {}".format(returned_list[entry]))
        


# FIND THE NAME FROM 'addresses.csv'
with open('addresses.csv','r') as file:
    cfile = csv.reader(file) 
    counter = 0 
    for row in cfile:
        if counter == closest_id:
            print("The nearest person's name is: {}".format(row[0]))
            break
        counter += 1