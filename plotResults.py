import csv
import sys
import datetime
import random
import matplotlib.pyplot as plt
from matplotlib.dates import MinuteLocator, SecondLocator, DateFormatter

time1 = []
fbfCounter = []
time2 = []
noFbfCounter = []

fbfFile = sys.argv[1]
noFbfFile = sys.argv[2]

with open(fbfFile, 'rU') as f1:
    try:
        reader1 = csv.reader(f1, delimiter=',')
        for row in reader1:
            time1.append(row[0])
            try:
                fbfCounter.append(float(row[1]))
            except ValueError:
                continue

       
    finally:
        f1.close()

    time11 = [datetime.datetime.strptime(s, '%H:%M:%S') for s in time1]

with open(noFbfFile, 'rU') as f2:
    try:
        reader2 = csv.reader(f2, delimiter=',')
        for row2 in reader2:
            time2.append(row2[0])
            try:
                noFbfCounter.append(float(row2[1]))
            except ValueError:
                continue
       
    finally:
        f2.close()

    time21 = [datetime.datetime.strptime(s, '%H:%M:%S') for s in time2]

fig = plt.figure(figsize=(8,6))
plt.title("Idempotent Updates using a FBF")
ax = fig.add_subplot(111)

#ln1 = ax.plot(time1, shard1_traffic, '-', label="shard1 traffic", lw=1)
ln2 = ax.plot(time11, fbfCounter, '-', label="Counter with FBF enabled", lw=1)
ln3 = ax.plot(time21, noFbfCounter, '--', label="Counter with FBF disabled", lw=1)
minutes = MinuteLocator()
seconds = SecondLocator()
ax.xaxis.set_major_locator(minutes)
ax.xaxis.set_minor_locator(seconds)
ax.xaxis.set_major_formatter(DateFormatter("%H:%M:%S"))
ax = plt.gca()

#lns = ln1 + ln2 + ln3 + ln4 + ln5 + ln6
lns = ln2 + ln3 
labs = [l.get_label() for l in lns]
ax.legend(lns, labs, loc='upper left', prop={'size':10})

ax.set_xlabel("Time")
ax.set_ylabel(r"Counter value")

plt.gcf().autofmt_xdate()

plt.show()
