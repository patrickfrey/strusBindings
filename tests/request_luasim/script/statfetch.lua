function GET( self, inputstr, path)
	local self = self:get("self")
	local iterator = self.iterator
	if iterator then
		local index = 1
		local msg = iterator:getNext()
		while (msg) do
			self:set( "msg" .. index, msg)
			index = index + 1
			msg = iterator:getNext()
		end
		self:set("timestamp", self.timestamp)
		self:set("self", nil)
	end
	if path then
		local msg = self:get( "msg" .. path)
		local timestamp = self:get( "timestamp")
		if msg then
			return {statisticsblob=msg, timestamp=timestamp}
		end
	else
		http_status( "404")
	end
end

