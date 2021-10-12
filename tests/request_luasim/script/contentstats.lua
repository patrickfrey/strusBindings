function GET( self, inputstr, path)
	local contentstats = self:get("contentstats")
	return {contentstats={contentstats:introspection( path)}}
end

function PUT( self, inputstr, path)
	if path then
		http_error( "404")
	else
		local input = schema( "contentstats", inputstr, true).contentstats
		local context = self:get("context")
		local contentstats = context:createContentStatistics()
		for _,a in ipairs(input.attribute or {}) do
			contentstats:addVisibleAttribute( a)
		end
		for _,s in ipairs(input.select or {}) do
			contentstats:addSelectorExpression( s)
		end
		for _,e in ipairs(input.element or {}) do
			contentstats:addLibraryElement( e.type, e.regex, e.priority, e.minlen, e.maxlen, e.tokenizer, e.normalizer)
		end
		self:set( "contentstats", contentstats)
	end
end

function POST( self, inputstr, path)
	if path then
		http_error( "404")
	else
		local contentstats = self:get("contentstats")
		local tid = transaction( "contentcollector", contentstats:createCollector())
		return {transaction={link=tid}}
	end
end

