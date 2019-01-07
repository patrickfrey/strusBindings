require "string"
require "utils"

function dumpVectorStorage( strusctx, config, vectors, examplevec)
	local output = {}

	-- Get a client for the vector storage:
	local storage = strusctx:createVectorStorageClient( config)

	-- Dump concept classes of the storage:
	output[ 'types'] = storage:types()
	output[ "nof vec word"] = storage:nofVectors( "word")
	output[ "nof vec nonvec"] = storage:nofVectors( "nonvec")
	for iv,vv in ipairs( vectors) do
		ftypes = storage:featureTypes( string.format( "F%u", iv))
		output[ string.format( "types F%u", iv)] = ftypes
		output[ string.format( "vec F%u", iv)] = storage:featureVector( "word", string.format( "F%u", iv))
		output[ string.format( 'vec F%u example sim', iv)] = storage:vectorSimilarity( vv, examplevec)
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

