/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Part of the implementation of the context for executing XML/JSON requests on the strus bindings, functions for executing method call requests
/// \file "webRequestContext_schema.cpp"
#include "webRequestContext.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "strus/lib/error.hpp"
#include "schemas_base.hpp"
#include "papuga/allocator.h"
#include "papuga/serialization.h"
#include "papuga/errors.h"
#include "papuga/request.h"
#include "papuga/typedefs.h"
#include "papuga/valueVariant.h"
#include "papuga/encoding.h"
#include "private/internationalization.hpp"
#include <string>

using namespace strus;

static bool initHostObjMethodParam( papuga_ValueVariant& arg, WebRequestHandler::MethodParamType paramtype, const char* path, const WebRequestContent& content, papuga_Allocator* allocator, papuga_ErrorCode& errcode)
{
	papuga_Serialization* ser;
	papuga_StringEncoding enc;
	switch (paramtype)
	{
		case WebRequestHandler::ParamEnd:
			return false;
		case WebRequestHandler::ParamPathString:
			papuga_init_ValueVariant_charp( &arg, path);
			return true;
		case WebRequestHandler::ParamPathArray:
			ser = papuga_Allocator_alloc_Serialization( allocator);
			if (!ser)
			{
				errcode = papuga_NoMemError;
				return false;
			}
			else
			{
				PathBuf pathsplit( path);
				const char* pathelem;
				while (!!(pathelem = pathsplit.getNext()))
				{
					if (!papuga_Serialization_pushValue_charp( ser, pathelem))
					{
						errcode = papuga_NoMemError;
						return false;
					}
				}
			}
			papuga_init_ValueVariant_serialization( &arg, ser);
			return true;
		case WebRequestHandler::ParamDocumentClass:
			ser = papuga_Allocator_alloc_Serialization( allocator);
			if (!ser)
			{
				errcode = papuga_NoMemError;
				return false;
			}
			papuga_Serialization_set_structid( ser, STRUS_BINDINGS_STRUCTID_DocumentClass);
			if (!papuga_Serialization_pushValue_charp( ser, content.doctype())
			||	!papuga_Serialization_pushValue_charp( ser, content.charset()))
			{
				errcode = papuga_NoMemError;
				return false;
			}
			papuga_init_ValueVariant_serialization( &arg, ser);
			return true;
		case WebRequestHandler::ParamContent:
			if (!papuga_getStringEncodingFromName( &enc, content.charset()))
			{
				enc = papuga_Binary;
			}
			papuga_init_ValueVariant_string_enc( &arg, enc, content.str(), content.len());
			return true;
	}
	errcode = papuga_LogicError;
	return false;
}

static bool hostObj_callMethod( void* self, const papuga_RequestMethodDescription* methoddescr, const char* path, const WebRequestContent& content, papuga_Allocator* allocator, papuga_CallResult& retval, papuga_RequestError& errstruct, int& httpStatus)
{
	// Get method function pointer to call:
	papuga_init_RequestError( &errstruct);
	if (methoddescr->id.classid == 0)
	{
		errstruct.errcode = papuga_ValueUndefined;
		httpStatus = 500;
		return false;
	}
	const papuga_ClassDef* cdeflist = strus_getBindingsClassDefs();
	const papuga_ClassDef* cdef = &cdeflist[ methoddescr->id.classid-1];
	if (methoddescr->id.functionid == 0)
	{
		errstruct.errcode = papuga_TypeError;
		httpStatus = 500;
		return false;
	}
	papuga_ClassMethod method = cdef->methodtable[ methoddescr->id.functionid-1];

	// Initialize the arguments:
	enum {MaxNofArgs=32};
	papuga_ValueVariant argv[MaxNofArgs];
	int argc = 0;
	bool path_argument_used = false;

	for (; argc < MaxNofArgs && methoddescr->paramtypes[argc]; ++argc)
	{
		WebRequestHandler::MethodParamType paramtype = (WebRequestHandler::MethodParamType)methoddescr->paramtypes[argc];
		path_argument_used |= (paramtype == WebRequestHandler::ParamPathArray || WebRequestHandler::ParamPathString);
		if (!initHostObjMethodParam( argv[ argc], paramtype, path, content, allocator, errstruct.errcode))
		{
			errstruct.classname = cdef->name;
			errstruct.methodname = cdef->methodnames[ methoddescr->id.functionid-1];
			errstruct.argcnt = argc;
			httpStatus = errorCodeToHttpStatus( papugaErrorToErrorCode( errstruct.errcode));
			return false;
		}
	}
	if (argc == MaxNofArgs)
	{
		errstruct.errcode = papuga_NofArgsError;
		httpStatus = errorCodeToHttpStatus( papugaErrorToErrorCode( errstruct.errcode));
		return false;
	}
	if (!path_argument_used && path[0])
	{
		// ... path defined but not required in method
		errstruct.errcode = papuga_InvalidRequest;
		httpStatus = errorCodeToHttpStatus( papugaErrorToErrorCode( errstruct.errcode));
		return false;
	}
	// Call the method:
	if (!(*method)( self, &retval, argc, argv))
	{
		errstruct.errcode = papuga_HostObjectError;
		errstruct.classname = cdef->name;
		errstruct.methodname = cdef->methodnames[ methoddescr->id.functionid-1];

		char* errstr = papuga_CallResult_lastError( &retval);
		char const* msgitr = errstr;
		int apperr = strus::errorCodeFromMessage( msgitr);
		if (apperr) strus::removeErrorCodesFromMessage( errstr);
		std::size_t errlen = std::strlen( errstr);
		if (errlen >= sizeof(errstruct.errormsg)) errlen = sizeof(errstruct.errormsg)-1;
		std::memcpy( errstruct.errormsg, errstr, errlen);
		errstruct.errormsg[ errlen] = 0;
		httpStatus = errorCodeToHttpStatus( papugaErrorToErrorCode( errstruct.errcode));
		return false;
	}
	httpStatus = methoddescr->httpstatus_success;
	return true;
}

bool WebRequestContext::callHostObjMethodToVariable( void* self, const papuga_RequestMethodDescription* methoddescr, PapugaContextRef& context_, const char* resultname)
{
	papuga_CallResult retval;
	char membuf_err[ 4096];
	papuga_init_CallResult( &retval, &m_allocator, false/*allocator ownerwhip*/, membuf_err, sizeof(membuf_err));
	WebRequestContent content;
	papuga_RequestError errstruct;
	int httpStatus;

	if (!hostObj_callMethod( self, methoddescr, ""/*path*/, content, &m_allocator, retval, errstruct, httpStatus))
	{
		setAnswer( errstruct.errcode, errstruct.errormsg, true/*do copy*/);
		return false;
	}
	// Assign the result to context:
	if (retval.nofvalues == 0)
	{
		setAnswer( ErrorCodeRuntimeError, _TXT( "result expected"));
		return false;
	}
	else if (retval.nofvalues > 1)
	{
		setAnswer( ErrorCodeRuntimeError, _TXT( "only one result expected"));
		return false;
	}
	else if (!papuga_RequestContext_define_variable( context_.get(), resultname, &retval.valuear[0]))
	{
		return false;
	}
	m_answer.setStatus( httpStatus);
	return true;
}

bool WebRequestContext::callHostObjMethodToAnswer( void* self, const papuga_RequestMethodDescription* methoddescr, const char* path, const WebRequestContent& content)
{
	papuga_CallResult retval;
	char membuf_err[ 4096];
	papuga_RequestError errstruct;
	int httpStatus;

	papuga_init_CallResult( &retval, &m_allocator, false/*allocator ownerwhip*/, membuf_err, sizeof(membuf_err));
	if (!hostObj_callMethod( self, methoddescr, path, content, &m_allocator, retval, errstruct, httpStatus))
	{
		setAnswer( errstruct.errcode, errstruct.errormsg, true/*do copy*/);
		return false;
	}
	// Assign the result:
	if (methoddescr->resulttype)
	{
		if (retval.nofvalues == 0)
		{
			m_answer.setMessage( httpStatus, methoddescr->resulttype, "");
			return true;
		}
		else if (retval.nofvalues > 1)
		{
			setAnswer( ErrorCodeRuntimeError, _TXT( "only one result expected"));
			return false;
		}
		else
		{
			size_t msglen;
			papuga_ErrorCode ec = papuga_Ok;
			const char* msgstr = papuga_ValueVariant_tostring( &retval.valuear[0], &m_allocator, &msglen, &ec);
			if (!msgstr)
			{
				setAnswer( papugaErrorToErrorCode( ec));
				return false;
			}
			else
			{
				m_answer.setMessage( methoddescr->httpstatus_success, methoddescr->resulttype, msgstr);
				return true;
			}
		}
	}
	else
	{
		if (retval.nofvalues == 0)
		{
			m_answer.setStatus( httpStatus);
			return true;
		}
		else if (retval.nofvalues > 1)
		{
			setAnswer( ErrorCodeRuntimeError, _TXT( "only one result expected"));
			return false;
		}
		else
		{
			if (!mapValueVariantToAnswer( m_answer, &m_allocator, m_handler->html_head(), m_html_base_href.c_str(), methoddescr->result_rootelem, methoddescr->result_listelem, m_result_encoding, m_result_doctype, retval.valuear[0]))
			{
				return false;
			}
			return true;
		}
	}
}

