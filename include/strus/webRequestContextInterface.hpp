/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestContextInterface.hpp"
#ifndef _STRUS_WEB_REQUEST_CONTEXT_INTERFACE_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_CONTEXT_INTERFACE_HPP_INCLUDED
#include "strus/webRequestAnswer.hpp"
#include "strus/webRequestContent.hpp"
#include "strus/webRequestDelegateRequest.hpp"
#include <cstddef>
#include <vector>

namespace strus
{

/// \brief Interface for executing an XML/JSON request on the strus bindings
class WebRequestContextInterface
{
public:
	/// \brief Destructor
	virtual ~WebRequestContextInterface(){}

	/// \brief Run the request
	/// \param[in] content content of the request
	/// \note The methods 'execute','getDelegateRequests','pushDelegateRequestAnswer','complete' are executed in a context of a state machine with getDelegateRequests and pushDelegateRequestAnswer executed in a loop until no following delegate request defined. 'execute' is called at the start and 'complete' at the end.
	/// \return bool true if succeeded, false else
	virtual bool execute( const WebRequestContent& content)=0;

	/// \brief Get a list of delegate requests
	/// \note Called after runnig the request or pushing the results of the last delegate requests
	/// \return delegate requests to perform for the completion of the request or an empty list in case of an error or no further delegate requests defined
	/// \note A possible error is indicated in the answer retrieved with 'complete();'
	virtual std::vector<WebRequestDelegateRequest> getDelegateRequests()=0;

	/// \brief Return the result of a delegate request to process the answer in the current context created with executeRequest()
	/// \param[in] schema to use for processing the answer content if available
	/// \param[in] answer answer with content of the result put
	/// \return bool true if succeeded, false else
	virtual bool putDelegateRequestAnswer(
			const char* schema,
			const WebRequestAnswer& answer)=0;

	/// \brief Complete the request executed with 'execute(const WebRequestContent&)'
	/// \return true on success, false on failure
	virtual bool complete()=0;

	/// \brief Get the result answer of the request
	/// \return result status and the data of the answer of the request
	virtual WebRequestAnswer getAnswer() const=0;
};

}//namespace
#endif


