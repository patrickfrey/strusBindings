/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Function to apply a list of patches according to RFC 6902 to a configuration content
/// \file "patchConfigRfc6902.cpp"
#include "patchConfigRfc6902.hpp"
#include "structDefs.hpp"
#include "treeNode.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/dll_tags.hpp"
#include "strus/reference.hpp"
#include "private/errorUtils.hpp"
#include "private/internationalization.hpp"
#include "papuga/typedefs.h"
#include "papuga/errors.h"
#include "papuga/valueVariant.h"
#include "papuga/valueVariant.hpp"

using namespace strus;
using namespace strus::bindings;

static papuga_ContentType detectContentType( const std::string& content)
{
	char const* ci = content.c_str();
	while (*ci && (unsigned char)*ci <= 32) ++ci;
	switch (*ci)
	{
		case '<': return papuga_ContentType_XML;
		case '{': return papuga_ContentType_JSON;
		default: return papuga_ContentType_Unknown;
	}
}

static void loadSerialization( papuga_Serialization& ser, const std::string& src)
{
	papuga_ContentType contentType = detectContentType( src);
	papuga_ErrorCode errcode;
	if (contentType == papuga_ContentType_JSON)
	{
		if (!papuga_Serialization_append_json( &ser, src.c_str(), src.size(), papuga_UTF8, true/*with root*/, &errcode))
		{
			throw strus::runtime_error(_TXT("error loading JSON as serialized content: %s"), papuga_ErrorCode_tostring(errcode));
		}
	}
	else if (contentType == papuga_ContentType_XML)
	{
		if (!papuga_Serialization_append_xml( &ser, src.c_str(), src.size(), papuga_UTF8, true/*with root*/, true/*ignoreEmptyContent*/, &errcode))
		{
			throw strus::runtime_error(_TXT("error loading XML as serialized content: %s"), papuga_ErrorCode_tostring(errcode));
		}
	}
	else
	{
		throw std::runtime_error(_TXT("cannot process document format other than XML or JSON here"));
	}
}

static std::string parseRoot( papuga_SerializationIter& itr)
{
	if (papuga_SerializationIter_tag( &itr) == papuga_TagName)
	{
		char rootbuf[64];
		const char* root = papuga_ValueVariant_toascii( rootbuf, sizeof(rootbuf), papuga_SerializationIter_value(&itr), 0);
		if (root)
		{
			papuga_SerializationIter_skip( &itr);
			if (papuga_SerializationIter_tag( &itr) != papuga_TagOpen)
			{
				throw strus::runtime_error(_TXT("expected root tag followed by structure"));
			}
			papuga_SerializationIter_skip( &itr);
			return root;
		}
		else
		{
			throw strus::runtime_error(_TXT("expected root tag with a string as name"));
		}
	}
	else
	{
		throw strus::runtime_error(_TXT("expected root tag"));
	}
}

static std::string parseTag( papuga_SerializationIter& itr)
{
	if (papuga_SerializationIter_tag( &itr) == papuga_TagName)
	{
		char nambuf[ 128];
		const char* nam = papuga_ValueVariant_toascii( nambuf, sizeof(nambuf), papuga_SerializationIter_value(&itr), 0);
		if (nam)
		{
			papuga_SerializationIter_skip( &itr);
			return nam;
		}
		else
		{
			return std::string();
		}
	}
	else
	{
		return std::string();
	}
}

static void consumeClose( papuga_SerializationIter& itr)
{
	if (papuga_SerializationIter_tag( &itr) != papuga_TagClose || papuga_SerializationIter_eof( &itr))
	{
		throw strus::runtime_error(_TXT("expected close at end of content"));
	}
	papuga_SerializationIter_skip( &itr);
}

static void expectEof( papuga_SerializationIter& itr)
{
	if (!papuga_SerializationIter_eof( &itr))
	{
		throw strus::runtime_error(_TXT("expected eof at end of content"));
	}
}

enum NodeType
{
	PathIsKey,
	NameTypeMembers,
	NameTypeRoot
};

static NodeType getNodeType( const std::vector<std::string>& path)
{
	if (path.empty()) throw std::runtime_error(_TXT("invalid patch operation: without 'path'"));
	if (path.size() == 1)
	{
		if (path[0] == "metadata")
		{
			return NameTypeRoot;
		}
		else
		{
			return PathIsKey;
		}
	}
	else if (path[path.size()-2] == "metadata")
	{
		return NameTypeMembers;
	}
	else
	{
		return PathIsKey;
	}
}

static int findNode_NameTypeMembers( TreeNode* arrayNode_NameTypeMembers, const std::string& name)
{
	TreeNode* nd = arrayNode_NameTypeMembers;
	if (nd && nd->isArray())
	{
		std::size_t ai = 0, ae = nd->size();
		for (; ai != ae; ++ai)
		{
			TreeNode* chld = nd->get( ai);
			TreeNode* namend = chld->get( "name");
			if (namend && namend->getValue() == name) return ai;
		}
		return -1;
	}
	else
	{
		throw std::runtime_error(_TXT("expected array node for name/type members node"));
	}
}

static void applyPatch( TreeNode* config, const PatchDef& patchdef)
{
	switch (patchdef.op)
	{
		case PatchDef::OpAdd:
			switch (getNodeType( patchdef.path))
			{
				case NameTypeRoot:
				{
					throw strus::runtime_error(_TXT("incomplete '%s' for add of name/type value"), "path");
				}
				case NameTypeMembers:
				{
					std::vector<std::string> parent( patchdef.path.begin(), patchdef.path.end()-1);
					TreeNode* nd = config->getOrCreatePath( parent, TreeNode::Array);
					strus::Reference<TreeNode> st( TreeNode::createDict());
					st->set( "name", patchdef.path.back());
					st->set( "type", patchdef.value);

					int chldidx = findNode_NameTypeMembers( nd, patchdef.path.back());
					if (chldidx < 0)
					{
						nd->append( st.release());
					}
					else
					{
						throw strus::runtime_error(_TXT("try to add already existing element"));
					}
					break;
				}
				case PathIsKey:
				{
					TreeNode* nd = config->getOrCreatePath( patchdef.path, TreeNode::Atomic);
					nd->setValue( patchdef.value);
				}
			}
			break;
		case PatchDef::OpRemove:
			switch (getNodeType( patchdef.path))
			{
				case NameTypeRoot:
				{
					config->removePath( patchdef.path);
				}
				case NameTypeMembers:
				{
					std::vector<std::string> parent( patchdef.path.begin(), patchdef.path.end()-1);
					TreeNode* nd = config->getPath( parent);
					if (!nd) throw std::runtime_error(_TXT("parent of node of remove not found"));
					int chldidx = findNode_NameTypeMembers( nd, patchdef.path.back());
					if (chldidx < 0) throw std::runtime_error(_TXT("node to remove not found"));
					nd->remove( chldidx);
					break;
				}
				case PathIsKey:
				{
					config->removePath( patchdef.path);
				}
			}
			break;
		case PatchDef::OpReplace:
			switch (getNodeType( patchdef.from))
			{
				case NameTypeRoot:
				{
					throw strus::runtime_error(_TXT("incomplete '%s' for replace of name/type value"), "from");
				}
				case NameTypeMembers:
				{
					TreeNode* nd = config->getPath(
							std::vector<std::string>( patchdef.from.begin(), patchdef.from.end()-1));
					if (!nd) throw std::runtime_error(_TXT("parent of node to replace not found"));
					int chldidx = findNode_NameTypeMembers( nd, patchdef.from.back());
					if (chldidx < 0) throw std::runtime_error(_TXT("node to replace not found"));
					TreeNode* chld = nd->get( chldidx);
					if (!patchdef.value.empty())
					{
						chld->set( "type", patchdef.value);
					}
					if (patchdef.path == patchdef.from)
					{
						//... do nothing, value replaced
					}
					if (getNodeType( patchdef.path) == NameTypeMembers)
					{
						strus::Reference<TreeNode> movnd( nd->release( chldidx)); 

						nd = config->getOrCreatePath(
								std::vector<std::string>( patchdef.path.begin(), patchdef.path.end()-1),
								TreeNode::Array);
						chldidx = findNode_NameTypeMembers( nd, patchdef.path.back());
						if (chldidx < 0)
						{
							nd->append( movnd.release());
						}
						else
						{
							nd->set( chldidx, movnd.release());
						}
					}
					else if (!patchdef.path.empty())
					{
						throw strus::runtime_error(_TXT("replace not possible with this '%s'"), "path");
					}
					else
					{
						//... do nothing, value replaced
					}
					break;
				}
				case PathIsKey:
				{
					std::vector<std::string> parent( patchdef.from.begin(), patchdef.from.end()-1);
					TreeNode* nd = config->getPath( parent);
					if (!nd) throw std::runtime_error(_TXT("node to replace not found"));
					strus::Reference<TreeNode> movnd( nd->release( patchdef.from.back())); 
					if (!patchdef.value.empty())
					{
						movnd->setValue( patchdef.value);
					}
					if (patchdef.path.empty()) throw strus::runtime_error(_TXT("missing '%s' for replace"), "path");
					!!!! FALSCH
					nd = config->getOrCreatePath( patchdef.path, TreeNode::Atomic);
					nd->setValue( patchdef.value);
				}
			}
			break;
		case PatchDef::OpCopy:
			if (getNodeType( patchdef.path) == NameTypeMembers) break; //... assignment ignored for configuration
			/*no break here! fallback to error*/
		case PatchDef::OpMove:
		case PatchDef::OpTest:
			throw strus::runtime_error_ec( ErrorCodeNotImplemented, _TXT("patch op %s not implemented"), PatchDef::opName( patchdef.op));
			break;
	}
}

DLL_PUBLIC std::string strus::patchConfigRfc6902( const std::string& configsrc, const std::string& patchsrc, ErrorBufferInterface* errorhnd)
{
	try
	{
		struct AllocatorRAII
		{
			papuga_Allocator* allocator;

			AllocatorRAII( papuga_Allocator* allocator_=0) :allocator(allocator_){}
			~AllocatorRAII()
			{
				if (allocator) papuga_destroy_Allocator( allocator);
			}
		};
		papuga_ErrorCode errcode = papuga_Ok;
		papuga_Allocator allocator;
		int allocator_mem[ 2048];
		papuga_init_Allocator( allocator, &allocator_mem, sizeof(allocator_mem));
		AllocatorRAII allocatorRAII( &allocator);

		papuga_Serialization configser;
		papuga_init_Serialization( &configser, &allocator);
		loadSerialization( configser, configsrc);

		strus::Reference<TreeNode> configTreeRoot( TreeNode::createFromSerialization( configser));
		if (!configTreeRoot->isDict() || configTreeRoot->size() != 1) throw strus::runtime_error(_TXT("structure with single root element expected in configuration to patch"));
		TreeNode* configTree = configTreeRoot->firstChild();
		std::string configRootName = configTreeRoot->firstKey();

		papuga_Serialization patchser;
		papuga_init_Serialization( &patchser, &allocator);
		loadSerialization( patchser, patchsrc);
		papuga_SerializationIter patchitr;
		papuga_init_SerializationIter( &patchitr, &patchser);
		std::string patchroot = parseRoot( patchitr);
		if (patchroot != "patch") throw strus::runtime_error(_TXT("RFC 6902 patch file not recognized, expected root element 'patch'"));

		std::vector<PatchDef> patchlist = PatchDef::parseList( patchitr);
		consumeClose( patchitr);
		expectEof( patchitr);

		std::vector<PatchDef>::const_iterator pi = patchlist.begin(), pe = patchlist.end();
		for (; pi != pe; ++pi)
		{
			try
			{
				applyPatch( configTree, *pi);
			}
			catch (const std::runtime_error& err)
			{
				pathstr = pi->pathAsString();
				fromstr = pi->fromAsString();
				throw strus::runtime_error(
						_TXT("%s, applying patch op=%s, path=%s, value=%s, from=%s"),
						err.what(), patchDef::opName(pi->op), pathstr.c_str(),
						pi->value.c_str(), fromstr.c_str());
			}
		}
		papuga_Serialization resultser;
		papuga_init_Serialization( &resultser, &allocator);
		configTree->serialize( &resultser);
		papuga_ValueVariant resultval;
		papuga_init_ValueVariant_serialization( &resultval, &resultser);

		papuga_ContentType contentType = detectContentType( configsrc);
		char* resultstr;
		std::size_t resultlen;
		switch (contentType)
		{
			case papuga_ContentType_JSON:
				resultstr = (char*) papuga_ValueVariant_tojson(
						&resultval, &allocator, NULL/*structdefs*/, papuga_UTF8,
						true/*beautified*/, configRootName.c_str(), NULL/*elemname*/,
						&resultlen, &errcode);
				break;
			case papuga_ContentType_XML:
				resultstr = (char*) papuga_ValueVariant_toxml(
						&resultval, &allocator, NULL/*structdefs*/, papuga_UTF8,
						true/*beautified*/, configRootName.c_str(), NULL/*elemname*/,
						&resultlen, &errcode);
				break;
			case papuga_ContentType_Unknown: /*no break here!*/
			default:throw std::runtime_error(_TXT("cannot process document format other than XML or JSON here"));
		}
		if (!resultstr)
		{
			throw strus::runtime_error( _TXT("mapping to %s failed: %s"), WebRequestContent::typeName(contentType), papuga_ErrorCode_tostring( errcode));
		}
		std::string rt( resultstr, resultlen);
	}
	CATCH_ERROR_MAP_RETURN( _TXT("failed to apply patch on configuration source"), *errorhnd, std::string());
}



