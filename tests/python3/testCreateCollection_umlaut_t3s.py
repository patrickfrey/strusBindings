import sys
import strus
from utils import *
from config_umlaut_t3s import *
from createCollection import createCollection
from dumpCollection import dumpCollection

testname = "CreateCollection_umlaut"
if len(sys.argv) > 1:
        datadir = sys.argv[1]
else:
        datadir = "../data/umlaut_t3s/"
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

createCollection( ctx, storage, metadata_umlaut_t3s(), createDocumentAnalyzer_umlaut_t3s( ctx), False, datadir, docfiles, aclmap, withrpc)
result = "collection dump:" + dumpTree( dumpCollection( ctx, storage)) + "\n"
expected = """collection dump:
str attribute:
  number 1: "docclass"
  number 2: "docid"
  number 3: "title"
str config:
  str acl: "true"
  str cache: "524288K"
  str path: "storage"
str docids:
  number 1: "A.xml"
  number 2: "B.xml"
  number 3: "C.xml"
str docs:
  str A.xml:
    str ACL: "a"
    str docclass: "mimetype='application/xml', encoding='utf-8', schema=''"
    str docid: "A.xml"
    str doclen: 14
    str endtitle: ""
    str orig: "Märchen und Ähnliches Es versammeln sich die Löwen, wenn die Vögel über alles fliegen"
    str title: "Maerchen und Aehnliches"
    str title_end: 5
    str title_start: 0
    str word: "march und aehnlich es versammeln sich die low wenn vogel uber all flieg"
  str B.xml:
    str ACL: "b"
    str docclass: "mimetype='application/xml', encoding='utf-8', schema=''"
    str docid: "B.xml"
    str doclen: 14
    str endtitle: ""
    str orig: "Hüben wie drüben Über alles könnte man sich aufregen, aber über die Löwen besonders."
    str title: "Hueben wie drueben"
    str title_end: 5
    str title_start: 0
    str word: "sich die low uber all hub wie drub ueber konnt man aufreg aber besond"
  str C.xml:
    str ACL: "c"
    str docclass: "mimetype='application/xml', encoding='utf-8', schema=''"
    str docid: "C.xml"
    str doclen: 7
    str endtitle: ""
    str orig: "Holla Hü Das ist ein weiterer Text"
    str title: "Holla Hue"
    str title_end: 4
    str title_start: 0
    str word: "holla hu das ist ein weit text"
str metadata:
  str doclen: "UInt16"
  str title_end: "UInt8"
  str title_start: "UInt8"
str nofdocs: 3
str stat:
  str dfchange:
    number 1:
      str increment: 3
      str type: "endtitle"
      str value: ""
    number 2:
      str increment: 1
      str type: "word"
      str value: "aber"
    number 3:
      str increment: 1
      str type: "word"
      str value: "aehnlich"
    number 4:
      str increment: 2
      str type: "word"
      str value: "all"
    number 5:
      str increment: 1
      str type: "word"
      str value: "aufreg"
    number 6:
      str increment: 1
      str type: "word"
      str value: "besond"
    number 7:
      str increment: 1
      str type: "word"
      str value: "das"
    number 8:
      str increment: 2
      str type: "word"
      str value: "die"
    number 9:
      str increment: 1
      str type: "word"
      str value: "drub"
    number 10:
      str increment: 1
      str type: "word"
      str value: "ein"
    number 11:
      str increment: 1
      str type: "word"
      str value: "es"
    number 12:
      str increment: 1
      str type: "word"
      str value: "flieg"
    number 13:
      str increment: 1
      str type: "word"
      str value: "holla"
    number 14:
      str increment: 1
      str type: "word"
      str value: "hu"
    number 15:
      str increment: 1
      str type: "word"
      str value: "hub"
    number 16:
      str increment: 1
      str type: "word"
      str value: "ist"
    number 17:
      str increment: 1
      str type: "word"
      str value: "konnt"
    number 18:
      str increment: 2
      str type: "word"
      str value: "low"
    number 19:
      str increment: 1
      str type: "word"
      str value: "man"
    number 20:
      str increment: 1
      str type: "word"
      str value: "march"
    number 21:
      str increment: 2
      str type: "word"
      str value: "sich"
    number 22:
      str increment: 1
      str type: "word"
      str value: "text"
    number 23:
      str increment: 2
      str type: "word"
      str value: "uber"
    number 24:
      str increment: 1
      str type: "word"
      str value: "ueber"
    number 25:
      str increment: 1
      str type: "word"
      str value: "und"
    number 26:
      str increment: 1
      str type: "word"
      str value: "versammeln"
    number 27:
      str increment: 1
      str type: "word"
      str value: "vogel"
    number 28:
      str increment: 1
      str type: "word"
      str value: "weit"
    number 29:
      str increment: 1
      str type: "word"
      str value: "wenn"
    number 30:
      str increment: 1
      str type: "word"
      str value: "wie"
  str nofdocs: 3
str statblobs:
  number 1:
    str blob: "AAAAAwALZW5kdGl0bGUgAAMAC3dvcmQgYWJlcgABBgllaG5saWNoAAEGBGxsAAIGB3VmcmVnAAEFCGJlc29uZAABBQVkYXMAAQYEaWUAAgYFcnViAAEFBWVpbgABBgNzAAEFB2ZsaWVnAAEFB2hvbGxhAAEGA3UAAQcDYgABBQVpc3QAAQUHa29ubnQAAQUFbG93AAIFBW1hbgABBwVyY2gAAQUGc2ljaAACBQZ0ZXh0AAEFBnViZXIAAgYGZWJlcgABBgRuZAABBQx2ZXJzYW1tZWxuAAEGBm9nZWwAAQUGd2VpdAABBwRubgABBgRpZQAB"
str terms:
  str A.xml:endtitle (f):
  str A.xml:endtitle (s):
    number 1:
      str firstpos: 4
      str tf: 1
      str value: ""
  str A.xml:orig (f):
    number 1:
      str pos: 1
      str value: "Märchen"
    number 2:
      str pos: 2
      str value: "und"
    number 3:
      str pos: 3
      str value: "Ähnliches"
    number 4:
      str pos: 5
      str value: "Es"
    number 5:
      str pos: 6
      str value: "versammeln"
    number 6:
      str pos: 7
      str value: "sich"
    number 7:
      str pos: 8
      str value: "die"
    number 8:
      str pos: 9
      str value: "Löwen,"
    number 9:
      str pos: 10
      str value: "wenn"
    number 10:
      str pos: 11
      str value: "die"
    number 11:
      str pos: 12
      str value: "Vögel"
    number 12:
      str pos: 13
      str value: "über"
    number 13:
      str pos: 14
      str value: "alles"
    number 14:
      str pos: 15
      str value: "fliegen"
  str A.xml:orig (s):
  str A.xml:word (f):
  str A.xml:word (s):
    number 1:
      str firstpos: 1
      str tf: 1
      str value: "march"
    number 2:
      str firstpos: 2
      str tf: 1
      str value: "und"
    number 3:
      str firstpos: 3
      str tf: 1
      str value: "aehnlich"
    number 4:
      str firstpos: 5
      str tf: 1
      str value: "es"
    number 5:
      str firstpos: 6
      str tf: 1
      str value: "versammeln"
    number 6:
      str firstpos: 7
      str tf: 1
      str value: "sich"
    number 7:
      str firstpos: 8
      str tf: 2
      str value: "die"
    number 8:
      str firstpos: 9
      str tf: 1
      str value: "low"
    number 9:
      str firstpos: 10
      str tf: 1
      str value: "wenn"
    number 10:
      str firstpos: 12
      str tf: 1
      str value: "vogel"
    number 11:
      str firstpos: 13
      str tf: 1
      str value: "uber"
    number 12:
      str firstpos: 14
      str tf: 1
      str value: "all"
    number 13:
      str firstpos: 15
      str tf: 1
      str value: "flieg"
  str B.xml:endtitle (f):
  str B.xml:endtitle (s):
    number 1:
      str firstpos: 4
      str tf: 1
      str value: ""
  str B.xml:orig (f):
    number 1:
      str pos: 1
      str value: "Hüben"
    number 2:
      str pos: 2
      str value: "wie"
    number 3:
      str pos: 3
      str value: "drüben"
    number 4:
      str pos: 5
      str value: "Über"
    number 5:
      str pos: 6
      str value: "alles"
    number 6:
      str pos: 7
      str value: "könnte"
    number 7:
      str pos: 8
      str value: "man"
    number 8:
      str pos: 9
      str value: "sich"
    number 9:
      str pos: 10
      str value: "aufregen,"
    number 10:
      str pos: 11
      str value: "aber"
    number 11:
      str pos: 12
      str value: "über"
    number 12:
      str pos: 13
      str value: "die"
    number 13:
      str pos: 14
      str value: "Löwen"
    number 14:
      str pos: 15
      str value: "besonders."
  str B.xml:orig (s):
  str B.xml:word (f):
  str B.xml:word (s):
    number 1:
      str firstpos: 9
      str tf: 1
      str value: "sich"
    number 2:
      str firstpos: 13
      str tf: 1
      str value: "die"
    number 3:
      str firstpos: 14
      str tf: 1
      str value: "low"
    number 4:
      str firstpos: 12
      str tf: 1
      str value: "uber"
    number 5:
      str firstpos: 6
      str tf: 1
      str value: "all"
    number 6:
      str firstpos: 1
      str tf: 1
      str value: "hub"
    number 7:
      str firstpos: 2
      str tf: 1
      str value: "wie"
    number 8:
      str firstpos: 3
      str tf: 1
      str value: "drub"
    number 9:
      str firstpos: 5
      str tf: 1
      str value: "ueber"
    number 10:
      str firstpos: 7
      str tf: 1
      str value: "konnt"
    number 11:
      str firstpos: 8
      str tf: 1
      str value: "man"
    number 12:
      str firstpos: 10
      str tf: 1
      str value: "aufreg"
    number 13:
      str firstpos: 11
      str tf: 1
      str value: "aber"
    number 14:
      str firstpos: 15
      str tf: 1
      str value: "besond"
  str C.xml:endtitle (f):
  str C.xml:endtitle (s):
    number 1:
      str firstpos: 3
      str tf: 1
      str value: ""
  str C.xml:orig (f):
    number 1:
      str pos: 1
      str value: "Holla"
    number 2:
      str pos: 2
      str value: "Hü"
    number 3:
      str pos: 4
      str value: "Das"
    number 4:
      str pos: 5
      str value: "ist"
    number 5:
      str pos: 6
      str value: "ein"
    number 6:
      str pos: 7
      str value: "weiterer"
    number 7:
      str pos: 8
      str value: "Text"
  str C.xml:orig (s):
  str C.xml:word (f):
  str C.xml:word (s):
    number 1:
      str firstpos: 1
      str tf: 1
      str value: "holla"
    number 2:
      str firstpos: 2
      str tf: 1
      str value: "hu"
    number 3:
      str firstpos: 4
      str tf: 1
      str value: "das"
    number 4:
      str firstpos: 5
      str tf: 1
      str value: "ist"
    number 5:
      str firstpos: 6
      str tf: 1
      str value: "ein"
    number 6:
      str firstpos: 7
      str tf: 1
      str value: "weit"
    number 7:
      str firstpos: 8
      str tf: 1
      str value: "text"
str types:
  number 1: "endtitle"
  number 2: "orig"
  number 3: "word"
"""

verifyTestOutput( outputdir, result, expected)


