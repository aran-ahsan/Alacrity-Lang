/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.

	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <iostream>
#include <vector>
#include <string>
#include <variant>

#include "../../include/IO.hpp"
#include "../../include/Parser/Stmt.hpp"

// Stmt class

Parser::Stmt::Stmt( int type ) : m_type( type ) {}

int Parser::Stmt::GetType() const { return m_type; }