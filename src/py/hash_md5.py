import sys
import hashlib

if len( sys.argv ) != 2 :
	sys.exit( 400 )

var_file = sys.argv[ 1 ]

md5 = hashlib.md5()

with open( var_file, 'rb' ) as f:
	while f:
		bits = f.read( 1024 * 1024 )
		if not bits:
			break
		md5.update( bits )

	print( md5.hexdigest().upper(), end = '' )
	sys.exit( 200 )

sys.exit( 10 )
