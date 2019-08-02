require "os"

-- Function checking a HTTP status code that exits if the code is not in the OK range
function exitOnBadHttpStatus( httpStatus)
	if (httpStatus < 200 or httpStatus >= 300) then
		print( "Request failed with HTTP status " .. httpStatus )
		os.exit()
	end
end

-- Function calling a server in the emulation context (not a real server) and handling possible errors, expecting the call to succeed
function call_server_checked( method, server, arg)
	result,status,errmsg = call_server( method, server, arg)
	if (status < 200 or status >= 300) then
		if arg then
			print( string.format("Request with method %s on server %s and argument %s failed with HTTP status %d: %s", method, server, arg, status, errmsg))
		else
			print( string.format("Request with method %s on server %s failed with HTTP status %d: %s", method, server, status, errmsg))
		end
		os.exit()
	end
	return result
end

-- Function to make result of query evaluation comparable between different platforms
function det_qeval_result( content)
	return reformat_regex( reformat_float( content, 7), "[\\n][ \\t]*[\"]docno[\"][:][ ]*[0-9]+[,][ \\t]*")
end

-- Function to evaluate the path of the executing script (for locating resources or input files in the same directory)
function script_path()
	local str = debug.getinfo(2, "S").source:sub(2)
	return str:match("(.*/)")
end

-- Function to get all files in lexicographically sorted order in a directory matching a defined extension
function getDirectoryFiles( dir, extension)
	rt = {}
	filenames = {}
	for filename in io.popen("ls " .. dir):lines() do
		table.insert( filenames, filename)
	end
	table.sort( filenames, function (left, right) return left < right end)
	for i,filename in ipairs(filenames) do
		if string.find( filename, string.format("%%%s$", extension)) then
			table.insert( rt, filename)
		end
	end
	return rt
end

-- Function to check if to files (result and expected of a test) are equal (accepting different line endings to make the comparison independent of the platform)
function checkExpected( output, expected, outputfile)
	res,lineno,line_a,line_b = cmp_content( output, expected )
	if not res then
		write_textfile( outputfile, output)
		io.stderr:write( string.format("ERROR result not as expected, difference on line %d, result: '%s', expected '%s'\n", lineno, line_a, line_b))
		os.exit( 1)
	end
end




