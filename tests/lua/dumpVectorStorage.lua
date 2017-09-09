require "string"
require "utils"

function dumpVectorStorage( strusctx, config)
	local output = {}
	-- Get a client for the vector storage:
	local storage = strusctx:createVectorStorageClient( config)

	-- Dump concept classes of the storage:
	local classes = storage:conceptClassNames()
	output[ 'classes'] = classes
	for _,cl in ipairs( classes) do
		local csize = storage:nofConcepts( cl)
		for ci=1,csize do
			local cfeats = storage:conceptFeatures( cl, ci)
			if next(cfeats) ~= nil then
				output[ string.format("class '%s' concept %u", cl, ci)] = cfeats
			end
		end
	end
	-- Dump features of the storage:
	local nofvecs = storage:nofFeatures()
	for fi = 0,nofvecs-1 do
		fname = storage:featureName( fi)
		if fi ~= storage:featureIndex( fname) then
			error( string.format( "feature index does not match %u ~= %u (%s)", fi, storage:featureIndex( fname), fname))
		end
		for _,cl in ipairs( classes) do
			concepts = storage:featureConcepts( cl, fi)
			if next(concepts) ~= nil then
				output[ string.format("class '%s' feature %u", cl, fi)] = concepts
			end
		end
		local fvector = storage:featureVector( fi)
		output[ string.format("vector '%s'", fname)] = fvector
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

