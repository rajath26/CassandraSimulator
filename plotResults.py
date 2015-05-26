import csv
import sys
import datetime
import random
import matplotlib.pyplot as plt
from matplotlib.dates import MinuteLocator, SecondLocator, DateFormatter

##
# Usage : python plotResults <fbf_csv> <nonFbf_csv> <expected_counter>
##

time1 = []
fbfCounter = []
time2 = []
noFbfCounter = []
time3 = []
expCounter = []
retryMarker1 = []
retryMarker1Time = []
retryMarker2 = []
retryMarker2Time = []
retryIndex1 = []
retryIndex2 = []
retryMarker3 = []
retryMarker3Time = []
retryIndex3 = []

fbfFile = sys.argv[1]
noFbfFile = sys.argv[2]
expFile = sys.argv[3]

with open(fbfFile, 'rU') as f1:
    try:
        reader1 = csv.reader(f1, delimiter=',')
        for row in reader1:
            time1.append(row[0])
            try:
                fbfCounter.append(float(row[1]))
            except ValueError:
                continue
            try:
                if str(row[2]) == "retry":
                    retryMarker1.append(float(row[1]))
                    retryMarker1Time.append(row[0])
                    index=int(len(fbfCounter))-2
                    retryIndex1.append(index)
            except IndexError:
                continue

    finally:
        f1.close()

    time11 = [datetime.datetime.strptime(s, '%H:%M:%S') for s in time1]
    retryTime11 = [datetime.datetime.strptime(s, '%H:%M:%S') for s in retryMarker1Time]

with open(noFbfFile, 'rU') as f2:
    try:
        reader2 = csv.reader(f2, delimiter=',')
        for row2 in reader2:
            time2.append(row2[0])
            try:
                noFbfCounter.append(float(row2[1]))
            except ValueError:
                continue
            try:
                if str(row2[2]) == "retry":
                    retryMarker2.append(float(row2[1]))
                    retryMarker2Time.append(row2[0])
                    index=int(len(noFbfCounter))-2
                    retryIndex2.append(index)
            except IndexError:
                continue

       
    finally:
        f2.close()

    time21 = [datetime.datetime.strptime(s, '%H:%M:%S') for s in time2]
    retryTime21 = [datetime.datetime.strptime(s, '%H:%M:%S') for s in retryMarker2Time]

with open(expFile, 'rU') as f3:
    try:
        reader3 = csv.reader(f3, delimiter=',')
        for row3 in reader3:
            time3.append(row3[0])
            try:
                expCounter.append(float(row3[1]))
            except ValueError:
                continue
            try:
                if str(row3[2]) == "retry":
                    retryMarker3.append(float(row3[1]))
                    retryMarker3Time.append(row3[0])
                    index=int(len(expCounter))-2
                    retryIndex3.append(index)
            except IndexError:
                continue

       
    finally:
        f3.close()

    time31 = [datetime.datetime.strptime(s, '%H:%M:%S') for s in time3]
    retryTime31 = [datetime.datetime.strptime(s, '%H:%M:%S') for s in retryMarker3Time]

fig = plt.figure(figsize=(8,6))
plt.title("Idempotent Updates using a FBF")
ax = fig.add_subplot(111)

#ln1 = ax.plot(time1, shard1_traffic, '-', label="shard1 traffic", lw=1)

retryTime11New = []
retryMarker1New = []
for i in range(0,len(retryIndex1)):
    try:
        retryTime11New.append(time11[retryIndex1[i]])
        retryMarker1New.append(fbfCounter[retryIndex1[i]])
    except IndexError:
        continue
retryTime21New = []
retryMarker2New = []
for i in range(0,len(retryIndex2)):
    try:
        retryTime21New.append(time21[retryIndex2[i]])
        retryMarker2New.append(noFbfCounter[retryIndex2[i]])
    except IndexError:
        continue
retryTime31New = []
retryMarker3New = []
for i in range(0,len(retryIndex3)):
    try:
        retryTime31New.append(time31[retryIndex3[i]])
        retryMarker3New.append(expCounter[retryIndex3[i]])
    except IndexError:
        continue
for i in range(0,len(retryMarker2New)):
    ln4 = ax.plot([retryTime21New[i]], [retryMarker2New[i]], 'rD', label="Retry")
for i in range(0,len(retryMarker1New)):
    ln5 = ax.plot([retryTime11New[i]], [retryMarker1New[i]], 'rD', label="Retry")
for i in range(0,len(retryMarker3New)):
    ln6 = ax.plot([retryTime31New[i]], [retryMarker3New[i]], 'rD', label="Retry")


ln1 = ax.step(time11, fbfCounter, label="Counter with FBF enabled", lw=1)
#ln1.set_marker('--')
ln3 = ax.step(time31, expCounter, label="Expected Counter value", lw=1)
ln2 = ax.step(time21, noFbfCounter, label="Counter with FBF disabled", lw=1)
#ln2.set_marker(':')
#ln3.set_marker('-.')

minutes = MinuteLocator()
seconds = SecondLocator()
ax.xaxis.set_major_locator(minutes)
ax.xaxis.set_minor_locator(seconds)
ax.xaxis.set_major_formatter(DateFormatter("%H:%M:%S"))
ax = plt.gca()

#lns = ln1 + ln2 + ln3 + ln4 + ln5 + ln6
lns = ln1 + ln2 + ln3 #+ ln4
labs = [l.get_label() for l in lns]
ax.legend(lns, labs, loc='upper left', prop={'size':10})

ax.set_xlabel("Time")
ax.set_ylabel(r"Counter value")

plt.gcf().autofmt_xdate()

plt.show()
