///\brief Implements an iterator on a set of postings
class PostingIteratorImpl
{
public:
	/// \brief Destructor
	~PostingIteratorImpl(){}

	///\brief Move the iterator to the internal document number of the next document bigger or equal the document number passed
	///\param[in] docno document number to move the iterator to the matching least upperbound from
	///\return the new internal document number or 0 if no more documents defined
	Index skipDoc( int docno);

	///\brief Get the list of matching positions in the current document
	///\return the list of matching positions
	std::vector<Index>* pos() const;

	///\brief Get the (local storage) document frequency of the feature, if available as stored value, else return an upper bound guess of it (do not calculate it)
	///\return the document frequency guess
	Index df() const;

	///\brief Get the feature frequency of the feature in the current document
	///\return the feature frequency
	unsigned int ff() const;

	///\brief Get the current document number
	///\return the current document number or 0 if not defined
	virtual Index docno() const;

private:
	friend class StorageClientImpl;
	PostingIteratorImpl(
		const ObjectRef& objbuilder_impl_,
		const ObjectRef& trace_impl_,
		const ObjectRef& storage_impl_,
		const ObjectRef& postingitr_impl,
		const ObjectRef& errorhnd_);

private:
	ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_postingitr_impl;
};

