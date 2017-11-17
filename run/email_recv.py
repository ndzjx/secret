import sys
import poplib
import email
from email.parser import Parser
from email.header import decode_header
from email.utils import parseaddr

if len( sys.argv ) != 5 :
	sys.exit( 400 )

var_pop3 = sys.argv[ 1 ]
var_user = sys.argv[ 2 ]
var_pawd = sys.argv[ 3 ]
var_uidl = sys.argv[ 4 ]

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
	pl = msg.get_payload()
	pl = pl.replace( '\n', '' )
	print( pl, end = '' )

	server.quit()
except Exception as e:
	sys.exit( 500 )

sys.exit( 200 )
