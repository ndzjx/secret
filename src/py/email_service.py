
from wsgiref.simple_server import make_server

import sys
import threading
import base64
import poplib

import email
from email.parser import Parser
from email.header import decode_header
from email.utils import parseaddr

import smtplib
from email import encoders
from email.header import Header
from email.mime.text import MIMEText
from email.utils import parseaddr, formataddr

g_dict_pop3 = {}
g_dict_smtp = {}

class secret:
    def email_login_pop3( param ):
        try:
            var_pop3 = param[ 'pop3' ]
            var_user = param[ 'user' ]
            var_pawd = param[ 'pawd' ]

            g_dict_pop3[ var_user ] = poplib.POP3( var_pop3 )
            g_dict_pop3[ var_user ].user( var_user )
            g_dict_pop3[ var_user ].pass_( var_pawd )
            return '200 OK', [ ( 'ok' ).encode( 'utf-8' ) ]

        except Exception as e:
            return '400 Error', [ b'email_login_pop3 exception.' ]

    def email_login_smtp( param ):
        try:
            var_smtp = param[ 'smtp' ]
            var_user = param[ 'user' ]
            var_pawd = param[ 'pawd' ]

            g_dict_smtp[ var_user ] = smtplib.SMTP( var_smtp, 25 )
            g_dict_smtp[ var_user ].login( var_user, var_pawd )
            return '200 OK', [ ( 'ok' ).encode( 'utf-8' ) ]

        except Exception as e:
            return '400 Error', [ b'email_login_smtp exception.' ]

    def email_stat( param ):
        try:
            var_user = param[ 'user' ]
            stat = g_dict_pop3[ var_user ].stat()
            return '200 OK', [ ( '%s,%s' % stat ).encode( 'utf-8' ) ]

        except Exception as e:
            return '400 Error', [ b'email_stat exception.' ]

    def decode_str( p ):
        value, charset = decode_header( p )[ 0 ]
        if charset:
            value = value.decode( charset )
        return value

    def email_subject( param ):
        try:
            var_user = param[ 'user' ]
            var_uidl = param[ 'uidl' ]
            lines = g_dict_pop3[ var_user ].top( var_uidl, 1 )[ 1 ]

            body = ''
            for n in lines:
                body += n.decode( 'utf-8' )
                body += '\n'

            msg = Parser().parsestr( body )
            val = msg.get( 'Subject', '' )
            val = secret.decode_str( val )
            return '200 OK', [ ( val ).encode( 'utf-8' ) ]

        except Exception as e:
            return '400 Error', [ b'email_subject exception.' ]

    def email_recv( param ):
        try:
            var_user = param[ 'user' ]
            var_uidl = param[ 'uidl' ]
            var_file = param[ 'file' ]
            var_beg  = param[ 'beg' ]
            var_end  = param[ 'end' ]
            var_seek = param[ 'seek' ]
            lines = g_dict_pop3[ var_user ].retr( var_uidl )[ 1 ]

            body = ''
            for n in lines:
                body += n.decode( 'utf-8' )
                body += '\n'

            msg = Parser().parsestr( body )

            f = open( var_file, 'rb+' )
            f.seek( int( var_seek ) )
            f.write( base64.b64decode( msg.get_payload() )[ int( var_beg ) : int( var_end ) ] )
            f.close()
            return '200 OK', [ ( 'ok' ).encode( 'utf-8' ) ]

        except Exception as e:
            return '400 Error', [ b'email_recv exception.' ]

    def transformaddr( p ) :
        name, addr = parseaddr( p )
        return formataddr( ( Header( name, 'utf-8' ).encode(), addr ) )

    def email_send( param ):
        try:
            var_user = param[ 'user' ]
            var_to = param[ 'to' ]
            var_subject = param[ 'subject' ]
            var_file = param[ 'file' ]
            var_body = ''

            with open(var_file, 'rb') as f:
                var_body = f.read()

            msg = MIMEText( var_body, 'plain', 'utf-8' )
            msg['From'] = secret.transformaddr( 'secret <%s>' % var_user )
            msg['To'] = secret.transformaddr( 'msg <%s>' % var_to )
            msg['Subject'] = Header( var_subject, 'utf-8' ).encode()
            g_dict_smtp[ var_user ].sendmail( var_user, [var_to], msg.as_string() )
            return '200 OK', [ ( 'ok' ).encode( 'utf-8' ) ]

        except Exception as e:
            return '400 Error', [ b'email_send exception.' ]

def webcore( environ, start_response ):
    method = environ[ 'PATH_INFO' ].strip().strip('//')
    if hasattr( secret, method ) == False:
        start_response( '400 Error', [ ( 'Content-Type', 'text/html' ) ] )
        return [ b'error' ]

    qs = environ[ 'QUERY_STRING' ]
    if qs == False:
        start_response( '400 Error', [ ( 'Content-Type', 'text/html' ) ] )
        return [ b'error' ]

    param = {}
    for x in qs.split( '&' ):
        key, val = x.split( '=' )
        param[ key ] = val

    method = getattr( secret, method )
    ec, ret = method( param ) ;
    start_response( ec, [ ( 'Content-Type', 'text/html' ) ] )
    return ret

httpd = make_server( '', 8811, webcore )

def work_thread():
    httpd.serve_forever()

for x in range( 7 ):
    th = threading.Thread( target = work_thread, name = 'work_thread' )
    th.start()

httpd.serve_forever()
