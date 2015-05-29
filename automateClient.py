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
	print "usage: python automateClient.py <counter_fbf> <counter_nofbf> <counter_rbf> <tries> [<coordinator_ip> <coordinator_port>,....]"

if len(sys.argv) < 7:
	print "Invalid usage"
	usage()
	sys.exit()

coordipList = []
coordportList = []
counter_fbf = sys.argv[1]
counter_nofbf = sys.argv[2]
counter_rbf = sys.argv[3]
tries = sys.argv[4]
x = int(tries)
for i in range(5,len(sys.argv)):
	if i%2 != 0:
		coordipList.append(str(sys.argv[i]))
	else:
		coordportList.append(str(sys.argv[i]))

randomList = [i for i in range(0,len(coordipList))]

sleepTimes=[1]
retryList=[5]
retryIndex=[1,2,3,4,5,6,7,8,9,10]

print coordipList
print coordportList

results_fbf = "./Results/results_fbf" + datetime.datetime.now().isoformat() + ".csv"
results_nofbf = "./Results/results_nofbf" + datetime.datetime.now().isoformat() + ".csv"
results_rbf = "./Results/results_rbf" + datetime.datetime.now().isoformat() + ".csv"
results_expected = "./Results/results_expected" + datetime.datetime.now().isoformat() + ".csv"
results_fbf = open(results_fbf, 'wt')
results_nofbf = open(results_nofbf, 'wt')
results_rbf = open(results_rbf, 'wt')
results_expected = open(results_expected, 'wt')
res_fbf = csv.writer(results_fbf, delimiter=',')
res_nofbf = csv.writer(results_nofbf, delimiter=',')
res_rbf = csv.writer(results_rbf, delimiter=',')
res_exp = csv.writer(results_expected, delimiter=',')

exp_count=0

index=random.choice(randomList)
coordIpAddr=coordipList[index]
coordPortNum=coordportList[index]

output = check_output(['./Client', coordIpAddr, coordPortNum, 'create', counter_fbf, 'fbf'])
print output

index=random.choice(randomList)
coordIpAddr=coordipList[index]
coordPortNum=coordportList[index]

output = check_output(['./Client', coordIpAddr, coordPortNum, 'create', counter_nofbf, 'nofbf'])
print output

index=random.choice(randomList)
coordIpAddr=coordipList[index]
coordPortNum=coordportList[index]

output = check_output(['./Client', coordIpAddr, coordPortNum, 'create', counter_rbf, 'rbf'])
print output

count = 0
retries = 0
for x in range(1, x):
	row = []
	row1 = []
	row2 = []
	row3 = []
	count += 1
	if count % (random.choice(retryList)) == 0:
		retries += 1

		exp_count += 0
		row2.append(str(time.strftime("%H:%M:%S")))
		row2.append(str(exp_count))
		row2.append("retry")
		res_exp.writerow(row2)
		results_expected.flush()
		
		idx=random.choice(retryIndex)
		
		index=random.choice(randomList)
		coordIpAddr=coordipList[index]
		coordPortNum=coordportList[index]
		
		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', counter_fbf, '1', str(x-idx)])
		print output
		row.append(str(time.strftime("%H:%M:%S")))
		val = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_fbf])
		pos = val.find("VALUE:")
		row.append((str(val[pos+6:]).strip(':')).strip())
		row.append("retry")
		res_fbf.writerow(row)
		results_fbf.flush()
		
		index=random.choice(randomList)
		coordIpAddr=coordipList[index]
		coordPortNum=coordportList[index]

		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', counter_nofbf, '1', str(x-idx)])
		print output
		row1.append(str(time.strftime("%H:%M:%S")))
		val1 = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_nofbf])
		pos1 = val1.find("VALUE:")
		row1.append((str(val1[pos+6:]).strip(':')).strip())
		row1.append("retry")
		res_nofbf.writerow(row1)
		results_nofbf.flush()
	
		index=random.choice(randomList)
		coordIpAddr=coordipList[index]
		coordPortNum=coordportList[index]
 
		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', counter_rbf, '1', str(x-idx)])
		print output
		row3.append(str(time.strftime("%H:%M:%S")))
		val3 = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_rbf])
		pos3 = val3.find("VALUE:")
		row3.append((str(val3[pos+6:]).strip(':')).strip())
		row3.append("retry")
		res_rbf.writerow(row3)
		results_rbf.flush()

		#time.sleep(random.choice(sleepTimes))
		
		exp_count += 1
		row2.append(str(time.strftime("%H:%M:%S")))
		row2.append(str(exp_count))
		res_exp.writerow(row2)
		results_expected.flush()
		
		index=random.choice(randomList)
		coordIpAddr=coordipList[index]
		coordPortNum=coordportList[index]

		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', counter_fbf, '1', str(x)])
		print output
		row.append(str(time.strftime("%H:%M:%S")))
		val = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_fbf])
		pos = val.find("VALUE:")
		row.append((str(val[pos+6:]).strip(':')).strip())
		res_fbf.writerow(row)
		results_fbf.flush()
		
		index=random.choice(randomList)
		coordIpAddr=coordipList[index]
		coordPortNum=coordportList[index]
	
		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', counter_nofbf, '1', str(x)])
		print output
		row1.append(str(time.strftime("%H:%M:%S")))
		val1 = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_nofbf])
		pos1 = val1.find("VALUE:")
		row1.append((str(val1[pos+6:]).strip(':')).strip())
		res_nofbf.writerow(row1)
		results_nofbf.flush()
		
		index=random.choice(randomList)
		coordIpAddr=coordipList[index]
		coordPortNum=coordportList[index]
	
		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', counter_rbf, '1', str(x)])
		print output
		row3.append(str(time.strftime("%H:%M:%S")))
		val3 = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_rbf])
		pos3 = val3.find("VALUE:")
		row3.append((str(val3[pos+6:]).strip(':')).strip())
		res_rbf.writerow(row3)
		results_rbf.flush()
	
		#time.sleep(random.choice(sleepTimes))

	else:
		exp_count += 1
		row2.append(str(time.strftime("%H:%M:%S")))
		row2.append(str(exp_count))
		res_exp.writerow(row2)
		results_expected.flush()
		
		index=random.choice(randomList)
		coordIpAddr=coordipList[index]
		coordPortNum=coordportList[index]

		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', counter_fbf, '1', str(x)])
		print output
		row.append(str(time.strftime("%H:%M:%S")))
		val = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_fbf])
		pos = val.find("VALUE:")
		row.append((str(val[pos+6:]).strip(':')).strip())
		res_fbf.writerow(row)
		results_fbf.flush()
		
		index=random.choice(randomList)
		coordIpAddr=coordipList[index]
		coordPortNum=coordportList[index]
	
		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', counter_nofbf, '1', str(x)])
		print output
		row1.append(str(time.strftime("%H:%M:%S")))
		val1 = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_nofbf])
		pos1 = val1.find("VALUE:")
		row1.append((str(val1[pos+6:]).strip(':')).strip())
		res_nofbf.writerow(row1)
		results_nofbf.flush()
		
		index=random.choice(randomList)
		coordIpAddr=coordipList[index]
		coordPortNum=coordportList[index]
	
		output = check_output(['./Client', coordIpAddr, coordPortNum, 'increment', counter_rbf, '1', str(x)])
		print output
		row3.append(str(time.strftime("%H:%M:%S")))
		val3 = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_rbf])
		pos3 = val3.find("VALUE:")
		row3.append((str(val3[pos+6:]).strip(':')).strip())
		res_rbf.writerow(row3)
		results_rbf.flush()
	
		#time.sleep(random.choice(sleepTimes))

index=random.choice(randomList)
coordIpAddr=coordipList[index]
coordPortNum=coordportList[index]

output = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_fbf])
print output

index=random.choice(randomList)
coordIpAddr=coordipList[index]
coordPortNum=coordportList[index]

output = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_nofbf])
print output

index=random.choice(randomList)
coordIpAddr=coordipList[index]
coordPortNum=coordportList[index]

output = check_output(['./Client', coordIpAddr, coordPortNum, 'read', counter_rbf])
print output

print "Expected Value: " + str(exp_count)
print "Number of increments: " + str(count)
print "Number of retries: " + str(retries)