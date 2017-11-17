import sys
import poplib

if len( sys.argv ) != 4 :
	sys.exit( 400 )

var_pop3 = sys.argv[ 1 ]
var_user = sys.argv[ 2 ]
var_pawd = sys.argv[ 3 ]

try:
	server = poplib.POP3( var_pop3 )
	server.user( var_user )
	server.pass_( var_pawd )
	stat = server.stat()
	print( '%s,%s' % stat, end = '' )
	server.quit()
except Exception as e:
	sys.exit( 500 )

sys.exit( 200 )
