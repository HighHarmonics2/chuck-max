//-----------------------------------------------------------------------------
// Entaro ChucK Developer!
// This is a Chugin boilerplate, generated by chuginate!
//-----------------------------------------------------------------------------

// this should align with the correct versions of these ChucK files
#include "chuck_dl.h"
#include "chuck_def.h"
#include "chuck_errmsg.h"
#include "chuck_instr.h"
#include "chuck_type.h"
#include "chuck_vm.h"

#ifdef _WIN32
#include "regex/regex.h"
#else
#include <regex.h>
#endif

#define CK_REGEX_MAX_MATCHES (10)

CK_DLL_SFUN( regex_match );
CK_DLL_SFUN( regex_match2 );
CK_DLL_SFUN( regex_replace );
CK_DLL_SFUN( regex_replaceAll );

// dynamic linking query function
// DLL_QUERY regex_query( Chuck_DL_Query * QUERY )
CK_DLL_QUERY(RegEx)
{
    // set name
    QUERY->setname( QUERY, "RegEx" );

    // begin class
    QUERY->begin_class( QUERY, "RegEx", "Object" );
    QUERY->doc_class( QUERY, "Class for regular expression matching and replacing in strings. Regex style is POSIX-extended." );

    // add match
    QUERY->add_sfun( QUERY, regex_match, "int", "match" );
    QUERY->add_arg( QUERY, "string", "pattern");
    QUERY->add_arg( QUERY, "string", "str");
    QUERY->doc_func( QUERY, "Return true if match for pattern is found in str, false otherwise." );

    // add match2
    QUERY->add_sfun( QUERY, regex_match2, "int", "match" );
    QUERY->add_arg( QUERY, "string", "pattern");
    QUERY->add_arg( QUERY, "string", "str");
    QUERY->add_arg( QUERY, "string[]", "matches");
    QUERY->doc_func( QUERY, "Return the match and sub-patterns in matches. matches[0] in the entire matched pattern, matches[1] is the first sub-pattern (if any), and so on." );

    // add replace
    QUERY->add_sfun( QUERY, regex_replace, "string", "replace" );
    QUERY->add_arg( QUERY, "string", "pattern");
    QUERY->add_arg( QUERY, "string", "replacement");
    QUERY->add_arg( QUERY, "string", "str");
    QUERY->doc_func( QUERY, "Replace the first instance of pattern in str with replacement, returning the result." );
    
    // add replaceAll
    QUERY->add_sfun( QUERY, regex_replaceAll, "string", "replaceAll" );
    QUERY->add_arg( QUERY, "string", "pattern");
    QUERY->add_arg( QUERY, "string", "replacement");
    QUERY->add_arg( QUERY, "string", "str");
    QUERY->doc_func( QUERY, "Replace all instances of pattern in str with replacement, returning the result.");
    
    QUERY->end_class( QUERY );

    return TRUE;
}

CK_DLL_SFUN( regex_match )
{
    Chuck_String * pattern = GET_NEXT_STRING(ARGS);
    Chuck_String * str = GET_NEXT_STRING(ARGS);

    regex_t regex;
    t_CKBOOL r_free = FALSE;
    int result = 0;

    if(pattern == NULL)
    {
        // throw_exception(SHRED, "NullPointerException", "RegEx.match: argument 'pattern' is null");
        fprintf( stderr, "RegEx.match(): argument 'pattern' is null\n" );
        goto error;
    }
    if(str == NULL)
    {
        // throw_exception(SHRED, "NullPointerException", "RegEx.match: argument 'str' is null");
        fprintf( stderr, "RegEx.match(): argument 'str' is null\n" );
        goto error;
    }

    result = regcomp(&regex, pattern->str().c_str(), REG_EXTENDED | REG_NOSUB);
    if(result != 0)
        goto error;

    r_free = TRUE;

    result = regexec(&regex, str->str().c_str(), 0, NULL, 0);

    RETURN->v_int = (result == 0 ? 1 : 0);

    regfree(&regex);
    r_free = FALSE;

    return;

error:
    if(result != 0)
    {
        char buf[256];
        regerror(result, &regex, buf, 256);
        fprintf( stderr, "RegEx.match(): regex reported error: %s\n", buf);
    }

    if(r_free)
    {
        regfree(&regex);
        r_free = FALSE;
    }

    RETURN->v_int = 0;
}


CK_DLL_SFUN( regex_match2 )
{
    Chuck_String * pattern = GET_NEXT_STRING(ARGS);
    Chuck_String * str = GET_NEXT_STRING(ARGS);
    Chuck_ArrayInt * matches = (Chuck_ArrayInt *) GET_NEXT_OBJECT(ARGS);

    regex_t regex;
    t_CKBOOL r_free = FALSE;
    size_t i;
    regmatch_t *matcharray = NULL;
    int result = 0;

    if(pattern == NULL)
    {
        // throw_exception(SHRED, "NullPointerException", "RegEx.match: argument 'pattern' is null");
        fprintf( stderr, "RegEx.match(): argument 'pattern' is null\n" );
        goto error;
    }
    if(str == NULL)
    {
        // throw_exception(SHRED, "NullPointerException", "RegEx.match: argument 'str' is null");
        fprintf( stderr, "RegEx.match(): argument 'str' is null\n" );
        goto error;
    }
    if(matches == NULL)
    {
        // throw_exception(SHRED, "NullPointerException", "RegEx.match: argument 'matches' is null");
        fprintf( stderr, "RegEx.match(): argument 'matches' is null\n" );
        goto error;
    }


    // matches->clear();
    // bugfix: array.clear() doesnt seem to work?
    matches->set_size(0);

    result = regcomp(&regex, pattern->str().c_str(), REG_EXTENDED);
    if(result != 0)
        goto error;

    r_free = TRUE;

    matcharray = new regmatch_t[regex.re_nsub+1];

    result = regexec(&regex, str->str().c_str(), regex.re_nsub+1, matcharray, 0);

    RETURN->v_int = (result == 0 ? 1 : 0);

    regfree(&regex);
    r_free = FALSE;

    if(result == 0)
    {
        for(i = 0; i < regex.re_nsub+1; i++)
        {
            // create string, no add ref since we are return this without keeping a ref to it
            Chuck_String * match = (Chuck_String *)API->object->create_string( VM, "", FALSE );
            if(matcharray[i].rm_so >= 0 && matcharray[i].rm_eo > 0)
                match->set( std::string(str->str(), matcharray[i].rm_so,
                                         matcharray[i].rm_eo-matcharray[i].rm_so) );
            // append to matches
            // matches->push_back((t_CKUINT)match);
            API->object->array_int_push_back(matches, (t_CKUINT)match);
        }
    }

    CK_SAFE_DELETE_ARRAY(matcharray);

    return;

error:
    if(result != 0)
    {
        char errbuf[256];

        regerror(result, &regex, errbuf, 256);
        fprintf( stderr, "RegEx.match(): regex reported error: %s\n", errbuf);
    }

    if(r_free)
    {
        regfree(&regex);
        r_free = FALSE;
    }

    CK_SAFE_DELETE_ARRAY(matcharray);

    RETURN->v_int = 0;
}


CK_DLL_SFUN( regex_replace )
{
    Chuck_String * pattern = GET_NEXT_STRING(ARGS);
    Chuck_String * replace = GET_NEXT_STRING(ARGS);
    Chuck_String * str = GET_NEXT_STRING(ARGS);

    // create string, no add ref since we are return this without keeping a ref to it
    Chuck_String * ret = (Chuck_String *)API->object->create_string(VM, str->str().c_str(), FALSE );

    regex_t regex;
    t_CKBOOL r_free = FALSE;
    regmatch_t * matcharray = NULL;
    int result = 0;

    if(pattern == NULL)
    {
        // throw_exception(SHRED, "NullPointerException",
        //                 "RegEx.match: argument 'pattern' is null");
        fprintf( stderr, "RegEx.replace(): argument 'pattern' is null\n" );
        goto error;
    }
    if(str == NULL)
    {
        // throw_exception(SHRED, "NullPointerException",
        //                 "RegEx.match: argument 'str' is null");
        fprintf( stderr, "RegEx.replace(): argument 'str' is null\n" );
        goto error;
    }
    if(replace == NULL)
    {
        // throw_exception(SHRED, "NullPointerException",
        //                 "RegEx.match: argument 'replace' is null");
        fprintf( stderr, "RegEx.replace(): argument 'replace' is null\n" );
        goto error;
    }

    // compile regex
    result = regcomp(&regex, pattern->str().c_str(), REG_EXTENDED);
    if(result != 0)
        goto error;

    r_free = TRUE;

    // perform match
    matcharray = new regmatch_t[regex.re_nsub+1];
    result = regexec(&regex, str->str().c_str(), regex.re_nsub+1, matcharray, 0);

    regfree(&regex);
    r_free = FALSE;

    // perform substitution
    if(result == 0 && matcharray[0].rm_so >= 0 && matcharray[0].rm_eo >= 0)
    {
        std::string s = ret->str();
        s.replace(matcharray[0].rm_so,
                  matcharray[0].rm_eo-matcharray[0].rm_so, replace->str());
        ret->set( s );
    }

    CK_SAFE_DELETE_ARRAY(matcharray);

    RETURN->v_string = ret;

    return;

error:
    if(result != 0)
    {
        char errbuf[256];

        regerror(result, &regex, errbuf, 256);
        fprintf( stderr, "RegEx.replace(): regex reported error: %s\n", errbuf);
    }

    if(r_free)
    {
        regfree(&regex);
        r_free = FALSE;
    }

    RETURN->v_int = 0;
}


CK_DLL_SFUN( regex_replaceAll )
{
    Chuck_String * pattern = GET_NEXT_STRING(ARGS);
    Chuck_String * replace = GET_NEXT_STRING(ARGS);
    Chuck_String * str = GET_NEXT_STRING(ARGS);

    // create string, no add ref since we are return this without keeping a ref to it
    Chuck_String * ret = (Chuck_String *)API->object->create_string(VM, str->str().c_str(), FALSE );

    regex_t regex;
    t_CKBOOL r_free = FALSE;
    regmatch_t *matcharray = NULL;
    int result = 0;
    size_t pos = 0;

    if(pattern == NULL)
    {
        // throw_exception(SHRED, "NullPointerException",
        //                 "RegEx.match: argument 'pattern' is null");
        fprintf( stderr, "RegEx.replaceAll(): argument 'pattern' is null\n" );
        goto error;
    }
    if(str == NULL)
    {
        // throw_exception(SHRED, "NullPointerException",
        //                 "RegEx.match: argument 'str' is null");
        fprintf( stderr, "RegEx.replaceAll(): argument 'str' is null\n" );
        goto error;
    }
    if(replace == NULL)
    {
        // throw_exception(SHRED, "NullPointerException",
        //                 "RegEx.match: argument 'replace' is null");
        fprintf( stderr, "RegEx.replaceAll(): argument 'replace' is null\n" );
        goto error;
    }

    // compile regex
    result = regcomp(&regex, pattern->str().c_str(), REG_EXTENDED);
    if(result != 0)
        goto error;

    r_free = TRUE;

    // perform match
    matcharray = new regmatch_t[regex.re_nsub+1];

    while(pos < str->str().size())
    {
        result = regexec(&regex, ret->str().c_str()+pos, regex.re_nsub+1, matcharray, 0);

        // perform substitution
        if(result != 0)
            break;
        else if(matcharray[0].rm_so >= 0 && matcharray[0].rm_eo >= 0)
        {
            std::string s = ret->str();
            s.replace(pos+matcharray[0].rm_so,
                      matcharray[0].rm_eo-matcharray[0].rm_so,
                      replace->str());
            ret->set( s );

            pos = pos + matcharray[0].rm_so + replace->str().size();
        }
    }

    CK_SAFE_DELETE_ARRAY(matcharray);

    regfree(&regex);
    r_free = FALSE;

    RETURN->v_string = ret;

    return;

error:
    if(result != 0)
    {
        char errbuf[256];

        regerror(result, &regex, errbuf, 256);
        fprintf( stderr, "RegEx.replaceAll(): regex reported error: %s\n", errbuf);
    }

    if(r_free)
    {
        regfree(&regex);
        r_free = FALSE;
    }

    RETURN->v_int = 0;
}
