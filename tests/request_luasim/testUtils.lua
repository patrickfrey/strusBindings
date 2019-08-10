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
			error( string.format("Request with method %s on server %s and argument %s failed with HTTP status %d: %s", method, server, arg, status, errmsg))
		else
			error( string.format("Request with method %s on server %s failed with HTTP status %d: %s", method, server, status, errmsg))
		end
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
		write_file( outputfile, output)
		io.stderr:write( string.format("ERROR result not as expected, difference on line %d, result: '%s', expected '%s'\n", lineno, line_a, line_b))
		os.exit( 1)
	else
		remove_file( outputfile)
		io.stderr:write( "OK\n");
	end
end

function isArray( val)
	if type(val) ~= 'table' then
		return false
		end
	-- objects always return empty size
	if #val > 0 then
		return true
	end
	-- only object can have empty length with elements inside
	for k, v in pairs(val) do
		return false
	end
	-- if no elements it can be array and not at same time
	return true
end

function concatArrays( t1, t2)
	res = t1
	for i=1,#t2 do
		res[#res+1] = t2[i]
	end
	return res
end

function mergeValues( t1, t2)
	if not t1 then
		return t2
	elseif not t2 then
		return t1
	elseif isArray( t1) and isArray( t2) then
		return concatArrays( t1, t2)
	elseif (type(t1) == "table" and type(t2) == "table") then
		res = {}
		for key,value in pairs(t1) do
			if t2[key] then
				res[ key] = mergeValues( value, t2[key])
			else
				res[ key] = value
			end
		end
		for key,value in pairs(t2) do
			if not res[key] then
				if t1[key] then
					res[ key] = mergeValues( value, t1[key])
				else
					res[ key] = value
				end
			end
		end
		return res
	else
		return {t1,t2}
	end
end

function hashString( str, md, start)
	local rt = 1013
	for i = 1, #str do
		local c = string.byte( str, i)
		rt = rt * 103 + c
		if md then
			rt = math.fmod( rt, md)
		end
	end
	if md then
		return math.fmod( rt, md) + start
	else
		return rt + start
	end
end


