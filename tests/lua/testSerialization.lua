--
-- This test is not part of the automated tests, because it does not run with a deterministic result and has to be verified manually
--
require "string"
require "math"
require "utils"

if len(sys.argv) > 1:
    outputdir = sys.argv[1]
else:
    outputdir = "."

ctx = strus.Context()

Example = {}
function Example.new( a, b, xyz )
	return {a=a, b=b, xyz=xyz}
end

ExampleStruct = {}
function ExampleStruct.new( e1, e2 )
	return {e1 = e1, e2 = e2}
end

example = Example.new( 1, "goloman", 12.45)
e1 = Example.new( 567, "hoho", 123.456)
e2 = Example.new( 789, "huhu", 456.789)
estruct1 = ExampleStruct.new( e1, e2)
estruct2 = ExampleStruct.new( example, estruct1)

result = "result:\n" ..
	string.format( "(%u)\n%s\n", 1, ctx.debug_serialize( example)) ..
	string.format( "(%u)\n%s\n", 2, ctx.debug_serialize( estruct1)) ..
	string.format( "(%u)\n%s\n", 3, ctx.debug_serialize( estruct2))

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

