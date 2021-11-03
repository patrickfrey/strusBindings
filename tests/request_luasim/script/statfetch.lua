function GET( self, inputstr, path)
	local statfetch = self:get("self")
	if inputstr or not path then
		http_status( "404")
	end
	local index = tonumber(path)
	local msg = statfetch:get( index)
	if next(msg) then
		return {statisticsblob=msg}
	end
end

