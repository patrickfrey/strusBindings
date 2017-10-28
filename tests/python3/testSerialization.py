#
# This test is not part of the automated tests, because it does not run with a deterministic result and has to be verified manually
#
import strus
import os
import sys
import utils
from collections import namedtuple, OrderedDict

if len(sys.argv) > 1:
    outputdir = sys.argv[1]
else:
    outputdir = "."

ctx = strus.Context()

class Example:
    a = 123
    b = "holoholo"
    xyx = 123.456

    def __init__( self, a, b, xyz):
        self.a = a
        self.b = b
        self.xyz = xyz

class ExampleStruct:
    e1 = None
    e2 = None

    def __init__( self, e1, e2):
        self.e1 = e1
        self.e2 = e2

ExampleTuple = namedtuple("ExampleStruct", "field1 field2 field3")


example = Example( 1, "goloman", 12.45)
e1 = Example( 567, "hoho", 123.456)
e2 = Example( 789, "huhu", 456.789)
estruct1 = ExampleStruct( e1, e2)
estruct2 = ExampleStruct( example, estruct1)
estruct3 = OrderedDict((('foo', "bar"), ('spamm', "eggs"), ('ham', None)))
estruct4 = ExampleTuple(  99, 1.23, "blabla" )

result = "result:\n"
result += "(%u)\n%s\n" % (1,ctx.debug_serialize( example))
result += "(%u)\n%s\n" % (2,ctx.debug_serialize( estruct1))
result += "(%u)\n%s\n" % (3,ctx.debug_serialize( estruct2))
result += "(%u)\n%s\n" % (4,ctx.debug_serialize( estruct3))
result += "(%u)\n%s\n" % (5,ctx.debug_serialize( estruct4))

expected = """result:
(1)
name b
value 'goloman'
name a
value 1
name xyz
value 12.45

(2)
name e2
open
  name b
  value 'huhu'
  name a
  value 789
  name xyz
  value 456.789
close
name e1
open
  name b
  value 'hoho'
  name a
  value 567
  name xyz
  value 123.456
close

(3)
name e2
open
  name e2
  open
    name b
    value 'huhu'
    name a
    value 789
    name xyz
    value 456.789
  close
  name e1
  open
    name b
    value 'hoho'
    name a
    value 567
    name xyz
    value 123.456
  close
close
name e1
open
  name b
  value 'goloman'
  name a
  value 1
  name xyz
  value 12.45
close

(4)
name foo
value 'bar'
name ham
value NULL
name spamm
value 'eggs'

(5)
value 99
value 1.23
value 'blabla'

"""

utils.verifyTestOutput( outputdir, result, expected)

