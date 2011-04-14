var 
	sys = require('sys')
	, zlib = require('../build/default/zlib')
	, b = new Buffer('hello world')
	, cb = zlib.deflateBuffer(b)
	, ib = zlib.inflateBuffer(cb)
	;

sys.debug(ib);

