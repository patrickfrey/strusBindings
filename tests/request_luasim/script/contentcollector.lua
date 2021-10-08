function GET( self, inputstr, path)
	collector = self:get("self")
	if path == "nofdocs" then
		return {nofdocs=collector:nofDocuments()}
	elseif path == "statistics" or path == "" then
		return {statistics=collector:statistics()}
	else
		http_error( "404")
	end
end

function PUT( self, inputstr, path)
	collector = self:get("self")
	collector:putContent( path, inputstr)
end

