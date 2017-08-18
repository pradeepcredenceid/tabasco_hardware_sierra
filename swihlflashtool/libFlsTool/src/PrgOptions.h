/*  -------------------------------------------------------------------------
    Copyright (C) 2011-2014 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)

     ----------------------------------------------------------------------
     Revision Information:
       $$File name:  /msw_tools/FlashTool/libFlsTool/src/PrgOptions.h $
       $$CC-Version: .../dhkristx_140314_v0/2 $
       $$Date:       2014-05-01    12:46:13 UTC $
     ----------------------------------------------------------------------
    Building on top of getopt.

    Creating the options structure dynamically.
    Keeping the options and long_options aligned.

    Example:
      PrgOptions opt = PrgOptions();
      opt.add("input,i",                       "Sets the input parameter");
      opt.add("input,i=s", &input,             "Saves an input parameter to string pointer");
      opt.add("input,i",   &input, "file.fls", "Above + adds a value to be saved if set");
      opt.add("input,i=s", &input, "file.fls", "Adds an optional default value");

     ---------------------------------------------------------------------- */

#ifndef PRGOPTIONS_H_
#define PRGOPTIONS_H_

#include <inttypes.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "StringExt.h"
#include "Exceptions.h"

#define _FILE_ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#include <typeinfo>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

/**
 * A single program option type.
 *
 * This contains three functions:
 *  setup(variable, default_value) - Assign variable with either default or variable.
 *  set(value)                     - Assigns the value to the variable from above.
 *  get()                          - Return the raw current value
 */
class PrgOptionType
{
public:
  PrgOptionType(const char* option_str, const char* description)
    : m_option(0),
      m_long_option(NULL),
      m_has_arg(0),
      m_arg_type(0),
      m_description(strdup(description)),
      m_save_typeid(0),
      m_save_to(NULL),
      m_default_value(NULL),
      m_value(NULL)
  {
    char**opts = strsplit(option_str, ",");
    if (!opts)
      return; //< Never gonna happen (KlocWorks)

    for(int i=0; opts[i]; ++i)
    {
      bool equal = true;
      char* type = strchr(opts[i], '=');
      if (!type)
      {
        equal = false;
        type = strchr(opts[i], ':');
      }

      if (type)
      {
        *type++ = 0;
        m_arg_type = *type;
        m_has_arg = (equal) ? required_argument : optional_argument;
      }

      if (!m_long_option && strlen(opts[i]) > 1)
        m_long_option = strdup(opts[i]);
      else
        m_option = *opts[i];

    free(opts[i]);
    }
    if (opts)
      free(opts);
  };

  ~PrgOptionType()
  {
    if (m_long_option)
      free(m_long_option);
    if (m_description)
      free(m_description);
  }

  /* The default values */
  union {
    bool    default_b;
    char    default_c;
    int8_t  default_int8;
    int16_t default_int16;
    int32_t default_int32;
    int64_t default_int64;

    uint8_t  default_uint8;
    uint16_t default_uint16;
    uint32_t default_uint32;
    uint64_t default_uint64;
  };

public:
  //< First three are for the "struct option" (flag is always 0)
  int   m_option;              //< Single character argument e.g. "-i"
  char* m_long_option;         //< Multi-character argument e.g. "--input"
  int   m_has_arg;             //< 0=Nope, 1=Yes, 2=Maybe

  //struct option long_option = { m_long_option, m_has_arg, 0, m_option };

  char  m_arg_type;            //< Expected argument type (If a save_to variable is used, then this type will be used)
  char* m_description;         //< For showDescription() output

  // Storing the value in a user defined variable.
  char  m_save_typeid;         //< Remember the int type
  void* m_save_to;             //< Pointer to the variable
  void* m_default_value;       //< Store a default value (for m_has_arg=0)

  //< Pointing to a value in the union
  //char m_value_typeid;
  void* m_value;

public:

  /**
   * Format a usage string for an option
   */
  string getUsage()
  {
    stringstream ss;

    if (m_option > ' ' && m_option < '~')
    {
      ss << "  -" << char(m_option);
      if (m_long_option)
        ss << " [ --" << m_long_option << " ]";
    }
    else if (m_long_option)
    {
      ss << "  --" << m_long_option;
    }
    else
    {
      cout << endl;
    }

    if (m_has_arg)
    {
      if (m_has_arg == 1)
        ss << " arg ";
      else
        ss << " [ arg ] "; //< TODO: Default argument? "[=arg(=1)]" OR "[ arg (=1) ]"
    }

    size_t len = ss.str().length();
    if (len)
      ss << setw(40 - len) << " ";

    ss << m_description;
    return ss.str();
  }

  /**
   * Get the current value (casted to anything)
   */
  template<class T>
  T get()
  {
    if (m_value)
    {
      #ifndef ANDROID
      const char* type = typeid(T).name();
      if (type[0] == 'P')
        return static_cast<T>(m_value);
      else
      #endif
        return *static_cast<T*>(m_value);
    }
    return 0;
  }

#ifdef ANDROID
  /**
   * Set the current value (casted to anything)
   *
   * Saves the m_value and sets a variable if m_save_to is set.
   */
  void set(char* new_value)
  {
    switch(m_save_typeid)
    {
      case 'S':
        *static_cast<string*>(m_save_to) = new_value;
        break;

      case 'A':
        strcpy(static_cast<char*>(m_save_to), new_value);
        break;

      default:
        break;
    }
    m_value = new_value;
  }

  void set(char new_value)     { *static_cast<char*>(m_save_to)     = new_value; m_value = m_save_to;}
  void set(bool new_value)     { *static_cast<bool*>(m_save_to)     = new_value; m_value = m_save_to;}
  void set(int8_t new_value)   { *static_cast<int8_t*>(m_save_to)   = new_value; m_value = m_save_to;}
  void set(int16_t new_value)  { *static_cast<int16_t*>(m_save_to)  = new_value; m_value = m_save_to;}
  void set(int32_t new_value)  { *static_cast<int32_t*>(m_save_to)  = new_value; m_value = m_save_to;}
  void set(int64_t new_value)  { *static_cast<int64_t*>(m_save_to)  = new_value; m_value = m_save_to;}
  void set(uint8_t new_value)  { *static_cast<uint8_t*>(m_save_to)  = new_value; m_value = m_save_to;}
  void set(uint16_t new_value) { *static_cast<uint16_t*>(m_save_to) = new_value; m_value = m_save_to;}
  void set(uint32_t new_value) { *static_cast<uint32_t*>(m_save_to) = new_value; m_value = m_save_to;}
  void set(uint64_t new_value) { *static_cast<uint64_t*>(m_save_to) = new_value; m_value = m_save_to;}
#else
  template<class T>
  void set(T* new_value)
  {
    const char* type = typeid(new_value).name();

    //if (!m_save_typeid != type[1])
    //  throw_exception("Option --"+ string(m_long_option) +" need an argument!");

    //cout << "    Saving POINTER: " << type << " -> " << m_save_typeid << endl;
    switch(m_save_typeid)
    {
      case 'S':
        *static_cast<string*>(m_save_to) = new_value;
        break;

      case 'A':
        strcpy(static_cast<char*>(m_save_to), new_value);
        break;

      default:
        if (type[0] != 'P')
        {
          stringstream ss;
          ss << "Type '" << m_save_typeid << "' not supported (" << _FILE_ << ":" << __LINE__ << ")";
          throw_exception( ss.str() );
        }
    }
    m_value = new_value;
  }

  /**
   * Set the current value
   */
  template<class T>
  void set(T new_value)
  {
    m_value = &new_value;

    // The setup() was called and we need to assure we have the right type.
    if (m_save_typeid)
    {
      char type = typeid(new_value).name()[0];
      if (type != m_save_typeid)
        throw_exception(" Type "+ string(&type)
                       +" does not match "+ string(&m_save_typeid));

      //cout << "    DEFAULT argument: " << *static_cast<T*>(m_save_to) << endl;
      *static_cast<T*>(m_save_to) = *static_cast<T*>(m_value);
    }
  }
#endif


//#if 1
#ifdef ANDROID

  /**
   * Setting up Union pointer
   * m_has_arg is setup to:
   *   Required if there is no default_value set
   *   Optional if there is a default_value set and a type was defined
   *   None if there is a default_value set and a no type defined
   **/
  template<class T>
  void setup_union(T* union_ptr, T* value_ptr, T default_value = 125)
  {
    bool default_set = !(default_value==125); // Lets hope no one ever makes 125 a default
    m_save_to = value_ptr;
    *union_ptr = default_value;
    m_default_value = union_ptr;
    m_has_arg = (m_arg_type ? 1 : 0);
    if (default_set && m_has_arg)
      m_has_arg = 2;
  }

  void setup(string* value, string default_value = "")
  {
    m_save_typeid = 'S';
    m_save_to = value;
    m_has_arg = (default_value.empty()) ? 1 : ((m_arg_type) ? 2 : 0);
    m_default_value = malloc(default_value.size()+1);
    strcpy((char*)m_default_value, default_value.c_str());
  }

  void setup(char* value, char default_value)
  {
    m_save_typeid = 'A';
    setup_union(&default_c, value, default_value);
  }

  void setup(bool* value, bool default_value = 125) { m_save_typeid = 'a'; setup_union(&default_b,  value, default_value); }
  void setup(int8_t  *value, int8_t  default_value = 125) { m_save_typeid = 'a'; setup_union(&default_int8,  value, default_value); }
  void setup(int16_t *value, int16_t default_value = 125) { m_save_typeid = 's'; setup_union(&default_int16, value, default_value); }
  void setup(int32_t *value, int32_t default_value = 125) { m_save_typeid = 'i'; setup_union(&default_int32, value, default_value); }
  void setup(int64_t *value, int64_t default_value = 125) { m_save_typeid = 'x'; setup_union(&default_int64, value, default_value); }
  void setup(uint8_t  *value, uint8_t  default_value = 125) { m_save_typeid = 'h'; setup_union(&default_uint8,  value, default_value); }
  void setup(uint16_t *value, uint16_t default_value = 125) { m_save_typeid = 't'; setup_union(&default_uint16, value, default_value); }
  void setup(uint32_t *value, uint32_t default_value = 125) { m_save_typeid = 'j'; setup_union(&default_uint32, value, default_value); }
  void setup(uint64_t *value, uint64_t default_value = 125) { m_save_typeid = 'y'; setup_union(&default_uint64, value, default_value); }

#else

  /**
   * Setup a pointer to be saved when a value is set()
   */
  template<class T>
  void setup(T* variable)
  {
    //
    // char buffer[19] => 'PA19_c'
    // TODO: Record length to avoid buffer overflows!!!!
    //
    m_save_typeid   = typeid(variable).name()[1];
    m_save_to       = (void*)variable;
    m_has_arg       = 1; // required_argument
  }

  /**
   * Setup a pointer to be saved with a default value
   */
  template<class T>
  void setup(T* variable, T default_value)
  {
    setup(variable);

    //cout << "  -> Setup type '" << m_save_typeid << "' (" << default_value << ")" << endl;
    switch(m_save_typeid)
    {
      case 'a': default_int8   = default_value; m_default_value = &default_int8;     break;
      case 's': default_int16  = default_value; m_default_value = &default_int16;    break;
      case 'i': default_int32  = default_value; m_default_value = &default_int32;    break;
      case 'x': default_int64  = default_value; m_default_value = &default_int64;    break;
      case 'h': default_uint8  = default_value; m_default_value = &default_uint8;    break;
      case 't': default_uint16 = default_value; m_default_value = &default_uint16;   break;
      case 'j': default_uint32 = default_value; m_default_value = &default_uint32;   break;
      case 'y': default_uint64 = default_value; m_default_value = &default_uint64;   break;
      case 'c': default_c      = default_value; m_default_value = &default_c;        break;
      case 'b': default_b      = default_value; m_default_value = &default_b;        break;
      default:
      {
        stringstream ss;
        ss << "Type '" << m_save_typeid << "' not supported (" << _FILE_ << ":" << __LINE__ << ")";
        throw_exception( ss.str() );
        break;
      }
    }
    m_has_arg = 2;  // optional_argument
  }
#endif

  /**
   * Set the default value
   * If none is set it defaults to 1 for all types
   */
  void setDefault()
  {
    if (!m_default_value || !m_save_typeid || !m_save_to)
    {
      default_uint8 = 1;
      m_value = &default_uint8;
      return;
    }

    switch(m_save_typeid)
    {
      case 'a': *static_cast<int8_t*>(m_save_to)   = default_int8;     break;
      case 's': *static_cast<int16_t*>(m_save_to)  = default_int16;    break;
      case 'i': *static_cast<int32_t*>(m_save_to)  = default_int32;    break;
      case 'x': *static_cast<int64_t*>(m_save_to)  = default_int64;    break;
      case 'h': *static_cast<uint8_t*>(m_save_to)  = default_uint8;    break;
      case 't': *static_cast<uint16_t*>(m_save_to) = default_uint16;   break;
      case 'j': *static_cast<uint32_t*>(m_save_to) = default_uint32;   break;
      case 'y': *static_cast<uint64_t*>(m_save_to) = default_uint64;   break;
      case 'c': *static_cast<char*>(m_save_to)     = default_c;        break;
      case 'b': *static_cast<char*>(m_save_to)     = default_b;        break;
      default:
        throw_exception("Type '"+ Stringify(m_save_typeid) +"' not supported");
        break;
    }
    m_value = m_default_value;
  }

}; /* class PrgOptionType */


/**
 * The actual PrgOptions Class
 */
class PrgOptions
{
public:

  PrgOptions(uint32_t count = 1);
  virtual ~PrgOptions();

  /**
   * Add a header field (Top field)
   */
  inline void addHeader(string head) { m_head = head; };
  inline void addHeader(const char* head) { m_head = head; };

  /**
   * Add a usage field (Top field)
   */
  inline void addTail(string tail) { m_tail = tail; };
  inline void addTail(const char* tail) { m_tail = tail; };

  /**
   * Add a description field (Second field)
   */
  inline void addDescription(string description) { m_description = description; };
  inline void addDescription(const char* description) { m_description = description; };

  /**
   * Add a separator in-between fields
   */
  inline void addSeparator(string sep) { m_separator = sep; };
  inline void addSeparator(const char* sep) { m_separator = sep; };

  /**
   * Add some text in between other adds()
   */
  void add(string text) { add("", text.c_str()); };
  void add(const char* text) { add("", text); };

  /**
   * Add an option with a description.
   *
   * Option syntax: "enable,e" => (--enable OR -e)
   */
  void add(const char* option, const char* description);

  /**
   * Add an option with requiring an argument.
   *
   * Option syntax: "input,i=s" => (--input <string> OR -i <string>)
   */
  template<class T>
  void add(const char* option, T* save_to, const char* description)
  {
    add(option, description);
    options[m_count-1]->setup(save_to);
  }

  /**
   * Add an option with requiring an argument + a default value.
   */
  template<class T>
  void add(const char* option, T* save_to, T default_value, const char* description)
  {
    add(option, description);
    options[m_count-1]->setup(save_to, default_value);
  }

  /**
   * Parse the command line arguments.
   * Run after everything is added().
   */
  bool parse(int argc, char** argv);

  /**
   * Return the arguments not parsed
   */
  const char** getArguments();
  vector<const char*> getFiles();

  /**
   * Return true if an argument is set
   */
  bool is_set(const char *opt);

  /**
   * Return an argument as a type
   */
  template<typename T>
  T get(const char *opt)
  {
    PrgOptionType *obj = find(opt);
    if (!obj)
      throw_exception("Didn't find "+ string(opt), 0);

    return obj->get<T>();
  }

  /**
   * Directly get a pointer to the object with argument _opt_ (e.g. 'i')
   */
  PrgOptionType *find(int opt);

  /**
   * Directly get a pointer to the object with LONG argument (e.g. "input")
   */
  PrgOptionType *find(const char *opt);

  /**
   * Show the description added
   */
  void showUsage(const char* opt);
  void showDescription();

  PrgOptions(const PrgOptions& __copy);                  //< Copy constructor (Klocworks)
  PrgOptions& operator=(const PrgOptions& __copy); //< Class clone (Klocworks)

protected:
  PrgOptionType **options;
  vector<const char*> m_arguments;  //< Options with no arguments

  int m_long_only_count;

private:
  uint32_t m_count;
  uint32_t m_size;

  string m_head;
  string m_tail;
  string m_description;
  string m_separator;
};

#endif /* PRGOPTIONS_H_ */
