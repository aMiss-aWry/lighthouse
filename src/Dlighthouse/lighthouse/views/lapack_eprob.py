import string, types, sys, os, StringIO, re, shlex, json, zipfile
from collections import OrderedDict
from django.contrib.auth.decorators import login_required
from django.core.servers.basehttp import FileWrapper
from django.http import HttpResponse
from django.shortcuts import render_to_response, redirect, render
from django.template import RequestContext
from django.template.loader import render_to_string
from django.views.decorators.csrf import csrf_exempt
from lighthouse.forms.lapack_eprob import *
from lighthouse.models.lapack_eprob import *

import datetime


def findAnsweredQuestions(answered_questions):
    results = OrderedDict()
    for key,value in answered_questions.items():
        field_label, field_choices = eprob_fields[key]
        for shortval, longval in field_choices:
            if value == shortval:
                results.update({field_label:longval})
    return results



def lapack_eprob(request):
    answered_temp = OrderedDict()
    formname = ''
    if request.method == 'POST':
        formname = request.session['eprob_current_form']
        if formname != 'finish':
            form = SimpleForm(formname,request.POST)
            if form.is_valid():
                 request.session['eprob_form_' + formname] = form.cleaned_data[formname]

   
    else:
        try :
            formname = request.session['eprob_current_form']
        except KeyError:
            request.session['eprob_current_form'] = 'start'
            formname = 'start'

        if formname == 'start':            
            for key, _ in eprob_fields.items():
                try:
                    del request.session['eprob_form_' + key]
                except KeyError:
                    continue

    for key,_ in eprob_fields.items():
        label = 'eprob_form_' + key
        try :
            name = request.session[label]
        except KeyError:
            continue
        answered_temp.update({key : name})


    answered = findAnsweredQuestions(answered_temp)
    results = getFilteredList(answered_temp)

    context = {
            'simple_answered' : answered,
            'advanced_query' : 'lighthouse/lapack_eprob/advanced/multichoice.html',
            'advanced_form' : AdvancedForm,    
            'results' : results,
            'content_eprob_simpleSearch' :   'lighthouse/lapack_eprob/simple/landing.html',
            'content_eprob_advancedSearch' : 'lighthouse/lapack_eprob/advanced/landing.html',
            'content_eprob_keywordSearch' : 'lighthouse/lapack_eprob/keyword/search.html'
    }

    nextform = findNextForm(results,answered_temp)

    if nextform != 'finish':
        request.session['eprob_current_form'] = nextform
        context.update({
                        'content_eprob_guided_form':'lighthouse/lapack_eprob/simple/guided.html', 
                        'simple_form' : FilteredForm(nextform,results),
                        'content_eprob_guided_buttons' : 'lighthouse/lapack_eprob/simple/buttons_more.html'
                        })
    else:
        context.update({
                        'content_eprob_guided_form':'lighthouse/lapack_eprob/simple/finished.html', 
                        })
 
    return render_to_response(
    'lighthouse/lapack_eprob/index.html',
            context_instance=RequestContext(request, context)
    )


@csrf_exempt
def eprob_clear(request):
    if request.is_ajax():
#      mode = [{"clear": request.POST.get('clear')}]
        request.session['eprob_current_form'] = 'start'
        for key, _ in eprob_fields.items():
            try:
                del request.session['eprob_form_' + key]
            except KeyError:
                continue
        return HttpResponse('cleared')              
    else:
        return HttpResponse('only AJAX requests are allowed!')
