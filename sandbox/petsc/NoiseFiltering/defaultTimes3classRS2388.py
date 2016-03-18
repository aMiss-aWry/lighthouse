# author : Kanika Sood 
# Date : Feb 13

# default timings available for 256 matrices (default solver: gmres + ilu , solver id: 32168839)
#input file: '/Users/kanikas/Desktop/petsc_anamod_35.csv' (File has all features + solver + class)
#output file: '/Users/kanikas/Desktop/solver_pc.csv' (File has all features + solver + solver_name + pc_name + class ) manually removing solver from the list for now
#input for this script is generated from CsvComparison.java

import csv
from collections import OrderedDict
from itertools import islice 
import operator


			
#Open and read the solver unique numbers from solverids_names.csv and make them the key of the dictionary
uniques_ids = {}
default_time = {}

with open('/Users/kanikas/Desktop/Feb22_MisPredictionAnalysis/3classlabelling/RS2/RFRS2combined21505_feb22.csv', 'r+') as csvinput1:
	with open('/Users/kanikas/Desktop/Feb22_MisPredictionAnalysis/3classlabelling/defaultTimings777all.csv', 'r+') as csvinput2:

#with open('/Users/kanikas/Desktop/Feb22_MisPredictionAnalysis/combined21505_feb22.csv', 'r+') as csvinput:
		infile1 = csv.reader(csvinput1)
		infile2 = csv.reader(csvinput2) #all default timings
		csvoutput = '/Users/kanikas/Desktop/Feb22_MisPredictionAnalysis/3classlabelling/RS2/RS2default_Time388.csv'
		with open(csvoutput,'w') as csvoutput:
			writer = csv.writer(csvoutput)
			header= infile1.__next__() 
			#print(header)
			writer.writerow(header + ['default_time']) #write the header to the new decoupled file before writing the feature values
			for row1 in islice(infile1, 1, None):
				for row2 in islice(infile2, 1, None):
					#print(row1[9], row2[9])
					if row1[9] == row2[9] : #67 #same matrix
						print("yes")
						default_time[row1[9]] = row2[10]
					writer.writerow(row1)
print(default_time)
with open('/Users/kanikas/Desktop/Feb22_MisPredictionAnalysis/3classlabelling/RS2/RS2default_Time388.csv', 'r+') as csvinput:
		infile = csv.reader(csvinput)
		csvoutput = '/Users/kanikas/Desktop/Feb22_MisPredictionAnalysis/3classlabelling/RS2TimeComparisonWithDefault388.csv'
		with open(csvoutput,'w') as csvout:
			writer = csv.writer(csvout)
			infile = csv.reader(csvinput)
			writer.writerow(header + ['default_time'] + ['Time Difference'] + ['Comparison'] + ['SpeedUp'])
			diff = 0.0
			bad=0
			good=0
			for row in infile:
				for key in default_time:
					if row[9] == key  :
						print(key)
						speed_up = float(default_time[key])/float(row[11])
						diff = round(((float(row[11]) - float(default_time[key]))/float(default_time[key]))*100,2)
						if diff>0:
							bad+=1
							ext = " % more time of default solver time"
						else:
							good+=1
							ext = " % less time of default solver time"
						diff1 = str(diff) + ext

						writer.writerow(row + [default_time[key]]) + [diff] + [diff1] + [speed_up])
		print("We did better than default solver in ",good,"many cases out of ",(good+bad))
		print(good,bad, (good+bad))
		print("Result in file -->", csvoutput)

#writing only for good class labels
with open('/Users/kanikas/Desktop/Feb22_MisPredictionAnalysis/3classlabelling/RS2TimeComparisonWithDefault388.csv', 'r+') as csvinput:
		infile = csv.reader(csvinput)
		csvoutput = '/Users/kanikas/Desktop/Feb22_MisPredictionAnalysis/3classlabelling/RS2TimeComparisonWithDefaultForGood388.csv'
		with open(csvoutput,'w') as csvout:
			writer = csv.writer(csvout)
			infile = csv.reader(csvinput)
			writer.writerow(header + ['default_time'] + ['Time Difference'] + ['Comparison'] + ['SpeedUp'])
			for row in islice(infile, 1, None):
				if row[9] == "good" :
					writer.writerow(row)
		print("Result for only good labels in file -->", csvoutput)



 		