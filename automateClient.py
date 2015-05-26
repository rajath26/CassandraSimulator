#!/usr/bin/python
import sys
from subprocess import check_output
import os
import time
import datetime
import csv
import random

###
# usage: python automateClient.py <coordinator_ip> <coordinator_port> <tries>
###

def usage():
	print "usage: python automateClient.py <coordinator_ip> <coordinator_port> <tries>"

if len(sys.argv) != 4:
	print "Invalid usage"
	usage()
	sys.exit()

coordIpAddr = sys.argv[1]
coordPortNum = sys.argv[2]
tries = sys.argv[3]
x = int(tries)

sleepTimes=[4,6,8,10]
retryList=[10,18,26,30]

print coordIpAddr
print coordPortNum

results_fbf = "./Results/results_fbf" + datetime.datetime.now().isoformat() + ".csv"
results_nofbf = "./Results/results_nofbf" + datetime.datetime.now().isoformat() + ".csv"
results_expected = "./Results/results_expected" + datetime.datetime.now().isoformat() + ".csv"
results_fbf = open(results_fbf, 'wt')
results_nofbf = open(results_nofbf, 'wt')
results_expected = open(results_expected, 'wt')
res_fbf = csv.writer(results_fbf, delimiter=',')
res_nofbf = csv.writer(results_nofbf, delimiter=',')
res_exp = csv.writer(results_expected, delimiter=',')

exp_count=0

output = check_output(['./Client', coordIpAddr, coordPortNum, 'create', 'c1', 'fbf'])
print output

output = check_output(['./Client', coordIpAddr, coordPortNum, 'create', 'c2', 'nofbf'])
print output

count = 0
retries = 0
for x in range(1, x):
	row = []
	row1 = []
	row2 = []
	count += 1
	if count % (random.choice(retryList)) == 0:
		retries += 1

		exp_count += 0
		row2.append(str(time.strftime("%H:%M:%S")))
		row2.append(str(exp_count))
		row2.append("retry")
		res_exp.writerow(row2)
		results_expected.flush()

		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', 'c1', '1', str(x-1)])
		print output
		row.append(str(time.strftime("%H:%M:%S")))
		val = check_output(['./Client', coordIpAddr, coordPortNum, 'read', 'c1'])
		pos = val.find("VALUE:")
		row.append((str(val[pos+6:]).strip(':')).strip())
		row.append("retry")
		res_fbf.writerow(row)
		results_fbf.flush()

		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', 'c2', '1', str(x-1)])
		print output
		row1.append(str(time.strftime("%H:%M:%S")))
		val1 = check_output(['./Client', coordIpAddr, coordPortNum, 'read', 'c2'])
		pos1 = val1.find("VALUE:")
		row1.append((str(val1[pos+6:]).strip(':')).strip())
		row1.append("retry")
		res_nofbf.writerow(row1)
		results_nofbf.flush()
	
		time.sleep(random.choice(sleepTimes))
	else:
		exp_count += 1
		row2.append(str(time.strftime("%H:%M:%S")))
		row2.append(str(exp_count))
		res_exp.writerow(row2)
		results_expected.flush()

		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', 'c1', '1', str(x)])
		print output
		row.append(str(time.strftime("%H:%M:%S")))
		val = check_output(['./Client', coordIpAddr, coordPortNum, 'read', 'c1'])
		pos = val.find("VALUE:")
		row.append((str(val[pos+6:]).strip(':')).strip())
		res_fbf.writerow(row)
		results_fbf.flush()
	
		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', 'c2', '1', str(x)])
		print output
		row1.append(str(time.strftime("%H:%M:%S")))
		val1 = check_output(['./Client', coordIpAddr, coordPortNum, 'read', 'c2'])
		pos1 = val1.find("VALUE:")
		row1.append((str(val1[pos+6:]).strip(':')).strip())
		res_nofbf.writerow(row1)
		results_nofbf.flush()
	
		time.sleep(random.choice(sleepTimes))

output = check_output(['./Client', coordIpAddr, coordPortNum, 'read', 'c1'])
print output
output = check_output(['./Client', coordIpAddr, coordPortNum, 'read', 'c2'])
print output

print "Number of increments: " + str(count)
print "Number of retries: " + str(retries)
