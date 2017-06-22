function round( num, numDecimalPlaces)
	local mult = 10^(numDecimalPlaces or 0)
	return math.floor(num * mult + 0.5) / mult
end

function dumpValue(o)
	if type(o) == 'table' then
		local s = '{ '
		local i = 0
		for k,v in pairs(o) do
			if i > 0 then
				s = s .. ', '
			end
			i = i + 1
			if type(k) == 'string' then k = '"'..k..'"' end
			s = s .. '['..k..'] = ' .. dumpValue(v)
		end
		return s .. '} '
	elseif type(o) == 'number' then
		num = tonumber( string.format("%.6f", o))
		if (num == math.floor(num)) then
			return string.format("%d", o)
		else
			return string.format("%.6f", o)
		end
	elseif type(o) == 'string' then
		return '"' .. tostring(o) .. '"'
	else
		return tostring(o)
	end
end

function dumpTree( indent, o)
	if type(o) == 'table' then
		local keyset = {}
		
		for k,v in pairs(o) do
			table.insert( keyset, k)
		end
		function compare(a,b)
			return a < b
		end
		table.sort( keyset, compare)
		local s = ''
		for i,k in ipairs(keyset) do
			local ke = "\n" .. indent .. type(k) .. " " .. k
			local ve = dumpTree( indent .. '  ', o[ k])
			if string.sub( ve,1,1) == "\n" then
				s = s .. ke .. ":" .. ve
			else
				s = s .. ke .. ": " .. ve
			end
		end
		return s
	elseif type(o) == 'number' then
		num = tonumber( string.format("%.6f", o))
		if (num == math.floor(num)) then
			return string.format("%d", o)
		else
			return string.format("%.6f", o)
		end
	elseif type(o) == 'string' then
		return '"' .. tostring(o) .. '"'
	else
		return tostring(o)
	end
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
	return "/" .. table.concat( pathar, "/")
end

function getFileName( filepath)
	local pathar = getPathArray( filepath)
	return pathar[ #pathar]
end

