import sys
import smtplib
from email import encoders
from email.header import Header
from email.mime.text import MIMEText
from email.utils import parseaddr, formataddr

if len( sys.argv ) != 7 :
	sys.exit( 400 )

var_smtp = sys.argv[ 1 ]
var_password = sys.argv[ 2 ]
var_from = sys.argv[ 3 ]
var_to = sys.argv[ 4 ]
var_subject = sys.argv[ 5 ]
var_body = ''

with open(sys.argv[ 6 ], 'rb') as f:
	var_body = f.read()

def transformaddr( p ) :
    name, addr = parseaddr( p )
    return formataddr( ( Header( name, 'utf-8' ).encode(), addr ) )

msg = MIMEText( var_body, 'plain', 'utf-8' )
msg['From'] = transformaddr( 'secret <%s>' % var_from )
msg['To'] = transformaddr( 'msg <%s>' % var_to )
msg['Subject'] = Header( var_subject, 'utf-8' ).encode()

try:
	server = smtplib.SMTP( var_smtp, 25 )
	server.login( var_from, var_password )
	server.sendmail( var_from, [var_to], msg.as_string() )
	server.quit()
except Exception as e:
	sys.exit( 500 )

sys.exit( 200 )
