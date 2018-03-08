<?php
include_once "utils.php";

try
{

$outputdir = '.';
if (isset($argv[1])) {
	$outputdir = $argv[1];
}

$ctx = new StrusContext();

class Example
{
	var $a;
	var $b;
	var $xyz;

	function __construct( $a, $b, $xyz)
	{
		$this->a = $a;
		$this->b = $b;
		$this->xyz = $xyz;
	}
}

class ExampleStruct
{
	var $e1;
	var $e2;

	function __construct( $e1, $e2)
	{
		$this->e1 = $e1;
		$this->e2 = $e2;
	}
}

$example = new Example( 1, "goloman", 12.45);
$e1 = new Example( 567, "hoho", 123.456);
$e2 = new Example( 789, "huhu", 456.789);
$estruct1 = new ExampleStruct( $e1, $e2);
$estruct2 = new ExampleStruct( $example, $estruct1);

$result = "result:\n";
$result .= sprintf( "(%u)\n%s", 1, $ctx->debug_serialize( $example, TRUE)) . "\n";
$result .= sprintf( "(%u)\n%s", 2, $ctx->debug_serialize( $estruct1, TRUE)) . "\n";
$result .= sprintf( "(%u)\n%s", 3, $ctx->debug_serialize( $estruct2, TRUE));
}
catch (Error $e)
{
	echo "ERROR in script: $e\n";
	exit( -1);
}
catch (Exception $e)
{
	echo "ERROR in script: $e\n";
	exit( -1);
}

$expected = <<<END_expected
result:
(1)
a:1, b:"goloman", xyz:12.45
(2)
e1:{a:567, b:"hoho", xyz:123.456}, e2:{a:789, b:"huhu", xyz:456.789}
(3)
e1:{a:1, b:"goloman", xyz:12.45}, e2:{e1:{a:567, b:"hoho", xyz:123.456}, e2:{a:789, b:"huhu", xyz:456.789}}
END_expected;

verifyTestOutput( $outputdir, $result, $expected);
?>
