require "os"

function exitOnBadHttpStatus( httpStatus)
	if (httpStatus < 200 or httpStatus >= 300) then
		print( "Request failed with HTTP status " .. httpStatus )
		os.exit()
	end
end

function call_server_checked( method, server, arg)
	result,status = call_server( method, server, arg)
	if (status < 200 or status >= 300) then
		if arg then
			print( string.format("Request with method %s on server %s and argument %s failed with HTTP status %d", method, server, arg, httpStatus))
		else
			print( string.format("Request with method %s on server %s failed with HTTP status %d", method, server, httpStatus))
		end
		os.exit()
	end
	return result
end

function script_path()
	local str = debug.getinfo(2, "S").source:sub(2)
	return str:match("(.*/)")
end

function getDirectoryFiles( dir, extension)
	rt = {}
	for filename in io.popen("ls " .. dir):lines() do
		if string.find( filename, string.format("%%%s$", extension)) then
			table.insert( rt, filename)
		end
	end
	return rt
end

function checkExpected( output, expected)
	res,lineno,line_a,line_b = cmp_content( output, expected )
	if not res then
		write_textfile( "RES", output)
		io.stderr:write( string.format("ERROR result not as expected, difference on line %d, result: '%s', expected '%s'\n", lineno, line_a, line_b))
		os.exit( 1)
	end
end




