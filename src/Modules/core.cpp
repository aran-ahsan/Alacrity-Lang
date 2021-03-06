/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.

	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <iostream>
#include <string>
#include <limits>

#include "../../include/Errors.hpp"
#include "../../include/Core.hpp"
#include "../../include/String.hpp"
#include "../../include/Env.hpp"
#include "../../include/FS.hpp"
#include "../../include/IO.hpp"
#include "../../include/Interpreter.hpp"
#include "../../include/Interpreter/Block.hpp"
#include "../../include/Interpreter/FnBase.hpp"

static int InterpretFile( const std::string & file, const int depth, const bool internal_display_enabled );

AL_FUNC_FIX_ARG( fail, 1, false, false )
{
	std::string op;
	EVAL_AND_CHECK( "fail", "{br}FAIL{0}: {r}" + args[ 0 ] + "{0}", op );
	std::cout << op;
	std::cout.flush();
	return FAIL_FN_CALLED;
}

AL_FUNC_VAR_ARG( print, 1, -1, false, false )
{
	for( auto & a : args ) {
		std::string op;
		EVAL_AND_CHECK( "print", a, op );
		std::cout << op;
	}
	std::cout.flush();
	return OK;
}

AL_FUNC_FIX_ARG( input, 2, false, false )
{
	std::string op;
	EVAL_AND_CHECK( "input", args[ 1 ], op );
	std::cout << op;
	std::cout.flush();
	std::string tmp;
	std::getline( std::cin, tmp );
	Env::SetVar( args[ 0 ], tmp );
	return OK;
}

AL_FUNC_FIX_ARG( input_num, 2, false, false )
{
	std::string op;
	EVAL_AND_CHECK( "input_num", args[ 1 ], op );
	std::string tmp;
	std::cout << op;
	std::cout.flush();
	while( tmp.empty() ) {
		std::getline( std::cin, tmp );
		if( !Str::IsNum( tmp ) ) {
			std::cout << "Input wasn't a number! Try again: ";
			std::cout.flush();
			tmp.clear();
		}
	}
	Env::SetVar( args[ 0 ], tmp );
	return OK;
}

AL_FUNC_FIX_ARG( set, 2, false, false )
{
	if( args[ 0 ].empty() ) return OK;
	std::string op1, op2;
	EVAL_AND_CHECK( "set", args[ 0 ], op1 );
	EVAL_AND_CHECK( "set", args[ 1 ], op2 );
	if( !Env::SetVar( op1, op2 ) ) {
		std::cout << "Function: set failed to set the variable: " << op1
			<< " to: " << op2 << "\n";
		return ENV_SETVAR_FAILED;
	}
	return OK;
}

AL_FUNC_VAR_ARG( prepend, 3, -1, false, false )
{
	std::string var;
	EVAL_AND_CHECK( "prepend", args[ 0 ], var );
	char delim = args[ 1 ].empty() ? ':' : args[ 1 ][ 0 ];
	for( auto arg = args.begin() + 2; arg != args.end(); ++arg ) {
		std::string farg;
		EVAL_AND_CHECK( "prepend", * arg, farg );
		Env::Prepend( var, farg, delim );
	}
	return OK;
}

AL_FUNC_VAR_ARG( append, 3, -1, false, false )
{
	std::string var;
	EVAL_AND_CHECK( "append", args[ 0 ], var );
	char delim = args[ 1 ].empty() ? ':' : args[ 1 ][ 0 ];
	for( auto arg = args.begin() + 2; arg != args.end(); ++arg ) {
		std::string farg;
		EVAL_AND_CHECK( "append", * arg, farg );
		Env::Append( var, farg, delim );
	}
	return OK;
}

AL_FUNC_VAR_ARG( remove, 3, -1, false, false )
{
	std::string var;
	EVAL_AND_CHECK( "remove", args[ 0 ], var );
	char delim = args[ 1 ].empty() ? ':' : args[ 1 ][ 0 ];
	for( auto arg = args.begin() + 2; arg != args.end(); ++arg ) {
		std::string farg;
		EVAL_AND_CHECK( "remove", * arg, farg );
		Env::Remove( var, farg, delim );
	}
	return OK;
}

AL_FUNC_FIX_ARG( reset, 1, false, false )
{
	std::string var;
	EVAL_AND_CHECK( "reset", args[ 0 ], var );
	Env::Reset( var );
	return OK;
}

AL_FUNC_VAR_ARG( al_add_lib_paths, 1, -1, false, false )
{
	std::string var = Core::ALLibPaths();
	for( auto arg = args.begin(); arg != args.end(); ++arg ) {
		std::string farg;
		EVAL_AND_CHECK( "al_add_lib_paths", * arg, farg );
		if( !farg.empty() && * farg.begin() != '/' ) farg = FS::GetCurrentDir() + "/" + farg;
		Env::Append( var, farg );
	}
	return OK;
}

AL_FUNC_VAR_ARG( al_add_src_paths, 1, -1, false, false )
{
	std::string var = Core::ALSourcePaths();
	for( auto arg = args.begin(); arg != args.end(); ++arg ) {
		std::string farg;
		EVAL_AND_CHECK( "al_add_src_paths", * arg, farg );
		if( !farg.empty() && * farg.begin() != '/' ) farg = FS::GetCurrentDir() + "/" + farg;
		Env::Append( var, farg );
	}
	return OK;
}

AL_FUNC_FIX_ARG( exec, 1, false, false )
{
	std::string op;
	EVAL_AND_CHECK( "exec", args[ 0 ], op );
	Env::Exec( op, nullptr );
	return OK;
}

AL_FUNC_FIX_ARG( eval, 1, false, false )
{
	std::string tmp;
	EVAL_AND_CHECK( "eval", args[ 0 ], tmp );
	return OK;
}

AL_FUNC_FIX_ARG( brkloop, 0, false, false )
{
	return LOOP_BREAK_ENCOUNTERED;
}

AL_FUNC_FIX_ARG( contloop, 0, false, false )
{
	return LOOP_CONTINUE_ENCOUNTERED;
}

AL_FUNC_VAR_ARG( load_file, 1, -1, true, false )
{
	int res = OK;

	for( auto & arg : args ) {
		if( arg.empty() ) continue;

		res = InterpretFile( arg, depth, internal_display_enabled );
		if( res != OK ) break;
	}

	return res;
}

AL_FUNC_VAR_ARG( use_lib, 1, 2, true, false )
{
	if( args.size() > 1 ) 	Env::SetVar( "OPTIONAL_VAR", args[ 1 ] );
	else 			Env::Reset( "OPTIONAL_VAR" );

	return InterpretFile( args[ 0 ], depth, internal_display_enabled );
}

/*
 * a1 -> src
 * a2 -> dest
 */
AL_FUNC_FIX_ARG( install, 2, false, false )
{
	std::string src, dest;
	EVAL_AND_CHECK( "install", args[ 0 ], src );
	EVAL_AND_CHECK( "install", args[ 1 ], dest );
	if( src.empty() || dest.empty() ) return OK;

	if( !FS::CreateDirectoriesForFile( dest ) ) {
		std::cerr << "Failed install: Could not create destination directories for " << dest << "! Ensure that you have required permissions!\n";
		return FAIL;
	}
	std::string cmd_str, cmd_final;
#if __linux__
	cmd_str = "cp -r --remove-destination " + src + " " + dest;
#elif __APPLE__ || __FreeBSD__
	cmd_str = "cp -rf " + src + " " + dest;
#endif
	IO::colout << "{bm}Installing {by}" + src + " {bm}to {by}" + dest << "{0}\n";
	if( Env::GetVar( "CMDS_ONLY" ) == "true" ) { std::cout << cmd_str << "\n"; return OK; }
	return Env::Exec( cmd_str );
}

static int InterpretFile( const std::string & file, const int depth, const bool internal_display_enabled )
{
	auto file_path = FS::GetFilePath( file + ".al", Core::ALSourcePaths() );
	if( file_path.empty() ) return FILE_NOT_FOUND;

	auto file_data_var = FS::ReadFile( file_path );
	if( std::holds_alternative< int >( file_data_var ) ) return std::get< int >( file_data_var );
	std::string file_data = std::get< std::string >( file_data_var );

	auto lex_syms_var = Lex::Tokenize( file_data );

	if( std::holds_alternative< int >( lex_syms_var ) ) return std::get< int >( lex_syms_var );
	auto lex_syms = std::get< Lex::Syms >( lex_syms_var );

	auto parse_syms_var = Parser::ParseTokens( lex_syms.GetSyms() );
	if( std::holds_alternative< int >( parse_syms_var ) ) return std::get< int >( parse_syms_var );
	auto parse_syms = std::get< Parser::ParseTree >( parse_syms_var );

	return Interpreter::Interpret( parse_syms, file_path, depth, internal_display_enabled );
}