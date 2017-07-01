// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    xmlfile.h

    XML file parsing code.

***************************************************************************/

#pragma once

#ifndef MAME_LIB_UTIL_XMLFILE_H
#define MAME_LIB_UTIL_XMLFILE_H

#include "osdcore.h"
#include "corefile.h"

#include <list>
#include <string>
#include <utility>


/***************************************************************************
    CONSTANTS
***************************************************************************/

enum
{
	XML_PARSE_FLAG_WHITESPACE_SIGNIFICANT = 1
};


/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

// forward type declarations
struct XML_ParserStruct;


/* extended error information from parsing */
struct xml_parse_error
{
	const char *            error_message;
	int                     error_line;
	int                     error_column;
};


// parsing options
struct xml_parse_options
{
	xml_parse_error *       error;
	void                    (*init_parser)(struct XML_ParserStruct *parser);
	uint32_t                flags;
};


// a node representing a data item and its relationships
class xml_data_node
{
public:
	enum class int_format
	{
		DECIMAL,
		DECIMAL_HASH,
		HEX_DOLLAR,
		HEX_C
	};



	/* ----- XML file objects ----- */

	// create a new empty xml file object
	static xml_data_node *file_create();

	// parse an XML file into its nodes */
	static xml_data_node *file_read(util::core_file &file, xml_parse_options const *opts);

	/* parse an XML string into its nodes */
	static xml_data_node *string_read(const char *string, xml_parse_options const *opts);

	// write an XML tree to a file
	void file_write(util::core_file &file) const;

	// free an XML file object
	void file_free();


	/* ----- XML node management ----- */

	char const *get_name() const { return m_name.empty() ? nullptr : m_name.c_str(); }

	char const *get_value() const { return m_value.empty() ? nullptr : m_value.c_str(); }
	void set_value(char const *value);
	void append_value(char const *value, int length);
	void trim_whitespace();

	xml_data_node *get_parent() { return m_parent; }
	xml_data_node const *get_parent() const { return m_parent; }

	// count the number of child nodes
	int count_children() const;

	// get the first child
	xml_data_node *get_first_child() { return m_first_child; }
	xml_data_node const *get_first_child() const { return m_first_child; }

	// find the first child with the given tag
	xml_data_node *get_child(const char *name);
	xml_data_node const *get_child(const char *name) const;

	// find the first child with the given tag and/or attribute/value pair
	xml_data_node *find_first_matching_child(const char *name, const char *attribute, const char *matchval);
	xml_data_node const *find_first_matching_child(const char *name, const char *attribute, const char *matchval) const;

	// get the next sibling
	xml_data_node *get_next_sibling() { return m_next; }
	xml_data_node const *get_next_sibling() const { return m_next; }

	// find the next sibling with the given tag
	xml_data_node *get_next_sibling(const char *name);
	xml_data_node const *get_next_sibling(const char *name) const;

	// find the next sibling with the given tag and/or attribute/value pair
	xml_data_node *find_next_matching_sibling(const char *name, const char *attribute, const char *matchval);
	xml_data_node const *find_next_matching_sibling(const char *name, const char *attribute, const char *matchval) const;

	// add a new child node
	xml_data_node *add_child(const char *name, const char *value);

	// either return an existing child node or create one if it doesn't exist
	xml_data_node *get_or_add_child(const char *name, const char *value);

	// delete a node and its children
	void delete_node();



	/* ----- XML attribute management ----- */

	// return whether a node has the specified attribute
	bool has_attribute(const char *attribute) const;

	// return the string value of an attribute, or the specified default if not present
	const char *get_attribute_string(const char *attribute, const char *defvalue) const;

	// return the integer value of an attribute, or the specified default if not present
	int get_attribute_int(const char *attribute, int defvalue) const;

	// return the format of the given integer attribute
	int_format get_attribute_int_format(const char *attribute) const;

	// return the float value of an attribute, or the specified default if not present
	float get_attribute_float(const char *attribute, float defvalue) const;

	// set the string value of an attribute
	void set_attribute(const char *name, const char *value);

	// set the integer value of an attribute
	void set_attribute_int(const char *name, int value);

	// set the float value of an attribute
	void set_attribute_float(const char *name, float value);

	// add an attribute even if an attribute with the same name already exists
	void add_attribute(const char *name, const char *value);



	int                     line;           /* line number for this node's start */


private:
	// a node representing an attribute
	struct attribute_node
	{
		template <typename T, typename U> attribute_node(T &&name, U &&value) : name(std::forward<T>(name)), value(std::forward<U>(value)) { }

		std::string name;
		std::string value;
	};


	xml_data_node();
	xml_data_node(xml_data_node *parent, const char *name, const char *value);
	~xml_data_node();

	xml_data_node(xml_data_node const &) = delete;
	xml_data_node(xml_data_node &&) = delete;
	xml_data_node &operator=(xml_data_node &&) = delete;
	xml_data_node &operator=(xml_data_node const &) = delete;

	xml_data_node *get_sibling(const char *name);
	xml_data_node const *get_sibling(const char *name) const;
	xml_data_node *find_matching_sibling(const char *name, const char *attribute, const char *matchval);
	xml_data_node const *find_matching_sibling(const char *name, const char *attribute, const char *matchval) const;

	attribute_node *get_attribute(const char *attribute);
	attribute_node const *get_attribute(const char *attribute) const;

	void write_recursive(int indent, util::core_file &file) const;


	xml_data_node *             m_next;
	xml_data_node *             m_first_child;
	std::string                 m_name;
	std::string                 m_value;
	xml_data_node *             m_parent;
	std::list<attribute_node>   m_attributes;
};



/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

/* ----- miscellaneous interfaces ----- */

/* normalize a string into something that can be written to an XML file */
const char *xml_normalize_string(const char *string);

#endif  /* MAME_LIB_UTIL_XMLFILE_H */
