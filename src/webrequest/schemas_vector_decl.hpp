/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Partial schema definition to include for vector storages
 * @file schemas_vector.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_VECTOR_DECL_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_VECTOR_DECL_HPP_INCLUDED
#include "schemas_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaVectorStoragePart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineVectorStorage( const char* rootexpr)
	{
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{"path", "()", DatabasePath, papuga_TypeString, "strus/vstorage"},
			{"path", "", "path", DatabasePath, '?'},
			{"memtypes", "()", VectorMemType, papuga_TypeString, "V;A"},
			{"lextypes", "()", VectorLexemTypePriorityMap, papuga_TypeString, "N,V,E : A,T,C"},
			{"spacesb", "()", VectorLexemSpaceSubst, papuga_TypeString, "_;-"},
			{"linksb", "()", VectorLexemLinkSubst, papuga_TypeString, "_;-"},
			{"coversim", "()", VectorLexemCoverSimilarity, papuga_TypeDouble, "_;-"},
			{"recall", "()", VectorLexemLshRecall, papuga_TypeDouble, "_;-"},
			{"vecdim", "()", VectorDim, papuga_TypeInt, "256;300;500"},
			{"bits", "()", VectorBits, papuga_TypeInt, "64"},
			{"variations", "()", VectorVariations, papuga_TypeInt, "32"},
			{"cache", "()", DatabaseLruCacheSize, papuga_TypeString, "200M"},
			{"autocompact", "()", DatabaseEnableAutoCompact, papuga_TypeBool, "true"},
			{"compression", "()", DatabaseEnableCompression, papuga_TypeBool, "true"},
			{"max_open_files", "()", DatabaseMaxNofOpenFiles, papuga_TypeInt, "128"},
			{"write_buffer_size", "()", DatabaseWriteBufferSize, papuga_TypeString, "4M"},
			{"block_size", "()", DatabaseBlockSize, papuga_TypeString, "4K"},
			{"", StorageConfig, {
					{"path", "path"},
					{"memtypes", VectorMemType, '*'},
					{"lextypes", VectorLexemTypePriorityMap, '?'},
					{"spacesb", VectorLexemSpaceSubst, '?'},
					{"linksb", VectorLexemLinkSubst, '?'},
					{"coversim", VectorLexemCoverSimilarity, '?'},
					{"recall", VectorLexemLshRecall, '?'},
					{"vecdim", VectorDim, '?'},
					{"bits", VectorBits, '?'},
					{"variations", VectorVariations, '?'},
					{"cache", DatabaseLruCacheSize, '?'},
					{"autocompact", DatabaseEnableAutoCompact, '?'},
					{"compression", DatabaseEnableCompression, '?'},
					{"max_open_files", DatabaseMaxNofOpenFiles, '?'},
					{"write_buffer_size", DatabaseWriteBufferSize, '?'},
					{"block_size", DatabaseBlockSize, '?'},
				}
			},
		});
	}
};

}}//namespace
#endif

