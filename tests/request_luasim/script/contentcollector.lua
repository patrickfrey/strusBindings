function GET( self, inputstr, path)
	local collector = self:get("self")
	if path == "nofdocs" then
		return {nofdocs=collector:nofDocuments()}
	elseif not path or path == "statistics" then
		return {statistics=collector:statistics()}
	else
		http_status( "404")
	end
end

function PUT( self, inputstr, path)
	local collector = self:get("self")
	collector:putContent( path, inputstr)
end

