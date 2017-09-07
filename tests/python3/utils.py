import collections
import numbers
import sys
import os

def concatValues(obj):
	if not obj:
		return "None"
	elif isinstance(obj, (str,bytes)):
		return obj
	elif isinstance(obj, collections.Sequence):
		rt = None
		for v in obj:
			if (rt):
				rt = rt + " " + str(v)
			else:
				rt = str(v)
	elif isinstance(obj, dict):
		rt = None
		for k,v in obj.items():
			if (rt):
				rt = rt + " " + str(v)
			else:
				rt = str(v)
		return rt
	else:
		return obj

def dumpValue_( obj, depth):
	if not obj:
		return "None"
	elif isinstance(obj, numbers.Number):
		if obj.is_integer():
			return "%d" % obj
		else:
			return "%.5f" % obj
	elif isinstance(obj, (str,bytes)):
		return '"' + obj + '"'
	elif isinstance(obj, collections.Sequence):
		if (depth == 0):
			return "{...}"
		s = '{ '
		i = 0
		for v in obj:
			if i > 0:
				s = s + ', '
			s = s + '[' + str(i) + '] = ' + dumpValue_( v, depth-1)
			i = i + 1
		return s + '} '
	elif isinstance(obj, dict):
		if (depth == 0):
			return "{...}"
		s = '{ '
		i = 0
		for k,v in obj.items():
			if i > 0:
				s = s + ', '
			i = i + 1
			if isinstance(obj, (str,bytes)):
				k = '"' + k + '"'
			s = s + '[' + str(k) + '] = ' + dumpValue_( v, depth-1)
		return s + '} '
	else:
		return str(obj)

def dumpValue( o):
	return dumpValue_( o, 10)

def dumpTree_( indent, obj, depth):
	if not obj:
		return "None"
	elif isinstance(obj, numbers.Number):
		if obj.is_integer():
			return "%d" % obj
		else:
			return "%.5f" % obj
	elif isinstance(obj, (str,bytes)):
		return '"' + obj + '"'
	elif isinstance(obj, collections.Sequence):
		if (depth == 0):
			return "{...}"
		s = None
		for v in obj:
			if s:
				s = s + ', '
			s = s + '[' + str(i) + '] = ' + dumpValue_( v, depth-1)
			i = i + 1
		return s
	elif isinstance(obj, dict):
		if (depth == 0):
			return "{...}"
		s = ''
		for k in sorted( obj.keys()):
			ke = "\n" + indent + " " + str(k)
			ve = dumpTree_( indent + '  ', obj[ k], depth-1)
			if ve[0] == "\n":
				s = s + ke + ":" + ve
			else:
				s = s + ke + ": " + ve
		return s
	else:
		return str(obj)

def dumpTree( obj):
	return dumpTree_( "", obj, 10)

def readFile( path):
	with open( path) as f:
		return f.read()

def writeFile( path, content):
	with open( path,"w") as f:
		return f.write( content)

def verifyTestOutput( outputdir, result, expected):
	if result != expected:
		writeFile( outputdir + "/RES", result)
		writeFile( outputdir + "/EXP", expected)

		print( "Failed")
		sys.exit( 1)
	else:
		print( "OK")

def getPathArray( filepath):
	return os.path.split( filepath)

def getFileParentDirectory( filepath):
	pathar = getPathArray( filepath)
	del( pathar[ -1])
	return os.sep + os.sep.join( pathar)

def getFileName( filepath):
	pathar = getPathArray( filepath)
	return pathar[ -1]

def joinLists(*args):
	rt = []
	for v in args:
		if isinstance( v, numbers.Number):
			rt.append( v)
		elif isinstance( v, (str,bytes)):
			rt.append( v)
		elif isinstance( v, collections.Sequence):
			for elem in v:
				rt.append( elem)
		elif isinstance(v, dict):
			for elem in v:
				rt.append( elem)
		elif isinstance( v, collections.Iterable):
			for elem in v:
				rt.append( elem)
	return rt

def getContextConfig( argval):
	if argval == 'trace':
		return {'trace':{'log':"dump",'file':"stdout"}}
	elif argval:
		return {'rpc':argval}
	else:
		return None
