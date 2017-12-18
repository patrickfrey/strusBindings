function round( num, numDecimalPlaces)
	local mult = 10^(numDecimalPlaces or 0)
	return math.floor(num * mult + 0.5) / mult
end

function concatValues(o)
	if type(o) == 'table' then
		rt = nil
		for _,value in ipairs(o) do
			if (rt) then
				rt = rt .. " " .. tostring(value)
			else
				rt = tostring(value)
			end
		end
		return rt
	else
		return o
	end
end

function dumpValue_( o, depth)
	if type(o) == 'table' then
		if (depth == 0) then
			return "{...}"
		end
		local s = '{ '
		local i = 0
		for k,v in pairs(o) do
			if i > 0 then
				s = s .. ', '
			end
			i = i + 1
			if type(k) == 'string' then k = '"'..k..'"' end
			s = s .. '['..k..'] = ' .. dumpValue_( v, depth-1)
		end
		return s .. '} '
	elseif type(o) == 'number' then
		num = tonumber( string.format("%.5f", o))
		if (num == math.floor(num)) then
			return string.format("%d", o)
		else
			return string.format("%.5f", o + 0.)
		end
	elseif type(o) == 'string' then
		return '"' .. tostring(o) .. '"'
	else
		return tostring(o)
	end
end
function dumpValue( o)
	return dumpValue_( o, 10)
end

function dumpTree_( indent, o, depth, excludeSet)
	if type(o) == 'table' then
		if (depth == 0) then
			return "{...}"
		end
		local keyset = {}

		for k,v in pairs(o) do
			if (excludeSet[k] == nil) then
				table.insert( keyset, k)
			end
		end
		function compare(a,b)
			return a < b
		end
		table.sort( keyset, compare)
		local s = ''
		for i,k in ipairs(keyset) do
			local ke = "\n" .. indent .. type(k) .. " " .. k
			local ve = dumpTree_( indent .. '  ', o[ k], depth-1, excludeSet)
			if string.sub( ve,1,1) == "\n" then
				s = s .. ke .. ":" .. ve
			else
				s = s .. ke .. ": " .. ve
			end
		end
		return s
	elseif type(o) == 'number' then
		num = tonumber( string.format("%.5f", o))
		if (num == math.floor(num)) then
			return string.format("%d", o)
		else
			return string.format("%.5f", o)
		end
	elseif type(o) == 'string' then
		return '"' .. tostring(o) .. '"'
	else
		return tostring(o)
	end
end
function dumpTree( o)
	return dumpTree_( "", o, 10, {})
end
function dumpTreeWithFilter( o, excludeList)
	excludeSet = {}
	for _,exl in ipairs(excludeList) do
		excludeSet[ exl] = true
	end
	return dumpTree_( "", o, 10, excludeSet)
end
function readFile( path)
	local file = io.open( path, "rb") -- r read mode and b binary mode
	if not file then
		error( string.format( "could not read file '%s'", path))
	end
	local content = file:read "*a" -- *a or *all reads the whole file
	file:close()
	return content
end

function writeFile( path, content)
	local file = io.open( path, "w")
	file:write( content)
	file:close()
end

function verifyTestOutput( outputdir, result, expected)
	if (result ~= expected) then
		writeFile( outputdir .. "/RES", result)
		writeFile( outputdir .. "/EXP", expected)

		print "Failed"
		os.exit( 1)
	else
		print "OK"
	end
end

function getPathArray( filepath)
	local pathar = {}
	for pp in string.gmatch( filepath, "[^/\\]+") do
		table.insert( pathar, pp)
	end
	return pathar
end

function getFileParentDirectory( filepath)
	local pathar = getPathArray( filepath)
	table.remove( pathar)
	return "/" .. table.concat( pathar, "/")
end

function getFileName( filepath)
	local pathar = getPathArray( filepath)
	return pathar[ #pathar]
end

function joinLists(...)
	rt = {}
	for _,v in ipairs({...}) do
		if (type(v) == "function") then
			for elem in v do
				table.insert( rt, elem)
			end
		elseif (type(v) == "table") then
			for _,elem in ipairs(v) do
				table.insert( rt, elem)
			end
		else
			table.insert( rt, v)
		end
	end
	return rt
end

function getContextConfig( argval)
	if argval == 'trace' then
		return {trace={log="dump",file="stdout"}}
	elseif argval then
		return {rpc=argval}
	else
		return nil
	end
end

