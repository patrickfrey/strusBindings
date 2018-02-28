import sys
import strus
from utils import *
from config_mdprim import *
from createCollection import createCollection
from dumpCollection import dumpCollection

testname = "CreateCollection_mdprim"
if len(sys.argv) > 1:
	datadir = sys.argv[1]
else:
	datadir = "../data/mdprim/"
if len(sys.argv) > 2:
	outputdir = sys.argv[2]
else:
	outputdir = "."
if len(sys.argv) > 3:
	ctxconfig = getContextConfig( sys.argv[3], testname)
else:
	ctxconfig = None

storage = outputdir + "/storage"
docfiles = ["doc10.xml"]
withrpc = False
if ctxconfig and 'rpc' in ctxconfig:
	withrpc = True

ctx = strus.Context( ctxconfig)

aclmap = {"1":"A","2":"A","3":"A","4":"A","5":"A","6":"B","7":"B","8":"B","9":"B","10":"B"}

createCollection( ctx, storage, metadata_mdprim(), createDocumentAnalyzer_mdprim( ctx), True, datadir, docfiles, aclmap, withrpc)
result = "collection dump:" + dumpTree( dumpCollection( ctx, storage)) + "\n"
expected = """collection dump:
str config:
  str acl: "true"
  str cache: "524288K"
  str metadata:
    str cross: "UInt8"
    str docidx: "UInt32"
    str doclen: "UInt16"
    str factors: "UInt8"
    str hi: "UInt16"
    str lo: "UInt16"
  str path: "storage"
str docids:
  number 1: "1"
  number 2: "10"
  number 3: "2"
  number 4: "3"
  number 5: "4"
  number 6: "5"
  number 7: "6"
  number 8: "7"
  number 9: "8"
  number 10: "9"
str docs:
  str 1:
    str ACL: "A"
    str cross: 1
    str docid: "1"
    str docidx: 1
    str doclen: 0
    str factors: 0
    str hi: 0
    str lo: 0
    str word: ""
  str 10:
    str ACL: "B"
    str cross: 1
    str docid: "10"
    str docidx: 10
    str doclen: 2
    str factors: 1
    str hi: 5
    str lo: 2
    str word: "2 5"
  str 2:
    str ACL: "A"
    str cross: 2
    str docid: "2"
    str docidx: 2
    str doclen: 1
    str factors: 0
    str hi: 2
    str lo: 2
    str word: "2"
  str 3:
    str ACL: "A"
    str cross: 3
    str docid: "3"
    str docidx: 3
    str doclen: 1
    str factors: 0
    str hi: 3
    str lo: 3
    str word: "3"
  str 4:
    str ACL: "A"
    str cross: 4
    str docid: "4"
    str docidx: 4
    str doclen: 2
    str factors: 1
    str hi: 2
    str lo: 2
    str word: "2 2"
  str 5:
    str ACL: "A"
    str cross: 5
    str docid: "5"
    str docidx: 5
    str doclen: 1
    str factors: 0
    str hi: 5
    str lo: 5
    str word: "5"
  str 6:
    str ACL: "B"
    str cross: 6
    str docid: "6"
    str docidx: 6
    str doclen: 2
    str factors: 1
    str hi: 3
    str lo: 2
    str word: "2 3"
  str 7:
    str ACL: "B"
    str cross: 7
    str docid: "7"
    str docidx: 7
    str doclen: 1
    str factors: 0
    str hi: 7
    str lo: 7
    str word: "7"
  str 8:
    str ACL: "B"
    str cross: 8
    str docid: "8"
    str docidx: 8
    str doclen: 3
    str factors: 2
    str hi: 2
    str lo: 2
    str word: "2 2 2"
  str 9:
    str ACL: "B"
    str cross: 9
    str docid: "9"
    str docidx: 9
    str doclen: 2
    str factors: 1
    str hi: 3
    str lo: 3
    str word: "3 3"
str nofdocs: 10
str stat:
  str dfchange:
    number 1:
      str increment: 5
      str type: "word"
      str value: "2"
    number 2:
      str increment: 3
      str type: "word"
      str value: "3"
    number 3:
      str increment: 2
      str type: "word"
      str value: "5"
    number 4:
      str increment: 1
      str type: "word"
      str value: "7"
  str nofdocs: 10
str terms:
  str 10:word (f):
    number 1:
      str pos: 1
      str value: "2"
    number 2:
      str pos: 2
      str value: "5"
  str 10:word (s):
    number 1:
      str firstpos: 1
      str tf: 1
      str value: "2"
    number 2:
      str firstpos: 2
      str tf: 1
      str value: "5"
  str 1:word (f): 
  str 1:word (s): 
  str 2:word (f):
    number 1:
      str pos: 1
      str value: "2"
  str 2:word (s):
    number 1:
      str firstpos: 1
      str tf: 1
      str value: "2"
  str 3:word (f):
    number 1:
      str pos: 1
      str value: "3"
  str 3:word (s):
    number 1:
      str firstpos: 1
      str tf: 1
      str value: "3"
  str 4:word (f):
    number 1:
      str pos: 1
      str value: "2"
    number 2:
      str pos: 2
      str value: "2"
  str 4:word (s):
    number 1:
      str firstpos: 1
      str tf: 2
      str value: "2"
  str 5:word (f):
    number 1:
      str pos: 1
      str value: "5"
  str 5:word (s):
    number 1:
      str firstpos: 1
      str tf: 1
      str value: "5"
  str 6:word (f):
    number 1:
      str pos: 1
      str value: "2"
    number 2:
      str pos: 2
      str value: "3"
  str 6:word (s):
    number 1:
      str firstpos: 1
      str tf: 1
      str value: "2"
    number 2:
      str firstpos: 2
      str tf: 1
      str value: "3"
  str 7:word (f):
    number 1:
      str pos: 1
      str value: "7"
  str 7:word (s):
    number 1:
      str firstpos: 1
      str tf: 1
      str value: "7"
  str 8:word (f):
    number 1:
      str pos: 1
      str value: "2"
    number 2:
      str pos: 2
      str value: "2"
    number 3:
      str pos: 3
      str value: "2"
  str 8:word (s):
    number 1:
      str firstpos: 1
      str tf: 3
      str value: "2"
  str 9:word (f):
    number 1:
      str pos: 1
      str value: "3"
    number 2:
      str pos: 2
      str value: "3"
  str 9:word (s):
    number 1:
      str firstpos: 1
      str tf: 2
      str value: "3"
str types:
  number 1: "word"
"""

verifyTestOutput( outputdir, result, expected)


