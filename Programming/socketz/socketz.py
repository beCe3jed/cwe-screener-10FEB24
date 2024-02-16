'''
This program connects to a port using a client socket. It then recieves and decodes responses from the client into math questions.
It then parses and calculates the result of each math question and sends it back to the client, for all 500 questions. 

The flag will be printed after the 500th question and will be parsed appropriately

reference:
- https://discuss.python.org/t/how-to-connect-to-remote-server-using-python/24379/2
- https://docs.python.org/3/howto/sockets.html
'''

import socket

TARGET_HOST = "www.cweaccessionsctf.com"  # ip address or URL
TARGET_PORT = 1455
OUTPUT_FILE = "response.html"

# create a socket object and handler
try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client:
        # connect the client
        client.connect((TARGET_HOST, TARGET_PORT))
        
        for i in range(500):
            # receive data from the port
            response = client.recv(4096)
            questions = response.decode().strip("?\n")
            # print the question to the terminal
            print(questions)

            # parse the numbers to be operated on
            questions = questions.split(" ")
            num1 = int(questions[5])
            num2 = int(questions[7])

            # parse the operator
            operator = questions[6]

            # execute the proper operator
            if operator == '+':
                result = num1 + num2
            elif operator == '-':
                result = num1 - num2
            elif operator == '*':
                result = num1 * num2
            elif operator == '/':
                result = num1 / num2
            # exit if the operator that we get is not valid
            else:
                print("We got a weird operator:" + operator)
                print(questions[5] + ' ' + operator + ' ' + questions[7] + " ?")
                exit()

            # prep the result to send back to the client
            result = str(result) + "\n"
            print(result)

            # send some data
            data = bytes(result, 'UTF-8')
            client.sendall(data)

        # after loop ends get the flag
        flag = client.recv(4096)
        print(flag)

        # create a file handler for the output
        with open(OUTPUT_FILE, mode='w', encoding='UTF-8') as output:
            print(flag.decode(), file=output)
except TimeoutError:
    print("TimeoutError! Unable to connect port in ample time. Exiting...")
    exit()

print("Script complete.")
