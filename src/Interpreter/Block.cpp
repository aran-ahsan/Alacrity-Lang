/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.

	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <iostream>

#include "../../include/Errors.hpp"
#include "../../include/IO.hpp"
#include "../../include/Parser.hpp"
#include "../../include/Parser/CondStmt.hpp"
#include "../../include/Parser/LoopStmt.hpp"
#include "../../include/Parser/AssignStmt.hpp"
#include "../../include/Parser/FnCallStmt.hpp"
#include "../../include/Parser/BlockStmt.hpp"
#include "../../include/Interpreter/Assignment.hpp"
#include "../../include/Interpreter/FuncCall.hpp"
#include "../../include/Interpreter/Conditional.hpp"
#include "../../include/Interpreter/Loops.hpp"
#include "../../include/Interpreter/Block.hpp"

int Interpreter::Block( const Parser::BlockStmt * blk, const size_t depth, const bool internal_display_enabled )
{
	int res = OK;

	if( blk->size() == 0 ) {
		if( internal_display_enabled ) IO::colout << "Block[" << depth << "]: Nothing to do!\n";
		return res;
	}

	if( internal_display_enabled ) IO::colout << "Block[" << depth << "]: Entering...\n";

	for( auto it = blk->GetStmts().begin(); it != blk->GetStmts().end(); ++it ) {
		auto & stmt = * it;
		if( stmt->GetType() == Parser::ASSIGN ) {
			res = Interpreter::Assignment( static_cast< const Parser::AssignStmt * >( stmt ), depth + 1, internal_display_enabled );
		} else if( stmt->GetType() == Parser::FNCALL ) {
			res = Interpreter::FuncCall( static_cast< const Parser::FnCallStmt * >( stmt ), depth + 1, internal_display_enabled );
		} else if( stmt->GetType() == Parser::BLOCK ) {
			res = Interpreter::Block( static_cast< const Parser::BlockStmt * >( stmt ), depth + 1, internal_display_enabled );
		} else if( stmt->GetType() == Parser::COND ) {
			res = Interpreter::Conditional( static_cast< const Parser::CondStmt * >( stmt ), depth + 1, internal_display_enabled );
		} else if( stmt->GetType() == Parser::LOOP ) {
			res = Interpreter::LoopCall( static_cast< const Parser::LoopStmt * >( stmt ), depth + 1, internal_display_enabled );
		} else {
			if( internal_display_enabled ) IO::colout << "Interpret[" << depth << "] {r}error{0}: Unrecognized object type with value: "
				<< stmt->GetType() << "\n";
			res = UNKNOWN_OBJ_TYPE;
			break;
		}
		if( res == LOOP_BREAK_ENCOUNTERED || res == LOOP_CONTINUE_ENCOUNTERED ) {
			break;
		}
		if( res != OK ) {
			if( res != FAIL_FN_CALLED && internal_display_enabled ) {
				IO::colout << "Interpret[" << depth << "] {r}error{0}: Failed to interpret object "
					<< stmt->GetType() << ", look at error above!\n";
			}
			break;
		}
	}
	if( res != FAIL_FN_CALLED && internal_display_enabled ) {
		IO::colout << "Block[" << depth << "]: Exiting! ";
		if( res != OK && res != LOOP_BREAK_ENCOUNTERED && res != LOOP_CONTINUE_ENCOUNTERED ) {
			IO::colout << "Errors encountered!\n";
		} else {
			IO::colout << "No errors encountered!\n";
		}
	}
	return res;
}