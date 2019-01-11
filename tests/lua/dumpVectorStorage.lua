require "string"
require "utils"

function vecToString( vec, delim)
	rt = ""
	for iv,vv in ipairs( vec) do
		if rt:len() > 0 then
			rt = rt .. delim
		end
		rt = rt .. string.format( "%.5f", vv)
	end
	return rt
end

function dumpVectorStorage( strusctx, config, vectors, examplevec)
	local output = {}

	-- Get a client for the vector storage:
	local storage = strusctx:createVectorStorageClient( config)

	ranklist = {}

	-- Dump concept classes of the storage:
	output[ 'types'] = storage:types()
	output[ "nof vec word"] = storage:nofVectors( "word")
	output[ "nof vec nonvec"] = storage:nofVectors( "nonvec")
	for iv,vv in ipairs( vectors) do
		featstr = string.format( "F%u", iv)
		featsim = tonumber( string.format( "%.3f", storage:vectorSimilarity( vv, examplevec)))
		featvec = storage:featureVector( "word", featstr)
		ftypes = storage:featureTypes( featstr)
		output[ string.format( "types F%u", iv)] = ftypes
		output[ string.format( "vec F%u", iv)] = "{" .. vecToString( featvec, ", ") .. "}"
		output[ string.format( 'vec F%u example sim', iv)] = featsim
		table.insert( ranklist, {featstr,featsim,featvec} )
	end
	table.sort( ranklist, function( a, b ) return a[2] > b[2] or (a[2] == b[2] and a[1] > b[1]) end )
	for iv,vv in ipairs( ranklist) do
		if iv > 5 then break end
		output[ string.format( "rank %3d", iv)] = string.format( "%.3f %s {%s}", vv[2], vv[1], vecToString( vv[3],", "))
	end
	-- Configuration of the storage:
	local output_config = {}
	for key,value in pairs( storage:config()) do
		if key == 'path' then
			output_config[ key] = getFileName( value)
		else
			output_config[ key] = value
		end
	end

	output[ "config"] = output_config

	storage:close()
	return output
end

