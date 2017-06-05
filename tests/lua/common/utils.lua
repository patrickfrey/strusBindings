
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
			local v = o[ k]
			s = s .. "\n" .. indent .. type(k) .. " " .. k .. ": " .. dumpTree( indent .. '  ', v)
		end
		return s
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

