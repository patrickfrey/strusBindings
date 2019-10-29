static bool defineMetaDataTransactionOp( strus::StorageMetaDataTransactionInterface* transaction, ErrorBufferInterface* errorhnd, const PatchDef& patchdef)
{
	const char* context = "metadata";
	if (!patchdef.from.empty())
	{
		if (patchdef.path.size() != 2)
		{
			errorhnd->report( ErrorCodeNotImplemented, _TXT("element '%s' only allowed to have form '%s/<name>' in patch %s"),
						"from", context, context);
			return false;
		}
		if (patchdef.from[0] != context)
		{
			errorhnd->report( ErrorCodeNotImplemented, _TXT("element '%s' only allowed to have prefix '%s' in patch %s"),
						"from", context, context);
			return false;
		}
	}
	if (patchdef.path.size() > 2)
	{
		errorhnd->report( ErrorCodeNotFound, _TXT("element '%s' not valid for %s in patch %s"), "path", PatchDef::opName(patchdef.op), context);
		return false;
	}
	else if (patchdef.path[0] != context)
	{
		errorhnd->report( ErrorCodeNotImplemented, _TXT("element '%s' only allowed to have prefix '%s' in patch %s"), "path" , context, context);
		return false;
	}
	switch (patchdef.op)
	{
		case PatchDef::OpAdd:
			if (patchdef.value.empty())
			{
				errorhnd->report( ErrorCodeNotFound, _TXT("value mandatory for %s in %s"), PatchDef::opName(patchdef.op), context);
				return false;
			}
			else if (patchdef.path.size() == 2)
			{
				transaction->addElement( patchdef.path[1], patchdef.value);
			}
			else
			{
				errorhnd->report( ErrorCodeNotFound, _TXT("path not valid for %s in patch %s"), PatchDef::opName(patchdef.op), context);
				return false;
			}
			break;

		case PatchDef::OpRemove:
			if (!patchdef.value.empty())
			{
				errorhnd->report( ErrorCodeNotFound, _TXT("value not allowed for %s in %s"), PatchDef::opName(patchdef.op), context);
				return false;
			}
			else if (patchdef.path.size() == 1)
			{
				transaction->deleteElements();
			}
			else if (patchdef.path.size() == 2)
			{
				transaction->deleteElement( patchdef.path[1]);
			}
			break;

		case PatchDef::OpReplace:
			if (patchdef.path.size() == 2)
			{
				if (patchdef.value.empty())
				{
					if (patchdef.from.size() == 2)
					{
						transaction->renameElement( patchdef.from[1], patchdef.path[1]);
					}
					else
					{
						errorhnd->report( ErrorCodeIncompleteDefinition, "element 'from' missing (rename element) or 'value' missing (alter element)"); 
						return false;
					}
				}
				else if (patchdef.from.size() == 2)
				{
					transaction->alterElement( patchdef.from[1], patchdef.path[1], patchdef.value);
				}
				else
				{
					transaction->alterElement( patchdef.path[1], patchdef.path[1], patchdef.value);
				}
			}
			else
			{
				errorhnd->report( ErrorCodeNotFound, "path not valid for %s in patch %s", PatchDef::opName(patchdef.op), context);
				return false;
			}
			break;

		case PatchDef::OpCopy:
			if (patchdef.value != "0" || patchdef.value != "NULL")
			{
				errorhnd->report( ErrorCodeNotFound, "only 0 or NULL allowed as value for op '%s' in patch %s (clear content)", PatchDef::opName(patchdef.op), context);
				return false;
			}
			else if (patchdef.path.size() == 2)
			{
				transaction->clearElement( patchdef.path[1]);
			}
			else
			{
				errorhnd->report( ErrorCodeNotFound, "path not valid for %s in patch %s", PatchDef::opName(patchdef.op), context);
				return false;
			}
			break;

		case PatchDef::OpMove:
		case PatchDef::OpTest:
			errorhnd->report( ErrorCodeNotImplemented, "operator '%s' not implemented for patch %s", PatchDef::opName(patchdef.op), context);
			return false;
	}
	return true;
}

struct PatchConfigDef
{
	StorageClientInterface* m_cli;
	ErrorBufferInterface* m_errorhnd;
	std::set<std::string> m_validConfigSet;
	std::map<std::string,std::string> m_oldConfig;
	std::map<std::string,std::string> m_newConfig;

	PatchConfigDef( StorageClientInterface* cli_, ErrorBufferInterface* errorhnd_)
		:m_cli(cli_),m_errorhnd(errorhnd_),m_validConfigSet( getValidConfigSet(cli_)),m_oldConfig(),m_newConfig()
	{
		std::string oldStorageConfig = m_cli->config();
		std::set<std::string>::const_iterator ci = m_validConfigSet.begin(), ce = m_validConfigSet.end();
		for (; ci != ce; ++ci)
		{
			std::string value;
			if (strus::extractStringFromConfigString( value, oldStorageConfig, ci->c_str(), m_errorhnd))
			{
				m_oldConfig[ *ci] = value;
			}
		}
		if (!oldStorageConfig.empty())
		{
			m_errorhnd->report( ErrorCodeLogicError, _TXT("internal: inconsistency in storage config description: %s"), oldStorageConfig.c_str());
		}
	}

	bool isValidKey( std::string& key) const
	{
		return m_validConfigSet.find( key) != m_validConfigSet.end();
	}

	void add( const std::string& key, const std::string& value)
	{
		m_oldConfig.erase( key);
		m_newConfig[ key] = value;
	}

	std::string get( const std::string& key)
	{
		std::map<std::string,std::string>::const_iterator ci = m_newConfig.find( key);
		return ci == m_newConfig.end() ? std::string() : ci->second;
	}

	void remove( const std::string& key)
	{
		m_oldConfig.erase( key);
		m_newConfig.erase( key);
	}

	static std::set<std::string> getValidConfigSet( StorageClientInterface* cli_)
	{
		std::set<std::string> rt;
		char const** ai = cli_->getConfigParameters();
		for (; *ai; ++ai)
		{
			rt.insert( *ai);
		}
		return rt;
	}

	std::string tostring()
	{
		std::string rt;
		std::map<std::string,std::string>::const_iterator ci = m_oldConfig.begin(), ce = m_oldConfig.end();
		for (; ci != ce; ++ci)
		{
			if (strus::addConfigStringItem( rt, ci->first, ci->second, m_errorhnd)) return std::string();
		}
		ci = m_newConfig.begin(), ce = m_newConfig.end();
		for (; ci != ce; ++ci)
		{
			if (strus::addConfigStringItem( rt, ci->first, ci->second, m_errorhnd)) return std::string();
		}
		return rt;
	}
};

static bool defineConfigPatchOp( PatchConfigDef& cfg, ErrorBufferInterface* errorhnd, const PatchDef& patchdef)
{
	const char* context = "storage configuration";
	if (!patchdef.path.empty())
	{
		errorhnd->report( ErrorCodeNotImplemented, "element '%s' required in every element of patch %s" , "path", context);
		return false;
	}
	if (patchdef.path.size() > 1)
	{
		errorhnd->report( ErrorCodeNotFound, "'%s' with more than one identifier not allowed in patch %s", "path", context);
		return false;
	}
	if (patchdef.from.size() > 1)
	{
		errorhnd->report( ErrorCodeNotFound, "'%s' with more than one identifier not allowed in patch %s", "from", context);
		return false;
	}
	switch (patchdef.op)
	{
		case PatchDef::OpAdd:
			if (!patchdef.from.empty())
			{
				errorhnd->report( ErrorCodeNotImplemented, "element '%s' not implemented in patch %s" , "from", context);
				return false;
			}
			if (patchdef.value.empty())
			{
				errorhnd->report( ErrorCodeNotFound, _TXT("value mandatory for %s in %s"), PatchDef::opName(patchdef.op), context);
				return false;
			}
			cfg.add( patchdef.path[0], patchdef.value);
			break;
		case PatchDef::OpRemove:
			if (!patchdef.from.empty())
			{
				errorhnd->report( ErrorCodeNotImplemented, "element '%s' not implemented in patch %s" , "from", context);
				return false;
			}
			if (!patchdef.value.empty())
			{
				errorhnd->report( ErrorCodeNotFound, _TXT("value not allowed for %s in %s"), PatchDef::opName(patchdef.op), context);
				return false;
			}
			cfg.remove( patchdef.path[0]);
			break;
		case PatchDef::OpReplace:
			if (patchdef.from.empty())
			{
				errorhnd->report( ErrorCodeNotImplemented, "element '%s' missing in for %s patch %s" , "from", PatchDef::opName(patchdef.op), context);
				return false;
			}
			if (patchdef.value.empty())
			{
				std::string value = cfg.get( patchdef.from[0]);
				cfg.add( patchdef.path[0], value);
			}
			else
			{
				cfg.add( patchdef.path[0], patchdef.value);
			}
			cfg.remove( patchdef.path[0]);
			break;
		case PatchDef::OpCopy:
		case PatchDef::OpMove:
		case PatchDef::OpTest:
			errorhnd->report( ErrorCodeNotImplemented, _TXT("operator '%s' not implemented for %s"), PatchDef::opName(patchdef.op), context);
			return false;
	}
	return true;
}

void StorageClientImpl::patch( const ValueVariant& patchlist)
{
	StorageClientInterface* THIS = m_storage_impl.getObject<StorageClientInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));

	std::vector<PatchDef> patchDefList = PatchDef::parseList( patchlist);
	std::vector<PatchDef>::iterator pi = patchDefList.begin(), pe = patchDefList.end();
	strus::Reference<strus::StorageMetaDataTransactionInterface> transaction( THIS->createMetaDataTransaction());
	PatchConfigDef configDef( THIS, errorhnd);

	for (; pi != pe; ++pi)
	{
		if (pi->path.empty())
		{
			errorhnd->report( ErrorCodeNotFound, _TXT("path has to be defined always in a %s"), "patch definition list");
			break;
		}
		if (pi->path[0] == "metadata")
		{
			if (!defineMetaDataTransactionOp( transaction.get(), errorhnd, *pi)) break;
		}
		else if (configDef.isValidKey( pi->path[0]))
		{
			if (!defineConfigPatchOp( configDef, errorhnd, *pi)) break;
		}
		else
		{
			errorhnd->report( ErrorCodeNotImplemented, _TXT("unknown configuration parameter '%s' (case sensitive)"), pi->path[0].c_str());
			break;
		}
	}

	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to patch storage configuration: %s"), errorhnd->fetchError());
	}
	if (!transaction->commit())
	{
		throw strus::runtime_error(_TXT("meta data transaction failed when patching storage configuration: %s"), errorhnd->fetchError());
	}
	strus::Reference<MetaDataReaderInterface> metadataReader( THIS->createMetaDataReader());
	if (!metadataReader.get())
	{
		throw strus::runtime_error(_TXT("failed to create meta data reader: %s"), errorhnd->fetchError());
	}
	std::string metadatadef;
	strus::Index ei = 0, ee = metadataReader->nofElements();
	for (; ei != ee; ++ei)
	{
		if (!metadatadef.empty()) metadatadef.push_back(',');
		metadatadef.append( strus::string_format("%s %s", metadataReader->getName(ei), metadataReader->getType(ei)));
	}
	configDef.add( "metadata", metadatadef);

	std::string configstr = configDef.tostring();
	if (!THIS->reload( configstr))
	{
		throw strus::runtime_error( "error reloading configuration: %s", errorhnd->fetchError());
	}
}

