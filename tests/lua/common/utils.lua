
function dumpValue(o)
	if type(o) == 'table' then
		local s = '{ '
		local i = 0
		for k,v in pairs(o) do
			if i > 0 then
				s = s .. ', '
			end
			i = i + 1
			if type(k) ~= 'number' then k = '"'..k..'"' end
			s = s .. '['..k..'] = ' .. dumpValue(v)
		end
		return s .. '} '
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

