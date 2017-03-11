/*
 * Copyright (c) 2016 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Parser for Strus interfaces
/// \file interfaceParser.hpp
#ifndef _STRUS_BINDINGS_INTERFACE_PARSER_HPP_INCLUDED
#define _STRUS_BINDINGS_INTERFACE_PARSER_HPP_INCLUDED
#include <string>
#include <vector>
#include <map>
#include <set>

namespace strus
{

class VariableType
{
public:
	explicit VariableType( const char* pattern, const char* scope_class, const char* scope_method);
	VariableType( const VariableType& o);
	~VariableType();

	VariableType& operator()( const char* eventname, const char* output);

	bool parse( std::map<std::string,std::string>& defmap, char const*& si, const char* se) const;

	std::string expand( 
			const char* eventname,
			const std::map<std::string,std::string>& defmap,
			const std::string& name,
			const std::string& value) const;

	const std::string& source() const;
	std::string tostring() const;
	const std::string& scope_class() const;
	const std::string& scope_method() const;

private:
	class Impl;
	Impl* m_impl;
};

class VariableValue
{
public:
	VariableValue()
		:m_type(0),m_defmap(){}
	VariableValue( const VariableType* type_, const std::map<std::string,std::string>& defmap_)
		:m_type(type_),m_defmap(defmap_){}
	VariableValue( const VariableValue& o)
		:m_type(o.m_type),m_defmap(o.m_defmap){}
	~VariableValue(){}

	const VariableType& type() const			{return *m_type;}
	const std::map<std::string,std::string>& defmap()	{return m_defmap;}
	std::string tostring() const;

	std::string expand( 
			const char* eventname,
			const std::string& name=std::string(),
			const std::string& value=std::string()) const
	{
		return m_type->expand( eventname, m_defmap, name, value);
	}
private:
	const VariableType* m_type;
	std::map<std::string,std::string> m_defmap;
};

class TypeSystem
{
public:
	TypeSystem(){}
	~TypeSystem(){}

	VariableType& defineType( const char* pattern, const char* scope_class=0, const char* scope_method=0);
	VariableValue parse( const std::string& className, const std::string& methodName, char const*& si, const char* se) const;

	std::string tostring() const;

private:
	std::vector<VariableType> m_variableTypes;
};

class MethodDef
{
public:
	MethodDef( const std::string& name_, const VariableValue& returnvalue_, const std::vector<VariableValue>& param_, bool isconst_)
		:m_name(name_),m_returnvalue(returnvalue_),m_param(param_),m_isconst(isconst_){}
	MethodDef( const MethodDef& o)
		:m_name(o.m_name),m_returnvalue(o.m_returnvalue),m_param(o.m_param),m_isconst(o.m_isconst){}

	const std::string& name() const				{return m_name;}
	const VariableValue& returnValue() const		{return m_returnvalue;}
	const std::vector<VariableValue>& parameters() const	{return m_param;}
	bool isconst() const					{return m_isconst;}

	std::string tostring() const;

private:
	std::string m_name;
	VariableValue m_returnvalue;
	std::vector<VariableValue> m_param;
	bool m_isconst;
};

class ClassDef
{
public:
	ClassDef( const ClassDef& o)
		:m_name(o.m_name),m_methodar(o.m_methodar){}
	explicit ClassDef( const std::string& name_)
		:m_name(name_){}

	void addMethod( const MethodDef& method)
	{
		m_methodar.push_back( method);
	}
	const std::string& name() const
	{
		return m_name;
	}
	const std::vector<MethodDef>& methodDefs() const
	{
		return m_methodar;
	}
	std::string tostring() const;

private:
	std::string m_name;
	std::vector<MethodDef> m_methodar;
};

class InterfacesDef
{
public:
	InterfacesDef( const InterfacesDef& o)
		:m_typeSystem(o.m_typeSystem),m_classdefar(o.m_classdefar){}
	explicit InterfacesDef( const TypeSystem* typeSystem_)
		:m_typeSystem(typeSystem_),m_classdefar(){}

	void addSource( const std::string& source);

	const std::vector<ClassDef>& classDefs() const
	{
		return m_classdefar;
	}
	std::string tostring() const;

private:
	void parseClass( const std::string& className, char const*& si, const char* se);
	std::vector<VariableValue> parseParameters(
			const std::string& scope_class,
			const std::string& scope_method,
			char const*& si, const char* se);

private:
	const TypeSystem* m_typeSystem;
	std::vector<ClassDef> m_classdefar;
};
} //namespace
#endif

