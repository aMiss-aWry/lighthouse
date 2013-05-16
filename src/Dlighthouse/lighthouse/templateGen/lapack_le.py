import os, fnmatch, re

from lighthouse.models.lapack_le import lapack_le_arg

fortran_path = './lighthouse/templateGen/fortran/'


routine_task = {'sv': 'solve Ax = B',
               'rf': 'factor matrix A',
               'on': 'estimate the reciprocal of the condition number of matrix A',}


class generateTemplate(object):
    __name__ = 'generateTemplate'
    
    def __init__(self, routineName):
        self.routineName = routineName
        
        
    def get_dataType(self):
        dataType = []
        if self.routineName.startswith('s'):
            dataType.extend(['REAL', 'KIND=4'])
        elif self.routineName.startswith('d'):
            dataType.extend(['REAL', 'KIND=8'])
        elif self.routineName.startswith('c'):
            dataType.extend(['COMPLEX', 'KIND=4'])
        elif self.routineName.startswith('z'):
            dataType.extend(['COMPLEX', 'KIND=8'])
        return dataType

            
        
    def make_template(self):
        ### --- get data from database --- ### 
        ROUTINE = lapack_le_arg.objects.filter(routineName=self.routineName[1:])
        
        ### --- copy head.txt file to test1.f90 --- ###
        with open(fortran_path+"codeTemplates/test1.f90", "w") as f:
            with open(fortran_path+"baseCode/head_"+self.routineName[-2:]+".txt", "r") as f_head:
                for line in f_head.readlines():
                    f.write(line)
        
        ### --- create SUBROUTINE DIMNS_ASSIGNMENT --- ###           
            ## --- set up input question subprogram by reading from readQ.txt --- ##
            input_list = ROUTINE[0].param_in.split(',')
            f.write('\n\n')
            f.write('\tSUBROUTINE DIMNS_ASSIGNMENT\n')
            f.write('\t    USE Declaration\n')
            f.write('\t    !--- obtain inputs ---!\n')
            for item in input_list:
                flag = 1
                with open(fortran_path+"baseCode/readQ.txt", "r") as f_readQ:
                    for line in f_readQ.readlines():
                        if "begin %s\n"%item in line:
                            flag = 0
                        if "end %s\n"%item in line:
                            flag = 1
                        if not flag and not "begin %s\n"%item in line:
                           f.write(line)
                           
            ## --- set up ALLOCATE --- ##
            allocate_list = ROUTINE[0].allocate.split(";")
            f.write('\n')
            f.write('\t    !--- allocate matrix/array ---!\n')
            for item in allocate_list:
                f.write('\t   ALLOCATE(%s)\n'%item)
            f.write('\tEND SUBROUTINE DIMNS_ASSIGNMENT\n')
            
            
        ### --- create SUBROUTINE GET_DATA --- ###
            readData_list = ROUTINE[0].readData.split(';')
            f.write('\n\n')
            f.write('\tSUBROUTINE GET_DATA\n')
            f.write('\t    USE Declaration\n')
            f.write('\t    !--- read data from files ---!\n')
            if ROUTINE[0].readData_L:
                f.write("\t    IF (UPLO == 'U') THEN\n")
                f.write("\t        READ(11, *) %s\n"%readData_list[0])
                f.write("\t    ELSE IF (UPLO == 'L') THEN\n")
                f.write("\t        READ(11, *) %s\n"%ROUTINE[0].readData_L.strip('\"'))
                f.write("\t    END IF\n\n")
            else:
                for item in readData_list:
                    f.write('\t    READ(11, *) %s\n\n'%item)

            if 'B' in ROUTINE[0].param_inout.split(','):
                f.write('\t    !--- read data from file for B ---!\n')
                f.write('\t    READ(99, *) ((B(I,J),J=1,NRHS),I=1,LDB)\n')
                
            f.write('\tEND SUBROUTINE GET_DATA\n')
            
            
        ### --- Combine with tail.txt file--- ###
        with open(fortran_path+"codeTemplates/test1.f90", "a") as f:
            with open(fortran_path+"baseCode/tail_"+self.routineName[-2:]+".txt", "r") as f_tail:
                for line in f_tail.readlines():
                    f.write(line)            

            
        ### --- final fixes --- ###
        ## --- set up format number for printing matrix --- ##
        if self.routineName.startswith('s') or self.routineName.startswith('d'):
            fmtNum = '11100'
        else:
            fmtNum = '22200'
            
        ## --- create a dictionary for replacing strings in the original file. --- ##
        replaceDict = {'routineName': self.routineName,
                       'routineTask': routine_task[self.routineName[-2:]],
                       'routine_parameters': ROUTINE[0].param_all,
                       'dataType': self.get_dataType()[0],
                       'KIND=': self.get_dataType()[1],
                       'integer_list': ROUTINE[0].integers,
                       'array_1D_int_list': ROUTINE[0].array_1d_int,
                       'character_list': ROUTINE[0].char,
                       'real_1D_list': ROUTINE[0].array_1d_real,
                       'matrix_list': ROUTINE[0].matrix,
                       'array_1D_list': ROUTINE[0].array_1d,
                       'LDA_condition': ROUTINE[0].LDA_condition,
                       'ALLOCATE_list': ROUTINE[0].allocate_list,
                       'fmtNum': fmtNum,
                       }

        with open(fortran_path+"codeTemplates/test2.f90", "wt") as fout:
            with open(fortran_path+"codeTemplates/test1.f90", "r") as fini:
                fout.write(replacemany(replaceDict, fini.read()))


        ### --- delete lines containing empty allocate lists and copy test2.f90 to the final temp_xxxxx.f90 file. --- ### 
        f_read = open(fortran_path+"codeTemplates/test2.f90", "r")
        lines = f_read.readlines()
        f_read.close()
        f_write = open(fortran_path+"codeTemplates/temp_%s.f90"%self.routineName,"w")
        for line in lines:
            if ':: \n' not in line:
                f_write.write(line)
        
                
        ### --- remove test files --- ###
        os.remove(fortran_path+'codeTemplates/test1.f90')
        os.remove(fortran_path+'codeTemplates/test2.f90')





def replacemany(adict, astring):
    pat = '|'.join(re.escape(s) for s in adict)
    there = re.compile(pat)
    def onerepl(mo):
        return adict[mo.group()]
    return there.sub(onerepl, astring)
