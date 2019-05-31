import sys
import strus
from utils import *
from config_t3s import *
from createCollection import createCollection
from dumpCollection import dumpCollection

testname = "CreateCollection_t3s"
if len(sys.argv) > 1:
	datadir = sys.argv[1]
else:
	datadir = "../data/t3s/"
if len(sys.argv) > 2:
	outputdir = sys.argv[2]
else:
	outputdir = "."
if len(sys.argv) > 3:
	ctxconfig = getContextConfig( sys.argv[3], testname)
else:
	ctxconfig = None

storage = outputdir + "/storage"
docfiles = ["A.xml","B.xml","C.xml"]
withrpc = False
if ctxconfig and 'rpc' in ctxconfig:
	withrpc = True

ctx = strus.Context( ctxconfig)
aclmap = {"A.xml":"a", "B.xml":"b", "C.xml":"c"}

createCollection( ctx, storage, metadata_t3s(), createDocumentAnalyzer_t3s( ctx), False, datadir, docfiles, aclmap, withrpc)
result = "collection dump:" + dumpTree( dumpCollection( ctx, storage)) + "\n"
expected = """collection dump:
str config:
  str acl: "true"
  str cache: "524288K"
  str metadata:
    str doclen: "UInt16"
    str title_end: "UInt8"
    str title_start: "UInt8"
  str path: "storage"
str docids:
  number 1: "A.xml"
  number 2: "B.xml"
  number 3: "C.xml"
str docs:
  str A.xml:
    str ACL: "a"
    str docclass: "mimetype='application/xml', encoding='utf-8', scheme=''"
    str docid: "A.xml"
    str doclen: 20
    str endtitle: ""
    str orig: "One day in Tokyo Tokyo is a city that is completely different than what you would expect as European citizen."
    str title: "One day in Tokyo"
    str title_end: 6
    str title_start: 0
    str word: "one day in tokyo is a citi that complet differ than what you would expect as european citizen"
  str B.xml:
    str ACL: "b"
    str docclass: "mimetype='application/xml', encoding='utf-8', scheme=''"
    str docid: "B.xml"
    str doclen: 16
    str endtitle: ""
    str orig: "A visit in New York New York is a city with dimensions you can't imagine."
    str title: "A visit in New York"
    str title_end: 7
    str title_start: 0
    str word: "in is a citi you visit new york with dimens can t imagin"
  str C.xml:
    str ACL: "c"
    str docclass: "mimetype='application/xml', encoding='utf-8', scheme=''"
    str docid: "C.xml"
    str doclen: 16
    str endtitle: ""
    str orig: "A journey through Germany When I first visited germany it was still splitted into two parts."
    str title: "A journey through Germany"
    str title_end: 6
    str title_start: 0
    str word: "a visit journey through germani when i first it was still split into two part"
str nofdocs: 3
str stat:
  str dfchange:
    number 1:
      str increment: 3
      str type: "endtitle"
      str value: ""
    number 2:
      str increment: 3
      str type: "word"
      str value: "a"
    number 3:
      str increment: 1
      str type: "word"
      str value: "as"
    number 4:
      str increment: 1
      str type: "word"
      str value: "can"
    number 5:
      str increment: 2
      str type: "word"
      str value: "citi"
    number 6:
      str increment: 1
      str type: "word"
      str value: "citizen"
    number 7:
      str increment: 1
      str type: "word"
      str value: "complet"
    number 8:
      str increment: 1
      str type: "word"
      str value: "day"
    number 9:
      str increment: 1
      str type: "word"
      str value: "differ"
    number 10:
      str increment: 1
      str type: "word"
      str value: "dimens"
    number 11:
      str increment: 1
      str type: "word"
      str value: "european"
    number 12:
      str increment: 1
      str type: "word"
      str value: "expect"
    number 13:
      str increment: 1
      str type: "word"
      str value: "first"
    number 14:
      str increment: 1
      str type: "word"
      str value: "germani"
    number 15:
      str increment: 1
      str type: "word"
      str value: "i"
    number 16:
      str increment: 1
      str type: "word"
      str value: "imagin"
    number 17:
      str increment: 2
      str type: "word"
      str value: "in"
    number 18:
      str increment: 1
      str type: "word"
      str value: "into"
    number 19:
      str increment: 2
      str type: "word"
      str value: "is"
    number 20:
      str increment: 1
      str type: "word"
      str value: "it"
    number 21:
      str increment: 1
      str type: "word"
      str value: "journey"
    number 22:
      str increment: 1
      str type: "word"
      str value: "new"
    number 23:
      str increment: 1
      str type: "word"
      str value: "one"
    number 24:
      str increment: 1
      str type: "word"
      str value: "part"
    number 25:
      str increment: 1
      str type: "word"
      str value: "split"
    number 26:
      str increment: 1
      str type: "word"
      str value: "still"
    number 27:
      str increment: 1
      str type: "word"
      str value: "t"
    number 28:
      str increment: 1
      str type: "word"
      str value: "than"
    number 29:
      str increment: 1
      str type: "word"
      str value: "that"
    number 30:
      str increment: 1
      str type: "word"
      str value: "through"
    number 31:
      str increment: 1
      str type: "word"
      str value: "tokyo"
    number 32:
      str increment: 1
      str type: "word"
      str value: "two"
    number 33:
      str increment: 2
      str type: "word"
      str value: "visit"
    number 34:
      str increment: 1
      str type: "word"
      str value: "was"
    number 35:
      str increment: 1
      str type: "word"
      str value: "what"
    number 36:
      str increment: 1
      str type: "word"
      str value: "when"
    number 37:
      str increment: 1
      str type: "word"
      str value: "with"
    number 38:
      str increment: 1
      str type: "word"
      str value: "would"
    number 39:
      str increment: 1
      str type: "word"
      str value: "york"
    number 40:
      str increment: 2
      str type: "word"
      str value: "you"
  str nofdocs: 3
str statblobs:
  number 1:
    str blob: "AAAAAwALZW5kdGl0bGUAAAMACHdvcmQAYQADBgNzAAEFBWNhbgABBgVpdGkAAgkFemVuAAEGCG9tcGxldAABBQVkYXkAAQYHaWZmZXIAAQcGbWVucwABBQpldXJvcGVhbgABBgd4cGVjdAABBQdmaXJzdAABBQlnZXJtYW5pAAEFA2kAAQYHbWFnaW4AAQYDbgACBwR0bwABBgNzAAIGA3QAAQUJam91cm5leQABBQVuZXcAAQUFb25lAAEFBnBhcnQAAQUHc3BsaXQAAQYGdGlsbAABBQN0AAEGBWhhbgABCAN0AAEHB3JvdWdoAAEGBm9reW8AAQYEd28AAQUHdmlzaXQAAgUFd2FzAAEGBWhhdAABBwRlbgABBgVpdGgAAQYGb3VsZAABBQZ5b3JrAAEHA3UAAg=="
str terms:
  str A.xml:endtitle (f): 
  str A.xml:endtitle (s):
    number 1:
      str firstpos: 5
      str tf: 1
      str value: ""
  str A.xml:orig (f):
    number 1:
      str pos: 1
      str value: "One"
    number 2:
      str pos: 2
      str value: "day"
    number 3:
      str pos: 3
      str value: "in"
    number 4:
      str pos: 4
      str value: "Tokyo"
    number 5:
      str pos: 6
      str value: "Tokyo"
    number 6:
      str pos: 7
      str value: "is"
    number 7:
      str pos: 8
      str value: "a"
    number 8:
      str pos: 9
      str value: "city"
    number 9:
      str pos: 10
      str value: "that"
    number 10:
      str pos: 11
      str value: "is"
    number 11:
      str pos: 12
      str value: "completely"
    number 12:
      str pos: 13
      str value: "different"
    number 13:
      str pos: 14
      str value: "than"
    number 14:
      str pos: 15
      str value: "what"
    number 15:
      str pos: 16
      str value: "you"
    number 16:
      str pos: 17
      str value: "would"
    number 17:
      str pos: 18
      str value: "expect"
    number 18:
      str pos: 19
      str value: "as"
    number 19:
      str pos: 20
      str value: "European"
    number 20:
      str pos: 21
      str value: "citizen."
  str A.xml:orig (s): 
  str A.xml:word (f): 
  str A.xml:word (s):
    number 1:
      str firstpos: 1
      str tf: 1
      str value: "one"
    number 2:
      str firstpos: 2
      str tf: 1
      str value: "day"
    number 3:
      str firstpos: 3
      str tf: 1
      str value: "in"
    number 4:
      str firstpos: 4
      str tf: 2
      str value: "tokyo"
    number 5:
      str firstpos: 7
      str tf: 2
      str value: "is"
    number 6:
      str firstpos: 8
      str tf: 1
      str value: "a"
    number 7:
      str firstpos: 9
      str tf: 1
      str value: "citi"
    number 8:
      str firstpos: 10
      str tf: 1
      str value: "that"
    number 9:
      str firstpos: 12
      str tf: 1
      str value: "complet"
    number 10:
      str firstpos: 13
      str tf: 1
      str value: "differ"
    number 11:
      str firstpos: 14
      str tf: 1
      str value: "than"
    number 12:
      str firstpos: 15
      str tf: 1
      str value: "what"
    number 13:
      str firstpos: 16
      str tf: 1
      str value: "you"
    number 14:
      str firstpos: 17
      str tf: 1
      str value: "would"
    number 15:
      str firstpos: 18
      str tf: 1
      str value: "expect"
    number 16:
      str firstpos: 19
      str tf: 1
      str value: "as"
    number 17:
      str firstpos: 20
      str tf: 1
      str value: "european"
    number 18:
      str firstpos: 21
      str tf: 1
      str value: "citizen"
  str B.xml:endtitle (f): 
  str B.xml:endtitle (s):
    number 1:
      str firstpos: 6
      str tf: 1
      str value: ""
  str B.xml:orig (f):
    number 1:
      str pos: 1
      str value: "A"
    number 2:
      str pos: 2
      str value: "visit"
    number 3:
      str pos: 3
      str value: "in"
    number 4:
      str pos: 4
      str value: "New"
    number 5:
      str pos: 5
      str value: "York"
    number 6:
      str pos: 7
      str value: "New"
    number 7:
      str pos: 8
      str value: "York"
    number 8:
      str pos: 9
      str value: "is"
    number 9:
      str pos: 10
      str value: "a"
    number 10:
      str pos: 11
      str value: "city"
    number 11:
      str pos: 12
      str value: "with"
    number 12:
      str pos: 13
      str value: "dimensions"
    number 13:
      str pos: 14
      str value: "you"
    number 14:
      str pos: 15
      str value: "can't"
    number 15:
      str pos: 17
      str value: "imagine."
  str B.xml:orig (s): 
  str B.xml:word (f): 
  str B.xml:word (s):
    number 1:
      str firstpos: 3
      str tf: 1
      str value: "in"
    number 2:
      str firstpos: 9
      str tf: 1
      str value: "is"
    number 3:
      str firstpos: 1
      str tf: 2
      str value: "a"
    number 4:
      str firstpos: 11
      str tf: 1
      str value: "citi"
    number 5:
      str firstpos: 14
      str tf: 1
      str value: "you"
    number 6:
      str firstpos: 2
      str tf: 1
      str value: "visit"
    number 7:
      str firstpos: 4
      str tf: 2
      str value: "new"
    number 8:
      str firstpos: 5
      str tf: 2
      str value: "york"
    number 9:
      str firstpos: 12
      str tf: 1
      str value: "with"
    number 10:
      str firstpos: 13
      str tf: 1
      str value: "dimens"
    number 11:
      str firstpos: 15
      str tf: 1
      str value: "can"
    number 12:
      str firstpos: 16
      str tf: 1
      str value: "t"
    number 13:
      str firstpos: 17
      str tf: 1
      str value: "imagin"
  str C.xml:endtitle (f): 
  str C.xml:endtitle (s):
    number 1:
      str firstpos: 5
      str tf: 1
      str value: ""
  str C.xml:orig (f):
    number 1:
      str pos: 1
      str value: "A"
    number 2:
      str pos: 2
      str value: "journey"
    number 3:
      str pos: 3
      str value: "through"
    number 4:
      str pos: 4
      str value: "Germany"
    number 5:
      str pos: 6
      str value: "When"
    number 6:
      str pos: 7
      str value: "I"
    number 7:
      str pos: 8
      str value: "first"
    number 8:
      str pos: 9
      str value: "visited"
    number 9:
      str pos: 10
      str value: "germany"
    number 10:
      str pos: 11
      str value: "it"
    number 11:
      str pos: 12
      str value: "was"
    number 12:
      str pos: 13
      str value: "still"
    number 13:
      str pos: 14
      str value: "splitted"
    number 14:
      str pos: 15
      str value: "into"
    number 15:
      str pos: 16
      str value: "two"
    number 16:
      str pos: 17
      str value: "parts."
  str C.xml:orig (s): 
  str C.xml:word (f): 
  str C.xml:word (s):
    number 1:
      str firstpos: 1
      str tf: 1
      str value: "a"
    number 2:
      str firstpos: 9
      str tf: 1
      str value: "visit"
    number 3:
      str firstpos: 2
      str tf: 1
      str value: "journey"
    number 4:
      str firstpos: 3
      str tf: 1
      str value: "through"
    number 5:
      str firstpos: 4
      str tf: 2
      str value: "germani"
    number 6:
      str firstpos: 6
      str tf: 1
      str value: "when"
    number 7:
      str firstpos: 7
      str tf: 1
      str value: "i"
    number 8:
      str firstpos: 8
      str tf: 1
      str value: "first"
    number 9:
      str firstpos: 11
      str tf: 1
      str value: "it"
    number 10:
      str firstpos: 12
      str tf: 1
      str value: "was"
    number 11:
      str firstpos: 13
      str tf: 1
      str value: "still"
    number 12:
      str firstpos: 14
      str tf: 1
      str value: "split"
    number 13:
      str firstpos: 15
      str tf: 1
      str value: "into"
    number 14:
      str firstpos: 16
      str tf: 1
      str value: "two"
    number 15:
      str firstpos: 17
      str tf: 1
      str value: "part"
str types:
  number 1: "endtitle"
  number 2: "orig"
  number 3: "word"
"""

verifyTestOutput( outputdir, result, expected)


