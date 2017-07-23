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
typedef std::multimap<std::string,std::string> AnnotationMap;
typedef std::pair<std::string,std::string> AnnotationMapElem;

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
	bool hasEvent( const char* eventname) const;

	const std::string& source() const;
	std::string tostring() const;
	const std::string& scope_class() const;
	const std::string& scope_method() const;

private:
	class Impl;
	Impl* m_impl;
};

typedef std::multimap<std::string,std::string> DocTagMap;

class VariableValue
{
public:
	VariableValue()
		:m_type(0),m_defmap(),m_name(),m_doc(){}
	VariableValue( const std::string& name_, const VariableType* type_, const std::map<std::string,std::string>& defmap_, const DocTagMap& doc_)
		:m_type(type_),m_defmap(defmap_),m_name(name_),m_doc(doc_){}
	VariableValue( const VariableValue& o)
		:m_type(o.m_type),m_defmap(o.m_defmap),m_name(o.m_name),m_doc(o.m_doc){}
	~VariableValue(){}

	const std::string& name() const				{return m_name;}
	const DocTagMap& doc() const				{return m_doc;}
	const VariableType& type() const			{return *m_type;}
	const std::map<std::string,std::string>& defmap()	{return m_defmap;}
	std::string tostring() const;

	void setName( const std::string& name_)			{m_name = name_;}
	void setDoc( const DocTagMap& doc_)			{m_doc = doc_;}

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
	std::string m_name;
	DocTagMap m_doc;
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
	MethodDef( const std::string& name_, const VariableValue& returnvalue_, const std::vector<VariableValue>& param_, bool isconst_, const DocTagMap& doc_)
		:m_name(name_),m_doc(doc_),m_returnvalue(returnvalue_),m_param(param_),m_isconst(isconst_){}
	MethodDef( const MethodDef& o)
		:m_name(o.m_name),m_doc(o.m_doc),m_returnvalue(o.m_returnvalue),m_param(o.m_param),m_isconst(o.m_isconst){}

	const std::string& name() const				{return m_name;}
	const DocTagMap& doc() const				{return m_doc;}
	const VariableValue& returnValue() const		{return m_returnvalue;}
	const std::vector<VariableValue>& parameters() const	{return m_param;}
	bool isconst() const					{return m_isconst;}

	std::string tostring() const;

private:
	std::string m_name;
	DocTagMap m_doc;
	VariableValue m_returnvalue;
	std::vector<VariableValue> m_param;
	bool m_isconst;
};

class ConstructorDef
{
public:
	ConstructorDef( const std::vector<VariableValue>& param_, const DocTagMap& doc_)
		:m_doc(doc_),m_param(param_){}
	ConstructorDef( const ConstructorDef& o)
		:m_doc(o.m_doc),m_param(o.m_param){}

	const std::vector<VariableValue>& parameters() const	{return m_param;}
	const DocTagMap& doc() const				{return m_doc;}

	std::string tostring() const;

private:
	DocTagMap m_doc;
	std::vector<VariableValue> m_param;
};

class ClassDef
{
public:
	ClassDef( const ClassDef& o)
		:m_name(o.m_name),m_doc(o.m_doc),m_constructorar(o.m_constructorar),m_methodar(o.m_methodar){}
	ClassDef( const std::string& name_, const DocTagMap& doc_)
		:m_name(name_),m_doc(doc_),m_constructorar(),m_methodar(){}

	void addMethod( const MethodDef& method)
	{
		m_methodar.push_back( method);
	}
	void addConstructor( const ConstructorDef& constructor)
	{
		m_constructorar.push_back( constructor);
	}
	const std::string& name() const
	{
		return m_name;
	}
	const DocTagMap& doc() const
	{
		return m_doc;
	}
	const std::vector<MethodDef>& methodDefs() const
	{
		return m_methodar;
	}
	const std::vector<ConstructorDef>& constructorDefs() const
	{
		return m_constructorar;
	}
	std::string tostring() const;

private:
	std::string m_name;
	DocTagMap m_doc;
	std::vector<ConstructorDef> m_constructorar;
	std::vector<MethodDef> m_methodar;
};


class InterfacesDef
{
public:
	InterfacesDef( const InterfacesDef& o)
		:m_typeSystem(o.m_typeSystem),m_classdefar(o.m_classdefar),m_classdefmap(o.m_classdefmap){}
	explicit InterfacesDef( const TypeSystem* typeSystem_)
		:m_typeSystem(typeSystem_),m_classdefar(),m_classdefmap(){}

	void addSource( const std::string& source);
	void checkUnresolved() const;

	const std::vector<ClassDef>& classDefs() const
	{
		return m_classdefar;
	}
	std::string tostring() const;

private:
	void parseClass(
			const std::string& className,
			const DocTagMap& classDoc,
			const std::string& classScope,
			char const*& si, const char* se);

	std::vector<VariableValue> parseParameters(
			const std::string& scope_class,
			const std::string& scope_method,
			const AnnotationMap& annotationMap,
			char const*& si, const char* se);

private:
	const TypeSystem* m_typeSystem;
	std::vector<ClassDef> m_classdefar;
	std::map<std::string,int> m_classdefmap;
};
} //namespace
#endif

