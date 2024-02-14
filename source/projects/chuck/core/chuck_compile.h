/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
    http://chuck.stanford.edu/
    http://chuck.cs.princeton.edu/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// file: chuck_compile.h
// desc: chuck compile system unifying parser, type checker, and emitter
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2005 - original
//-----------------------------------------------------------------------------
#ifndef __CHUCK_COMPILE_H__
#define __CHUCK_COMPILE_H__

#include "chuck_def.h"
#include "chuck_parse.h"
#include "chuck_scan.h"
#include "chuck_type.h"
#include "chuck_emit.h"
#include "chuck_vm.h"
#include <list>


// forward reference
struct Chuck_DLL;




//-----------------------------------------------------------------------------
// name: struct Chuck_Compiler
// desc: the sum of the components in compilation
//-----------------------------------------------------------------------------
struct Chuck_Compiler
{
protected: // data
    // carrier
    Chuck_Carrier * m_carrier;

public: // get protected data
    // REFACTOR-2017: get associated, per-compiler environment
    Chuck_Env * env() const { return m_carrier->env; }
    // REFACTOR-2017: get associated, per-compiler VM
    Chuck_VM * vm() const { return m_carrier->vm; }
    // REFACTOR-2017: get associated, per-compiler carrier
    Chuck_Carrier * carrier() const { return m_carrier; }
    // set carrier
    t_CKBOOL setCarrier( Chuck_Carrier * c ) { m_carrier = c; return TRUE; }

public: // data
    // emitter
    Chuck_Emitter * emitter;
    // generated code
    Chuck_VM_Code * code;

    // auto-depend flag
    t_CKBOOL m_auto_depend;
    // recent map
    std::map<std::string, Chuck_Context *> m_recent;

    // chugins
    std::list<Chuck_DLL *> m_dlls;
    // libraries (ck code) to import
    std::list<std::string> m_cklibs_to_preload;

    // origin hint; this flag is set to different ckte_Origin values
    // to denote where new entities originate | 1.5.0.0 (ge) added
    ckte_Origin m_originHint;

public: // to all
    // contructor
    Chuck_Compiler();
    // destructor
    virtual ~Chuck_Compiler();

    // initialize
    t_CKBOOL initialize();
    // shutdown
    void shutdown();

public: // additional binding
    // bind a new type system module, via query function
    t_CKBOOL bind( f_ck_query query_func, const std::string & name,
                   const std::string & nspc = "global" );

public: // compile
    // set auto depend
    void set_auto_depend( t_CKBOOL v );
    // parse, type-check, and emit a program
    t_CKBOOL go( const std::string & filename,
                 const std::string & full_path = "",
                 const std::string & codeLiteral = "" );
    // resolve a type automatically, if auto_depend is on
    t_CKBOOL resolve( const std::string & type );
    // get the code generated from the last go()
    Chuck_VM_Code * output( );

public: // special FILE input mode | added 1.5.0.5 (ge)
    // set an already open FILE descriptor `fd` for one time use
    // by the next call to go(), which will use `fd` as the input
    // to the parser (NOTE in any invocation of go(), `codeLiteral`
    // and `fd` should not both be non-empty, otherwise a warning
    // will be output and the `codeLiteral` will take precedence
    // and the `fd` will be cleaned up and skipped
    // MEMORY: if `autoClose` is true, the compiler will automatically
    // call fclose() on `fd` on the next call to go(), regardless of
    // the aforementioned conflict with `codeLiteral`
    void set_file2parse( FILE * fd, t_CKBOOL autoClose );

public: // replace-dac | added 1.4.1.0 (jack)
    // sets a "replacement dac": one global UGen is secretly used
    // as a stand-in for "dac" for this compilation;
    // for example, ChuckSubInstances in Chunity use a global Gain as a
    // replacement dac, then use the global getUGenSamples() function to
    // get the samples of the gain. this enables the creation
    // of a new sample sucker.
    void setReplaceDac( t_CKBOOL shouldReplaceDac, const std::string & replacement );

public:
    // chugin load | 1.4.1.0 (ge) refactored
    t_CKBOOL load_external_modules( const char * extension,
                                    std::list<std::string> & chugin_search_paths,
                                    std::list<std::string> & named_dls,
                                    t_CKBOOL recursiveSearch );

    // chugin probe | 1.5.0.4 (ge) added
    static t_CKBOOL probe_external_modules( const char * extension,
                                            std::list<std::string> & chugin_search_paths,
                                            std::list<std::string> & named_dls,
                                            t_CKBOOL recursiveSearch,
                                            std::list<std::string> & ck_libs );

protected: // internal
    // do entire file
    t_CKBOOL do_entire_file( Chuck_Context * context );
    // do just class definitions
    t_CKBOOL do_only_classes( Chuck_Context * context );
    // do all excect classes
    t_CKBOOL do_all_except_classes( Chuck_Context * context );
    // do normal compile
    t_CKBOOL do_normal_depend( const std::string & path,
                               const std::string & codeLiteral = "",
                               const std::string & full_path = "" );
    // do auto-depend compile
    t_CKBOOL do_auto_depend( const std::string & path,
                             const std::string & codeLiteral = "",
                             const std::string & full_path = "" );
    // look up in recent
    Chuck_Context * find_recent_path( const std::string & path );
    // look up in recent
    Chuck_Context * find_recent_type( const std::string & type );
    // add to recent
    t_CKBOOL add_recent_path( const std::string & path, Chuck_Context * context );
};




#endif
