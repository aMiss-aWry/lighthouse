# author : Kanika Sood 
# Date : Aug 24
#this program takes in (WekaPredictionResults.csv)csv file (file with the predicted classes from Weka) and gives the top 10 most used solvers for PETSc
import csv
from collections import OrderedDict
import operator

#dictionary for solver count
solverCount = {}
#154 solvers fo PETSc
solvers = [89565283,8793455,90197667,49598909,91036839,45869639,45869638,45869637,47942867,89269802,
89269803,89269801,89269804,59072883,59072882,59072881,7285381,7285384,59072884,49598911,49598910,
49598912,30870721,36025723,36025722,53302993,30870720,64278029,36025724,44526267,8793454,8793456,
8793453,17734818,32168839,32168838,57331597,95762352,57331599,57331598,32168837,88865078,88865079,
49834417,49834419,49834418,88865076,88865077,11256942,11256943,11256941,42851841,11256944,31459546,
17887723,91845162,53362206,95762355,12321508,75830644,57331600,43373444,95762353,43373441,43373442,
43373443,91068411,91068410,5890861,85483012,5890863,5890862,18868444,18868441,18868443,18868442,
91068408,47942864,47942865,47942866,91068409,8520536,5890860,82456576,29030069,95762354,90783920,
99720138,29030071,29030070,29030072,85490469,30870723,30870722,26415435,26415434,26415433,26415432,
13323659,45869640,36564233,68908713,7285382,36564232,7285383,75830645,36564234,81986705,29553941,
29553943,29553942,69654761,29553944,69654763,69654762,32874609,32168840,90197664,90197665,90197666,
69654760,37052870,37052871,19932321,19932323,19932322,19932324,80361466,80361467,80361464,80361465,
49834420,1216556,38678404,38678401,38678402,38678403,32874611,32874610,32874612,44114477,44114476,
44114479,44114478,36564235,36025721,75830647,75830646,85490471,85490470,85490472,18524981,37052869,
37052868]

#setting all solver counts to 0 initially
solverCount = dict((solver,0) for solver in solvers)
#print solverCount

#to count total solved cases
totalCount = 0

with open('WekaPredictionResults.csv', 'rb') as csvfile:
	reader = csv.DictReader(csvfile)
	for row in reader:
		if row['class'] == "good" and row['predictedclass'] == "good" : 
			solverCount[int(row['solver'])] +=1
			totalCount+=1

print "count" , totalCount

solverCountAscending = sorted(solverCount.items(), key=operator.itemgetter(1))

#write the result(solvers + no. of occurrences) to output file
outFile = open('outputTop10SolverList.txt', 'w')
outFile.seek(0)
outFile.write("(Solver id, No. of times it occurred) \n")
#print solverCount_ascending, type(solverCount_ascending), len(solverCount_ascending)


for i in range(len(solverCountAscending) - 1,0, -1):
	if i > 143 : 
		solverIdCount = str(solverCountAscending[i])
		print  solverCountAscending[i]
		outFile.write(solverIdCount + " ")
		outFile.write("\n")
		i = i-1
total = "Total Count for all 154 solvers : " + str(totalCount)
outFile.write(total)
outFile.close()



		