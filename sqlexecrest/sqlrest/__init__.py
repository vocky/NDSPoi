# -*- encoding: utf-8 -*-
# We'll render HTML templates and access data sent by POST
# using the request object from flask
import flask
from flask import Flask, request, abort, make_response, current_app, render_template
import poi_search
import os
# Initialize the Flask application

__version__ = '1.0.0'

def jsonreq():
    # Get the JSON data sent from the form
    #     jsondata = request.form['jsondata']
    #     #data = timolib.decodingjson(jsondata)
    #     data = jsondata
    #     return render_template('request.html', data=data, jsondata=jsondata)
    timolibrary = current_app.timolibrary
    content_type = request.headers['Content-Type']
    if content_type == 'application/json':
        return timolibrary.decoding2json(request.get_data())
    else:
        abort(make_response("Content-Type Error", 400))            

def describe(app):
    """get general descriptions of the current instance"""
    rtn = dict(
        version=__version__,
        flask=flask.__version__,
        # XXX: no flask-restful info
        config=dict((k, v) for k, v in app.config.items()\
                    if k.startswith('SQL_')),
    )
    return rtn

def description():
    return flask.jsonify(**flask.current_app.description)

def index():
    if request.method == 'POST':
        search_info = request.form.get('search_input')
        timolibrary = current_app.timolibrary
        result_info = timolibrary.decoding2json(search_info.encode("utf-8"))
        result_info = unicode(result_info, "utf-8")
    else:
        search_info = u'''{"match":"星巴克","city":310000}'''
        result_info = "None"
    return render_template('index.html', url_back='/index/', search_info=search_info, result_info=result_info)

def create_app(config=None):
    """Create timorest application
    Config tries the following:
    1. 'config' parameter
    2. 'TIMO_APP_SETTINGS' from the environment variable
    3. 'settings.cfg' file in current path
    
    All config prefixed with 'TIMO_' can be overridden by direct envvar
    """
    app = Flask(__name__)
    
    app.config.from_object('sqlrest.default_settings')
    
    if config:
        if isinstance(config, dict):
            app.config.update(config)
        else:
            app.config.from_pyfile(config)
    else:
        app.config.from_envvar('TIMO_APP_SETTINGS', silent=True)
        
    env_override = dict((k, v) for k, v in os.environ.items() \
                        if k.startswith('SQL_'))
    app.config.update(env_override)

    @app.route('/')
    def form():
        result_info = 'adbc'
        return render_template('index.html', result_info=result_info)
    
    timolibrary = poi_search.TimoLibrary(app.config)

# This route will show a form to submit some JSON data
# This route will accept a request containing JSON
# Then we'll convert that data into Python a structure
# and print it.
# Because we used a standard HTML form post to send the
# data, we need to get the JSON from request.form
# If on other hand the information was sent from an app,
# or even a python urllib2.Request we would need to use
# request.data to get the JSON string
    app.timolibrary = timolibrary
    app.add_url_rule('/timo', 'jsonreq', jsonreq, methods=['POST',])
    app.description = describe(app)
    app.add_url_rule('/describe', 'describe', description)
    app.add_url_rule('/health_check', 'describe')  # alias
    app.add_url_rule('/index/', 'index', index, methods=['GET','POST'])  # alias
    return app
