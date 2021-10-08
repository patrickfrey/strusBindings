function GET( self, inputstr, path)
	contentstats = self:get("contentstats")
	return {contentstats={contentstats:introspection( path)}}
end

function PUT( self, inputstr, path)
	input = schema( "contentstats", inputstr, true)
	context = self:get("context")
	contentstats = context:createContentStatistics()
	if input.attribute then
		for _,a in ipairs(input.attribute) do
			contentstats:addVisibleAttribute( a)
		end
	end
	if input.select then
		for _,s in ipairs(input.select) do
			contentstats:addSelectorExpression( s)
		end
	end
	if input.element then
		for _,e in ipairs(input.element) do
			contentstats:addLibraryElement( e.type, e.regex, e.priority, e.minlen, e.maxlen, e.tokenizer, e.normalizer)
		end
	end
	self:set( "contentstats", contentstats)
end

function POST( self, inputstr, path)
	if (path == "transaction") then
		contentstats = self:get("contentstats")
		local tid = transaction( "contentcollector", contentstats:createCollector())
		return {transaction={link=tid}}
	else
		http_error( "404")
	end
end

