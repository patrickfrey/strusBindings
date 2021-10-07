function GET( context, inputstr, path)
	return nil
end

function PUT( self, inputstr, path)
	input = schema( "contentstats", inputstr, true)
	context = self:get("context")
	contentstats = context:createContentStatistics()
	for _,a in ipairs(input.attribute) do
		contentstats:addVisibleAttribute( a)
	end
	for _,s in ipairs(input.select) do
		contentstats:addSelectorExpression( s)
	end
	for _,e in ipairs(input.element) do
		contentstats:addLibraryElement( e.type, e.regex, e.priority, e.minlen, e.maxlen, e.tokenizer, e.normalizer)
	end
	self:set( "contentstats", contentstats)
	return nil
end

function POST( self, inputstr, path)
	return nil
end

