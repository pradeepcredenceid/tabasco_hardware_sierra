/*  -------------------------------------------------------------------------
    Copyright (C) 2011-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/libFlsTool/src/PrgOptions.cpp $
       $$CC-Version: .../oint_tools_flashtoole2_01/11 $
       $$Date:       2014-03-13    15:37:31 UTC $
     ---------------------------------------------------------------------- */

//#define DEBUG_PRGOPTIONS
//#define DEBUG_ALOT

#include "PrgOptions.h"

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;


PrgOptions::PrgOptions(uint32_t size)
  : m_long_only_count(256),
    m_count(0),
    m_size(size)
{
  options = (PrgOptionType**)malloc(sizeof(void*) * (m_size+1));
}

PrgOptions::PrgOptions(const PrgOptions& __copy)
{
  m_count = __copy.m_count;
  m_size = __copy.m_size;
  m_head = __copy.m_head;
  m_tail = __copy.m_tail;
  m_description = __copy.m_description;
  m_separator = __copy.m_separator;
  m_arguments = __copy.m_arguments;
  m_long_only_count = __copy.m_long_only_count;
  options = NULL;

  if (__copy.options)
  {
    options = (PrgOptionType**)malloc(sizeof(void*) * m_size);
    if (options)
    {
      for(uint32_t i=0; i < m_size; ++i)
        options[i] = __copy.options[i];
    }
  }
}

PrgOptions& PrgOptions::operator=(const PrgOptions& __copy)
{
  if(this != &__copy)
  {
    m_count = __copy.m_count;
    m_size = __copy.m_size;
    m_head = __copy.m_head;
    m_tail = __copy.m_tail;
    m_description = __copy.m_description;
    m_separator = __copy.m_separator;
    m_arguments = __copy.m_arguments;
    m_long_only_count = __copy.m_long_only_count;

    if (options)
      free(options);

    options = NULL;
    if (__copy.options)
    {
      options = (PrgOptionType**)malloc(sizeof(void*) * m_size);
      if (options)
      {
        for(uint32_t i=0; i < m_size; ++i)
          options[i] = __copy.options[i];
      }
    }
  }
  return *this;
}


PrgOptions::~PrgOptions()
{
  if (!options)
    return;

  uint32_t i;
  for (i=0; i < m_count; ++i)
  {
    if (options[i]->m_long_option)
      free( options[i]->m_long_option );
  }

  free(options);
}



void PrgOptions::add(const char* new_option, const char* description)
{
  if (!new_option) return; //< Never gonna happen (KlocWorks)

  if (m_size < (m_count+1))
  {
    cout << "PrgOptions> Realloc (" << (m_size+1) << ")!" << endl;
    options = (PrgOptionType**)realloc(options, sizeof(void*) * (++m_size));
    if (!options)
    {
      perror("Realloc failed");
      return;
    }
  }

  PrgOptionType* opt = new PrgOptionType(new_option, description);
  if (!opt)
    throw_exception("Memory allocation error");

  // Make sure there is an unique option ID
  if (!opt->m_option && opt->m_long_option)
    opt->m_option = m_long_only_count++;

  options[m_count++] = opt;
}


bool PrgOptions::is_set(const char *opt)
{
  PrgOptionType *obj = find(opt);
  if (!obj)
    throw_exception("Didn't find "+ string(opt), false);

  return (obj && obj->m_value) ? true : false;
}

/**
 * Locate an option in the option list
 */
PrgOptionType *PrgOptions::find(int opt)
{
  if(opt == 0 || !options)
    return NULL;

  for(int i=0; options[i]; ++i)
    if (opt == options[i]->m_option)
        return options[i];

  stringstream ss;
  ss << "Option '" << char(opt) << "' does not exist!" << endl;
  throw_exception( ss.str(), NULL);
}

PrgOptionType *PrgOptions::find(const char *opt)
{
  if(!opt || !options)
  {
    perror("No option!");
    return NULL;
  }

  bool long_option = (strlen(opt) > 1);

  for(int i=0; options[i]; ++i)
  {
    if (long_option && options[i]->m_long_option)
    {
      if (strcmp(opt, options[i]->m_long_option) == 0)
        return options[i];
    }
    else if (options[i]->m_option)
    {
      if (int(opt[0]) == options[i]->m_option)
        return options[i];
    }
  }
  return NULL;
}


/**
 * Call directly with the arguments from main.
 */
bool PrgOptions::parse(int argc, char** argv)
{
  if(!options)
    throw_exception("No options to parse", false);

  uint32_t i, opt_count = 0;
  vector<char> short_options;
  vector<struct option> long_options;

  long_options.resize(m_count+1);
  short_options.push_back(':'); //< Return ':' on missing argument

  //cout << "Parsing options (" << m_count << ")..." << endl;
  for(i=0; i < m_count; ++i)
  {
    if (options[i]->m_long_option)
    {
      long_options[opt_count].flag = 0;
      long_options[opt_count].val = options[i]->m_option;
      long_options[opt_count].name = options[i]->m_long_option;
      long_options[opt_count].has_arg = options[i]->m_has_arg;
      if (options[i]->m_option > ' ' && options[i]->m_option < '~')
      {
        short_options.push_back(options[i]->m_option);
        if (options[i]->m_has_arg)      short_options.push_back(':');  //< Argument
        if (options[i]->m_has_arg == 2) short_options.push_back(':');  //< Optional argument
      }
      opt_count++;
    }
  }
  short_options.push_back('\0');

  for(; opt_count <= m_count; ++opt_count)
  {
    //cout << "Filling zero's in " << opt_count << "..." << endl;
    long_options[opt_count].name = 0;
    long_options[opt_count].flag = 0;
    long_options[opt_count].val = 0;
    long_options[opt_count].has_arg = 0;
  }

#ifdef DEBUG_PRGOPTIONS
  cout << "Options list: '" << short_options.size() << "'" << endl;
  cout << "struct option long_options[] = {" << endl;
  for (int i=0; long_options[i].name; ++i)
  {
    cout << "  " << i << " { " << long_options[i].name;
    cout << ", " << long_options[i].has_arg;
    if (long_options[i].flag)
      cout << ", Flag";
    if (long_options[i].val > ' ' && long_options[i].val < '~')
      cout << ", " << char(long_options[i].val);
    cout << " } " << endl;
  }
  cout << "};" << endl;
#endif

  PrgOptionType *obj;
  opterr = 0;
  int c = 'a', option_index = 0;
  while ((c = getopt_long(argc, argv, &short_options[0], &long_options[0], &option_index)) != -1)
  {
    if (c == '?')
    {
      throw_exception("Unknown option: "+ string(argv[optind-1]) +"\n", false);
    }
    else if (c == ':')
    {
      obj = find(optopt);
      if (!obj || !obj->m_long_option)
        throw_exception("Missing argument", false);
      throw_exception("Missing argument for "+ string(obj->m_long_option), false);
    }

    obj = find( c );
    if (!obj) {
      cerr << "Option " << char(c) << "(" << int(c) << ") not defined!" << endl;
      return false;
    }

    #ifdef DEBUG_ALOT
      if (c > ' ' && c < '~')
        cout << "   '" << char(c) << "' ";
      else
        cout << "   '" << obj->m_long_option << "' ";

      if (optarg == 0)
        cout << "that is setting a flag." << endl;
      else if (optarg)
        cout << "with argument '" << optarg << "'" << endl;
      else
        cout << "with no argument ?!?!?!?! " << endl;
    #endif

    if (optarg == 0)
    {
      #ifdef DEBUG_ALOT
        cout << " Setting default value for " << char(obj->m_option)
             << ", " << obj->m_long_option << "..." << endl;
      #endif
      obj->setDefault();
    }
    else if (optarg)
    {
      #ifdef DEBUG_ALOT
        cout << " Saving optarg " << char(obj->m_option)
             << " (Type: " << obj->m_save_typeid << ")..." << endl;
      #endif

      //< Save the value in the right union type and point with the void*
      switch(obj->m_save_typeid)
      {
        case 'a': obj->set( StringTo<int8_t>(optarg) );    break;
        case 's': obj->set( StringTo<int16_t>(optarg) );   break;
        case 'i': obj->set( StringTo<int32_t>(optarg) );   break;
        case 'x': obj->set( StringTo<int64_t>(optarg) );   break;
        case 'h': obj->set( StringTo<uint8_t>(optarg) );   break;
        case 't': obj->set( StringTo<uint16_t>(optarg) );  break;
        case 'j': obj->set( StringTo<uint32_t>(optarg) );  break;
        case 'y': obj->set( StringTo<uint64_t>(optarg) );  break;
        case 'S': obj->set( optarg );                      break;
        case 'A': obj->set( optarg );                      break;
        //case 'c': obj->set( string(optarg) );     break;
        default:
          obj->set( optarg );
          //throw_runtime_error("Type not supported :'("+ string(&obj->m_save_typeid) +")", false);
          break;
      }
    }
    else
    {
      throw_exception(" ?!?!?!?! no argument ?!?!?!?! ", false);
    }
  }

  // Non-option arguments are considered input files
  if (optind < argc)
  {
    //cout << "Extra arguments:" << endl;
    for (int i=optind; i < argc; ++i)
    {
      //cout << "  -> " << argv[i] << endl;
      m_arguments.push_back( argv[i] );
    }
  }
  m_arguments.push_back( NULL );
  return true;
}

/**
 * @return The list of arguments in ANSI C style
 */
const char** PrgOptions::getArguments()
{
  //cout << "Number of arguments: " << m_arguments.size() << endl;
  return &m_arguments[0];
}

/**
 * @return The list of arguments in STL C++ style
 */
vector<const char*> PrgOptions::getFiles()
{
  m_arguments.pop_back(); //< remove the NULL
  return m_arguments;
}

/**
 * Show the help of an argument
 */
void PrgOptions::showUsage(const char* arg)
{
  bool found = false;

  if (!arg)
    throw_exception("Please add an option");

  string option = arg;
  bool long_opt = (option.size() > 1);

  for (size_t i=0; i<m_count; ++i)
  {
    PrgOptionType *opt = options[i];

    if ((!long_opt && opt->m_option == int(*arg))
      || (long_opt && opt->m_long_option
          && option.compare(opt->m_long_option) == 0))
    {
      found = true;
      cout << opt->getUsage() << endl;
    }
  }

  if (!found)
    throw_exception("Option "+ option +" not found");
}

/**
 * Showing the description
 */
void PrgOptions::showDescription()
{
  cout << endl;

  if (!m_head.empty())
  {
    cout << m_head << endl;

    if (!m_separator.empty())
      cout << m_separator << endl;
  }

  if (!m_description.empty())
  {
    cout << m_description << endl;

    if (!m_separator.empty())
      cout << m_separator << endl;
  }

  for (size_t i=0; i<m_count; ++i)
  {
    cout << options[i]->getUsage() << endl;
  }

  if (!m_tail.empty())
  {
    if (!m_separator.empty())
      cout << m_separator << endl;

    cout << m_tail << endl;
  }
  cout << endl;
}
