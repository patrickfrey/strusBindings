import collections
import numbers
import sys
import os
from inspect import currentframe, getframeinfo   #... [-]

def concatValues(obj):
	if obj is None:
		return ""
	elif isinstance(obj, (str,bytes)):
		return obj
	elif isinstance(obj, collections.Sequence):
		rt = ''
		for v in obj:
			if (rt):
				rt = rt + " " + str(v)
			else:
				rt = str(v)
		return rt
	elif isinstance(obj, dict):
		rt = ''
		for k,v in obj.items():
			if (rt):
				rt = rt + " " + str(v)
			else:
				rt = str(v)
		return rt
	else:
		return obj

def _dumpValue( obj, depth):
	if obj is None:
		return "None"
	elif isinstance(obj, numbers.Number):
		if isinstance(obj, numbers.Integral):
			return "%d" % int(obj)
		elif isinstance(obj, numbers.Real) and obj.is_integer():
			return "%d" % int(obj)
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
			s = s + '[' + str(i) + '] = ' + _dumpValue( v, depth-1)
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
			if isinstance(k, (str,bytes)):
				if k[0] == '_':
					continue
				k = '"' + k + '"'
			s = s + '[' + str(k) + '] = ' + _dumpValue( v, depth-1)
		return s + '} '
	else:
		attrdict = {}
		for k in dir(obj):
			if k[0] != '_' and getattr( obj, k) != None:
				attrdict[ k] = getattr( obj, k)
		return _dumpValue( attrdict, depth)

def dumpValue( o):
	return _dumpValue( o, 20)

def _dumpTree( indent, obj, depth, excludeList):
	if obj is None:
		return "None"
	elif isinstance(obj, numbers.Number):
		if isinstance(obj, numbers.Integral):
			return "%d" % int(obj)
		elif isinstance(obj, numbers.Real) and obj.is_integer():
			return "%d" % int(obj)
		else:
			return "%.5f" % obj
	elif isinstance(obj, str):
		return '"' + obj + '"'
	elif isinstance(obj, bytes):
		return '"' + obj.decode('utf-8') + '"'
	elif isinstance(obj, collections.Sequence):
		if (depth == 0):
			return "{...}"
		s = ''
		i = 0
		for v in obj:
			ke = "\n" + indent + "number " + str(i+1)
			ve = _dumpTree( indent + '  ', v, depth-1, excludeList)
			if ve and ve[0] == "\n":
				s = s + ke + ":" + ve
			else:
				s = s + ke + ": " + ve
			i = i + 1
		return s
	elif isinstance(obj, dict):
		if (depth == 0):
			return "{...}"
		s = ''
		for k in sorted( obj.keys()):
			kstr = str(k)
			if kstr[0] == '_':
				continue
			if kstr in excludeList:
				continue
			ke = "\n" + indent + type(k).__name__ + " " + kstr
			ve = _dumpTree( indent + '  ', obj[ k], depth-1, excludeList)
			if ve and ve[0] == "\n":
				s = s + ke + ":" + ve
			else:
				s = s + ke + ": " + ve
		return s
	else:
		attrdict = {}
		for k in dir(obj):
			if k[0] != '_' and getattr( obj, k) != None:
				attrdict[ k] = getattr( obj, k)
		return _dumpTree( indent, attrdict, depth, excludeList)

def dumpTree( obj):
	return _dumpTree( "", obj, 20, {})
def dumpTreeWithFilter( obj, excludeList):
	return _dumpTree( "", obj, 20, excludeList)

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

def getContextConfig( argval, testname):
	if argval == 'trace':
		tracefile = "trace.%s.txt" % testname
		return {'trace':{'log':"dump", 'file':tracefile}}
	elif argval:
		return {'rpc':argval}
	else:
		return None


