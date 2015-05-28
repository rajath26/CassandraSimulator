import sys
import datetime
from subprocess import check_output

coord_ip=""
coord_port=""
counter_fbf=""
counter_nofbf=""
num_of_updates=0

def usage():
    print "Invalid Usage!!!!"
    print "python <program_name> <coord_ip> <coord_port> <counter_fbf_name> <counter_nofbf_name> <num_of_updates>"
    
def check_cla():
    if len(sys.argv) != 6:
        usage()
        sys.exit()
        
def create_counter():
    global coord_ip
    global coord_port
    global counter_fbf
    global counter_nofbf
    global num_of_updates
    output=check_output(['./Client', coord_ip, coord_port, 'create', counter_fbf, 'fbf'])
    print output
    output=check_output(['./Client', coord_ip, coord_port, 'create', counter_nofbf, 'nofbf'])
    print output
    
def run_latency_test():
    global coord_ip
    global coord_port
    global counter_fbf
    global counter_nofbf
    global num_of_updates
    
    print "Update on counter with FBF"
    start_fbf=datetime.datetime.now()
    for i in num_of_updates:
        output=check_output(['./Client', coord_ip, coord_port, 'increment', counter_fbf, '1', i])
    end_fbf=datetime.datetime.now()
    print "Elapsed time is: "
    elapsed_fbf=end_fbf-start_fbf
    print elapsed_fbf.total_seconds()
    
    print "Update on counter without FBF"
    start_nofbf=datetime.datetime.now()
    for i in num_of_updates:
        output=check_output(['./Client', coord_ip, coord_port, 'increment', counter_nofbf, '1', i])
    end_nofbf=datetime.datetime.now()
    print "Elapsed time is: "
    elapsed_nofbf=end_nofbf-start_nofbf
    print elapsed_nofbf.total_seconds()
        
def main():
    global coord_ip
    global coord_port
    global counter_fbf
    global counter_nofbf
    global num_of_updates
    check_cla()
    coord_ip=sys.argv[1]
    coord_port=sys.argv[2]
    counter_fbf=sys.argv[3]
    counter_nofbf=sys.argv[4]
    num_of_updates=sys.argv[5]
    create_counter()
    run_latency_test()
    

if __name__=='__main__':
    main()