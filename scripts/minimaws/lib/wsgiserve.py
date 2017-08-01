#!/usr/bin/python
##
## license:BSD-3-Clause
## copyright-holders:Vas Crabb

from . import dbaccess
from . import htmltmpl

import cgi
import inspect
import mimetypes
import os.path
import sys
import wsgiref.simple_server
import wsgiref.util

if sys.version_info > (3, ):
    import urllib.parse as urlparse
else:
    import urlparse


class HandlerBase(object):
    STATUS_MESSAGE = {
            400: 'Bad Request',
            401: 'Unauthorized',
            403: 'Forbidden',
            404: 'Not Found',
            405: 'Method Not Allowed',
            500: 'Internal Server Error',
            501: 'Not Implemented',
            502: 'Bad Gateway',
            503: 'Service Unavailable',
            504: 'Gateway Timeout',
            505: 'HTTP Version Not Supported' }

    def __init__(self, app, application_uri, environ, start_response, **kwargs):
        super(HandlerBase, self).__init__(**kwargs)
        self.app = app
        self.application_uri = application_uri
        self.environ = environ
        self.start_response = start_response

    def error_page(self, code):
        yield htmltmpl.ERROR_PAGE.substitute(code=cgi.escape('%d' % code), message=cgi.escape(self.STATUS_MESSAGE[code])).encode('utf-8')


class ErrorPageHandler(HandlerBase):
    def __init__(self, code, app, application_uri, environ, start_response, **kwargs):
        super(ErrorPageHandler, self).__init__(app=app, application_uri=application_uri, environ=environ, start_response=start_response, **kwargs)
        self.code = code
        self.start_response('%d %s' % (self.code, self.STATUS_MESSAGE[code]), [('Content-type', 'text/html; charset=utf-8'), ('Cache-Control', 'public, max-age=3600')])

    def __iter__(self):
        return self.error_page(self.code)


class AssetHandler(HandlerBase):
    def __init__(self, directory, app, application_uri, environ, start_response, **kwargs):
        super(AssetHandler, self).__init__(app=app, application_uri=application_uri, environ=environ, start_response=start_response, **kwargs)
        self.directory = directory
        self.asset = wsgiref.util.shift_path_info(environ)

    def __iter__(self):
        if not self.asset:
            self.start_response('403 %s' % (self.STATUS_MESSAGE[403], ), [('Content-type', 'text/html; charset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
            return self.error_page(403)
        elif self.environ['PATH_INFO']:
            self.start_response('404 %s' % (self.STATUS_MESSAGE[404], ), [('Content-type', 'text/html; charset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
            return self.error_page(404)
        else:
            path = os.path.join(self.directory, self.asset)
            if not os.path.isfile(path):
                self.start_response('404 %s' % (self.STATUS_MESSAGE[404], ), [('Content-type', 'text/html; charset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
                return self.error_page(404)
            elif self.environ['REQUEST_METHOD'] != 'GET':
                self.start_response('405 %s' % (self.STATUS_MESSAGE[405], ), [('Content-type', 'text/html; charset=utf-8'), ('Accept', 'GET, HEAD, OPTIONS'), ('Cache-Control', 'public, max-age=3600')])
                return self.error_page(405)
            else:
                try:
                    f = open(path, 'rb')
                    type, encoding = mimetypes.guess_type(path)
                    self.start_response('200 OK', [('Content-type', type or 'application/octet-stream'), ('Cache-Control', 'public, max-age=3600')])
                    return wsgiref.util.FileWrapper(f)
                except:
                    self.start_response('500 %s' % (self.STATUS_MESSAGE[500], ), [('Content-type', 'text/html; charset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
                    return self.error_page(500)


class QueryPageHandler(HandlerBase):
    def __init__(self, app, application_uri, environ, start_response, **kwargs):
        super(QueryPageHandler, self).__init__(app=app, application_uri=application_uri, environ=environ, start_response=start_response, **kwargs)
        self.dbcurs = app.dbconn.cursor()

    def machine_href(self, shortname):
        return cgi.escape(urlparse.urljoin(self.application_uri, 'machine/%s' % (shortname, )), True)

    def sourcefile_href(self, sourcefile):
        return cgi.escape(urlparse.urljoin(self.application_uri, 'sourcefile/%s' % (sourcefile, )), True)


class MachineHandler(QueryPageHandler):
    def __init__(self, app, application_uri, environ, start_response, **kwargs):
        super(MachineHandler, self).__init__(app=app, application_uri=application_uri, environ=environ, start_response=start_response, **kwargs)
        self.shortname = wsgiref.util.shift_path_info(environ)

    def __iter__(self):
        if not self.shortname:
            # could probably list machines here or something
            self.start_response('403 %s' % (self.STATUS_MESSAGE[403], ), [('Content-type', 'text/html; charset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
            return self.error_page(403)
        elif self.environ['PATH_INFO']:
            # subdirectory of a machine
            self.start_response('404 %s' % (self.STATUS_MESSAGE[404], ), [('Content-type', 'text/html; charset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
            return self.error_page(404)
        else:
            machine_info = self.dbcurs.get_machine_info(self.shortname).fetchone()
            if not machine_info:
                self.start_response('404 %s' % (self.STATUS_MESSAGE[404], ), [('Content-type', 'text/html; charset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
                return self.error_page(404)
            elif self.environ['REQUEST_METHOD'] != 'GET':
                self.start_response('405 %s' % (self.STATUS_MESSAGE[405], ), [('Content-type', 'text/html; charset=utf-8'), ('Accept', 'GET, HEAD, OPTIONS'), ('Cache-Control', 'public, max-age=3600')])
                return self.error_page(405)
            else:
                self.start_response('200 OK', [('Content-type', 'text/html; chearset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
                return self.machine_page(machine_info)

    def machine_page(self, machine_info):
        description = machine_info['description']
        yield htmltmpl.MACHINE_PROLOGUE.substitute(
                assets=cgi.escape(urlparse.urljoin(self.application_uri, 'static'), True),
                sourcehref=self.sourcefile_href(machine_info['sourcefile']),
                description=cgi.escape(description),
                shortname=cgi.escape(self.shortname),
                isdevice=cgi.escape('Yes' if machine_info['isdevice'] else 'No'),
                runnable=cgi.escape('Yes' if machine_info['runnable'] else 'No'),
                sourcefile=cgi.escape(machine_info['sourcefile'])).encode('utf-8')
        if machine_info['year'] is not None:
            yield (
                    '    <tr><th>Year:</th><td>%s</td></tr>\n' \
                    '    <tr><th>Manufacturer:</th><td>%s</td></tr>\n' %
                    (cgi.escape(machine_info['year']), cgi.escape(machine_info['Manufacturer']))).encode('utf-8')
        if machine_info['cloneof'] is not None:
            parent = self.dbcurs.listfull(machine_info['cloneof']).fetchone()
            if parent:
                yield (
                        '    <tr><th>Parent Machine:</th><td><a href="%s">%s (%s)</a></td></tr>\n' %
                        (cgi.escape('%s/machine/%s' % (self.application_uri, machine_info['cloneof']), True), cgi.escape(parent[1]), cgi.escape(machine_info['cloneof']))).encode('utf-8')
            else:
                yield (
                        '    <tr><th>Parent Machine:</th><td><a href="%s">%s</a></td></tr>\n' %
                        (cgi.escape('%s/machine/%s' % (self.application_uri, machine_info['cloneof']), True), cgi.escape(machine_info['cloneof']))).encode('utf-8')
        if (machine_info['romof'] is not None) and (machine_info['romof'] != machine_info['cloneof']):
            parent = self.dbcurs.listfull(machine_info['romof']).fetchone()
            if parent:
                yield (
                        '    <tr><th>Parent ROM set:</th><td><a href="%s">%s (%s)</a></td></tr>\n' %
                        (cgi.escape('%s/machine/%s' % (self.application_uri, machine_info['romof']), True), cgi.escape(parent[1]), cgi.escape(machine_info['romof']))).encode('utf-8')
            else:
                yield (
                        '    <tr><th>Parent Machine:</th><td><a href="%s">%s</a></td></tr>\n' %
                        (cgi.escape('%s/machine/%s' % (self.application_uri, machine_info['romof']), True), cgi.escape(machine_info['romof']))).encode('utf-8')
        yield '</table>\n'.encode('utf-8')

        first = True
        for name, desc, src in self.dbcurs.get_devices_referenced(machine_info['id']):
            if first:
                yield \
                        '<h2>Devices Referenced</h2>\n' \
                        '<table id="tbl-dev-refs">\n' \
                        '    <thead>\n' \
                        '        <tr><th>Short name</th><th>Description</th><th>Source file</th></tr>\n' \
                        '    </thead>\n' \
                        '    <tbody>\n'.encode('utf-8')
                first = False
            yield self.machine_row(name, desc, src)
        if not first:
            yield '    </tbody>\n</table>\n<script>make_table_sortable(document.getElementById("tbl-dev-refs"));</script>\n'.encode('utf-8')

        first = True
        for name, desc, src in self.dbcurs.get_device_references(self.shortname):
            if first:
                yield \
                        '<h2>Referenced By</h2>\n' \
                        '<table id="tbl-ref-by">\n' \
                        '    <thead>\n' \
                        '        <tr><th>Short name</th><th>Description</th><th>Source file</th></tr>\n' \
                        '    </thead>\n' \
                        '    <tbody>\n'.encode('utf-8')
                first = False
            yield self.machine_row(name, desc, src)
        if not first:
            yield '    </tbody>\n</table>\n<script>make_table_sortable(document.getElementById("tbl-ref-by"));</script>\n'.encode('utf-8')

        yield '</html>\n'.encode('utf-8')

    def machine_row(self, shortname, description, sourcefile):
        return (htmltmpl.MACHINE_ROW if description is not None else htmltmpl.EXCL_MACHINE_ROW).substitute(
                machinehref=self.machine_href(shortname),
                sourcehref=self.sourcefile_href(sourcefile),
                shortname=cgi.escape(shortname),
                description=cgi.escape(description or ''),
                sourcefile=cgi.escape(sourcefile or '')).encode('utf-8')


class SourceFileHandler(QueryPageHandler):
    def __init__(self, app, application_uri, environ, start_response, **kwargs):
        super(SourceFileHandler, self).__init__(app=app, application_uri=application_uri, environ=environ, start_response=start_response, **kwargs)

    def __iter__(self):
        self.filename = self.environ['PATH_INFO']
        if self.filename and (self.filename[0] == '/'):
            self.filename = self.filename[1:]
        if not self.filename:
            if self.environ['REQUEST_METHOD'] != 'GET':
                self.start_response('405 %s' % (self.STATUS_MESSAGE[405], ), [('Content-type', 'text/html; charset=utf-8'), ('Accept', 'GET, HEAD, OPTIONS'), ('Cache-Control', 'public, max-age=3600')])
                return self.error_page(405)
            else:
                self.start_response('200 OK', [('Content-type', 'text/html; chearset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
                return self.sourcefile_listing_page(None)
        else:
            id = self.dbcurs.get_sourcefile_id(self.filename)
            if id is None:
                if ('*' not in self.filename) and ('?' not in self.filename) and ('?' not in self.filename):
                    self.filename += '*' if self.filename[-1] == '/' else '/*'
                    if not self.dbcurs.count_sourcefiles(self.filename):
                        self.start_response('404 %s' % (self.STATUS_MESSAGE[404], ), [('Content-type', 'text/html; charset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
                        return self.error_page(404)
                    elif self.environ['REQUEST_METHOD'] != 'GET':
                        self.start_response('405 %s' % (self.STATUS_MESSAGE[405], ), [('Content-type', 'text/html; charset=utf-8'), ('Accept', 'GET, HEAD, OPTIONS'), ('Cache-Control', 'public, max-age=3600')])
                        return self.error_page(405)
                    else:
                        self.start_response('200 OK', [('Content-type', 'text/html; chearset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
                        return self.sourcefile_listing_page(self.filename)
                else:
                    self.start_response('404 %s' % (self.STATUS_MESSAGE[404], ), [('Content-type', 'text/html; charset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
                    return self.error_page(404)
            elif self.environ['REQUEST_METHOD'] != 'GET':
                self.start_response('405 %s' % (self.STATUS_MESSAGE[405], ), [('Content-type', 'text/html; charset=utf-8'), ('Accept', 'GET, HEAD, OPTIONS'), ('Cache-Control', 'public, max-age=3600')])
                return self.error_page(405)
            else:
                self.start_response('200 OK', [('Content-type', 'text/html; chearset=utf-8'), ('Cache-Control', 'public, max-age=3600')])
                return self.sourcefile_page(id)

    def sourcefile_listing_page(self, pattern):
        if not pattern:
            title = heading = 'All Source Files'
        else:
            heading = self.linked_title(pattern)
            title = 'Source Files: ' + cgi.escape(pattern)
        yield htmltmpl.SOURCEFILE_LIST_PROLOGUE.substitute(
                assets=cgi.escape(urlparse.urljoin(self.application_uri, 'static'), True),
                title=title,
                heading=heading).encode('utf-8')
        for filename, machines in self.dbcurs.get_sourcefiles(pattern):
            yield htmltmpl.SOURCEFILE_LIST_ROW.substitute(
                    sourcefile=self.linked_title(filename, True),
                    machines=cgi.escape('%d' % machines)).encode('utf-8')
        yield '    </tbody>\n</table>\n<script>make_table_sortable(document.getElementById("tbl-sourcefiles"));</script>\n</body>\n</html>\n'.encode('utf-8')

    def sourcefile_page(self, id):
        yield htmltmpl.SOURCEFILE_PROLOGUE.substitute(
                assets=cgi.escape(urlparse.urljoin(self.application_uri, 'static'), True),
                filename=cgi.escape(self.filename),
                title=self.linked_title(self.filename)).encode('utf-8')

        first = True
        for machine_info in self.dbcurs.get_sourcefile_machines(id):
            if first:
                yield \
                        '<table id="tbl-machines">\n' \
                        '    <thead>\n' \
                        '        <tr>\n' \
                        '            <th>Short name</th>\n' \
                        '            <th>Description</th>\n' \
                        '            <th>Year</th>\n' \
                        '            <th>Manufacturer</th>\n' \
                        '            <th>Runnable</th>\n' \
                        '            <th>Parent</th>\n' \
                        '        </tr>\n' \
                        '    </thead>\n' \
                        '    <tbody>\n'.encode('utf-8')
                first = False
            yield self.machine_row(machine_info)
        if first:
            yield '<p>No machines found.</p>\n'.encode('utf-8')
        else:
            yield '    </tbody>\n</table>\n<script>make_table_sortable(document.getElementById("tbl-machines"));</script>\n'.encode('utf-8')

        yield '</body>\n</html>\n'.encode('utf-8')

    def linked_title(self, filename, linkfinal=False):
        parts = filename.split('/')
        final = parts[-1]
        del parts[-1]
        uri = urlparse.urljoin(self.application_uri, 'sourcefile')
        title = ''
        for part in parts:
            uri = urlparse.urljoin(uri + '/', part)
            title += '<a href="{0}">{1}</a>/'.format(cgi.escape(uri, True), cgi.escape(part))
        if linkfinal:
            uri = urlparse.urljoin(uri + '/', final)
            return title + '<a href="{0}">{1}</a>'.format(cgi.escape(uri, True), cgi.escape(final))
        else:
            return title + final

    def machine_row(self, machine_info):
        return (htmltmpl.SOURCEFILE_ROW_PARENT if machine_info['cloneof'] is None else htmltmpl.SOURCEFILE_ROW_CLONE).substitute(
                machinehref=self.machine_href(machine_info['shortname']),
                parenthref=self.machine_href(machine_info['cloneof'] or '__invalid'),
                shortname=cgi.escape(machine_info['shortname']),
                description=cgi.escape(machine_info['description']),
                year=cgi.escape(machine_info['year'] or ''),
                manufacturer=cgi.escape(machine_info['manufacturer'] or ''),
                runnable=cgi.escape('Yes' if machine_info['runnable'] else 'No'),
                parent=cgi.escape(machine_info['cloneof'] or '')).encode('utf-8')


class MiniMawsApp(object):
    def __init__(self, dbfile, **kwargs):
        super(MiniMawsApp, self).__init__(**kwargs)
        self.dbconn = dbaccess.QueryConnection(dbfile)
        self.assetsdir = os.path.join(os.path.dirname(inspect.getfile(self.__class__)), 'assets')
        if not mimetypes.inited:
            mimetypes.init()

    def __call__(self, environ, start_response):
        application_uri = wsgiref.util.application_uri(environ)
        module = wsgiref.util.shift_path_info(environ)
        if module == 'machine':
            return MachineHandler(self, application_uri, environ, start_response)
        elif module == 'sourcefile':
            return SourceFileHandler(self, application_uri, environ, start_response)
        elif module == 'static':
            return AssetHandler(self.assetsdir, self, application_uri, environ, start_response)
        elif not module:
            return ErrorPageHandler(403, self, application_uri, environ, start_response)
        else:
            return ErrorPageHandler(404, self, application_uri, environ, start_response)


def run_server(options):
    application = MiniMawsApp(options.database)
    server = wsgiref.simple_server.make_server(options.host, options.port, application)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
