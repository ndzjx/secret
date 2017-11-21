import sys
import base64
import poplib
import email
from email.parser import Parser
from email.header import decode_header
from email.utils import parseaddr

if len( sys.argv ) != 9 :
	sys.exit( 400 )

var_pop3 = sys.argv[ 1 ]
var_user = sys.argv[ 2 ]
var_pawd = sys.argv[ 3 ]
var_uidl = sys.argv[ 4 ]
var_file = sys.argv[ 5 ]
var_beg  = sys.argv[ 6 ]
var_end  = sys.argv[ 7 ]
var_seek = sys.argv[ 8 ]

def decode_str( p ):
	value, charset = decode_header( p )[ 0 ]
	if charset:
		value = value.decode( charset )
	return value

try:
	server = poplib.POP3( var_pop3 )
	server.user( var_user )
	server.pass_( var_pawd )
	lines = server.retr( var_uidl )[ 1 ]
	
	body = ''
	for n in lines:
		body += n.decode( 'utf-8' )
		body += '\n'

	msg = Parser().parsestr( body )

	f = open( var_file, 'rb+' )
	f.seek( int( var_seek ) )
	f.write( base64.b64decode( msg.get_payload() )[ int( var_beg ) : int( var_end ) ] )
	f.close()
	
	server.quit()

except Exception as e:
	sys.exit( 500 )

sys.exit( 200 )
