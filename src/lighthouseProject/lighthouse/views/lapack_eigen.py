import string, types, sys, os, StringIO, re, shlex, json, zipfile
from collections import OrderedDict
from django.contrib.auth.decorators import login_required
from django.core.servers.basehttp import FileWrapper
from django.http import HttpResponse, HttpResponseNotFound
from django.shortcuts import render_to_response, redirect, render
from django.template import RequestContext
from django.template.loader import render_to_string
from django.views.decorators.csrf import csrf_exempt
from lighthouse.forms.lapack_eigen import *
from lighthouse.models.lapack_eigen import lapack_eigen
from lighthouse.models.lapack_choiceDict import *

import datetime

##############################################
######--------- Guided Search --------- ######
##############################################

### help functions
def question_and_answer(form, value, choices):
    for field in form:
        question = unicode(field.label)
    for choice in choices:
        if choice[0] == value:
            answer = choice[1]
    return {question: [answer]}
    



def find_next_form(current_form, request):
    form_order = ('problemForm', 'standardGeneralizedForm', 'complexNumberForm', 'matrixTypeForm', 'storageTypeForm',
                  'selectedEVForm', 'eigenvectorForm', 'schurForm', 'cndNumberForm', 'singleDoubleForm')
    
    ## eigen problem for "general matrix" is special at the eigenvector question
    if 'matrixType' in request.session and 'eigenvector' in request.session:
        if request.session['matrixType'] == 'general' and request.session['eigenvector'] == 'no':
            nextForm_name = 'Schur'
            next_form = schurForm()
        elif equest.session['matrixType'] == 'general' and request.session['eigenvector'] == 'yes':
            nextForm_name = 'cndNumber'
            next_form = cndNumberForm()
    else:   
        current_index = form_order.index(current_form)
        nextForm_name = ""        
        next_form = ""
        try:
            next_index = next(i for i in range(current_index+1, len(form_order)) if request.session['Routines'].filter(**{form_order[i][:-4]: 'none'}).count() == 0)
            nextForm_name = form_order[next_index]
            if nextForm_name in['matrixTypeForm', 'storageTypeForm',]:
                next_form = getattr(sys.modules[__name__], nextForm_name)(request)
            else:
                next_form = getattr(sys.modules[__name__], nextForm_name)()        
        except Exception as e:          ## the end of the guided search, etc.
            print type(e)
            print "e.message: ", e.message
            print "e.args: ", e.args
    
    return {'nextForm_name': nextForm_name, 'nextForm': next_form}
    



### start guided search views
def guidedSearch_index(request):
    request.session['eigen_guided_answered'] = OrderedDict()
    request.session['currentForm_name'] = 'problemForm'
    request.session['Routines'] = lapack_eigen.objects.all()
    context = {
                'formHTML': "problemForm",
                'form': "invalid",
                'eigen_guided_answered' : '',
                'results' : 'start'
    }
    return render_to_response('lighthouse/lapack_eigen/index.html', context_instance=RequestContext(request, context))




def guidedSearch(request):
    ## distinguish forms that take 2 arguments and that take 1 argument.
    if request.session['currentForm_name'] in ['matrixTypeForm', 'storageTypeForm']:
        form = getattr(sys.modules[__name__], request.session['currentForm_name'])(request, request.GET or None)   #handle GET and POST in the same view
    else:
        form = getattr(sys.modules[__name__], request.session['currentForm_name'])(request.GET or None)
        
    if form.is_valid():
        ## get current question and user's answer
        current_question = request.session['currentForm_name'][:-4]
        formField_name = 'eigen_'+current_question
        value = form.cleaned_data[formField_name]
        choices = form.fields[formField_name].choices        
        request.session['eigen_guided_answered'].update(question_and_answer(form, value, choices))
        
        ## do search based on the answer
        lookup = "%s__contains" % current_question
        query = {lookup : value}
        request.session['Routines'] = request.session['Routines'].filter(**query)
        
        ## generate a session for current question/answer
        request.session[formField_name] = value
        
        ## set up next form for next question
        dict_nextQuestion = find_next_form(type(form).__name__, request)           
        nextForm_name = dict_nextQuestion['nextForm_name']
        nextForm = dict_nextQuestion['nextForm']
        
        ## make next form current for request.session['currentForm_name']
        request.session['currentForm_name'] = nextForm_name 
        
        ## decide whether or not to use form HTML files (if help buttons are needed, use HTML file instead of form)
        if nextForm_name in ["standardGeneralizedForm"]:
            formHTML = nextForm_name
        else:
            formHTML = "invalid"
        
        ## get ready for the template       
        context = {
                    'formHTML': formHTML,
                    'form': nextForm,
                    'eigen_guided_answered' : request.session['eigen_guided_answered'],
                    'results' : request.session['Routines']
        }
    else:       
        form = problemForm() # An unbound form       
        context = {
                    'formHTML': "problemForm",
                    'form': "invalid",
                    'eigen_guided_answered' : '',
                    'results' : 'start'
        }
    return render_to_response('lighthouse/lapack_eigen/index.html', context_instance=RequestContext(request, context))



## ------------------------- Note: question order for different problem types in the guided search ------------------------##
#(1) eigen problem 
#    (a)'symmetric':            ['standardGeneralized', 'complexNumber', 'matrixTypeForm', 'storageTypeForm', 'selectedEVForm', 'eigenvectorForm', 'singleDoubleForm'],
#    (b)'Hermitian':            ['standardGeneralized', 'complexNumber', 'matrixTypeForm', 'storageTypeForm', 'selectedEVForm', 'eigenvectorForm', 'singleDoubleForm'],
#    (c)'SPD':                  ['standardGeneralized', 'complexNumber', 'matrixTypeForm', 'storageTypeForm', 'eigenvectorForm', 'singleDoubleForm'],
#    (d)'HPD':                  ['standardGeneralized', 'complexNumber', 'matrixTypeForm', 'storageTypeForm', 'eigenvectorForm', 'singleDoubleForm'],    
#    (e)'upper Hessenberg':     ['standardGeneralized', 'complexNumber', 'matrixTypeForm', 'storageTypeForm', 'eigenvectorForm', 'singleDoubleForm'],
#    (f)'general':              ['standardGeneralized', 'complexNumber', 'matrixTypeForm', 'storageTypeForm', 'eigenvectorForm', 'schurForm', 'cndNumberForm', 'singleDoubleForm'],   
#(2) upper Hessenberg:          ['standardGeneralized', 'complexNumber', 'matrixType', 'storageType', 'singleDouble']
#(3) generalized_to_standard:   ['complexNumber', 'matrixType', 'storageType', 'singleDouble']
#(4) conditionNumber:           ['standardGeneralized', 'complexNumber', 'matrixType', 'singleDouble']
#(5) balance:                   ['standardGeneralized', 'complexNumber', 'storageType', 'singleDouble']
## ------------------------------------------------------------------------------------------------------------------------##




##############################################
######-------- Advanced Search -------- ######
##############################################