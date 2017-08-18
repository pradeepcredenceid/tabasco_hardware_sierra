/*  -------------------------------------------------------------------------
    Copyright (C) 2013 Intel Mobile Communications GmbH

         Sec Class: Intel Confidential (IC)
    ----------------------------------------------------------------------
    Revision Information:
       $File name:  /msw_tools/FlashTool/libFlsTool/src/JSON.cpp $
       $CC-Version: .../oint_tools_flashtoole2_01/7 $
       $Date:       2013-12-11    23:17:53 UTC $
    ----------------------------------------------------------------------
    Description:
       A simple JSON parser
    ---------------------------------------------------------------------- */

/* This code is adapted from free code written by Ivan Vashchaev:

  Copyright (C) 2010
  Ivan Vashchaev <vivkin@gmail.com>

  The MIT License (MIT)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "JSON.h"

#include <string.h>
#include <memory.h>

// true if character represent a digit
#define IS_DIGIT(c) (c >= '0' && c <= '9')

// convert string to integer
char *atoi(char *first, char *last, int *out)
{
  int sign = 1;
  if (first != last)
  {
    if (*first == '-')
    {
      sign = -1;
      ++first;
    }
    else if (*first == '+')
    {
      ++first;
    }
  }

  int result = 0;
  for (; first != last && IS_DIGIT(*first); ++first)
  {
    result = 10 * result + (*first - '0');
  }
  *out = result * sign;

  return first;
}

// convert hexadecimal string to unsigned integer
char *hatoui(char *first, char *last, unsigned int *out)
{
  unsigned int result = 0;
  for (; first != last; ++first)
  {
    int digit;
    if (IS_DIGIT(*first))
    {
      digit = *first - '0';
    }
    else if (*first >= 'a' && *first <= 'f')
    {
      digit = *first - 'a' + 10;
    }
    else if (*first >= 'A' && *first <= 'F')
    {
      digit = *first - 'A' + 10;
    }
    else
    {
      break;
    }
    result = 16 * result + digit;
  }
  *out = result;

  return first;
}

// convert string to floating point
char *atof(char *first, char *last, float *out)
{
  // sign
  float sign = 1;
  if (first != last)
  {
    if (*first == '-')
    {
      sign = -1;
      ++first;
    }
    else if (*first == '+')
    {
      ++first;
    }
  }

  // integer part
  float result = 0;
  for (; first != last && IS_DIGIT(*first); ++first)
  {
    result = 10 * result + (*first - '0');
  }

  // fraction part
  if (first != last && *first == '.')
  {
    ++first;

    float inv_base = 0.1f;
    for (; first != last && IS_DIGIT(*first); ++first)
    {
      result += (*first - '0') * inv_base;
      inv_base *= 0.1f;
    }
  }

  // result w\o exponent
  result *= sign;

  // exponent
  bool exponent_negative = false;
  int exponent = 0;
  if (first != last && (*first == 'e' || *first == 'E'))
  {
    ++first;

    if (*first == '-')
    {
      exponent_negative = true;
      ++first;
    }
    else if (*first == '+')
    {
      ++first;
    }

    for (; first != last && IS_DIGIT(*first); ++first)
    {
      exponent = 10 * exponent + (*first - '0');
    }
  }

  if (exponent)
  {
    float power_of_ten = 10;
    for (; exponent > 1; exponent--)
    {
      power_of_ten *= 10;
    }

    if (exponent_negative)
    {
      result /= power_of_ten;
    }
    else
    {
      result *= power_of_ten;
    }
  }

  *out = result;

  return first;
}

json_value *json_alloc()
{
    json_value *value = new json_value;
    #if 0
      json_value *value = (json_value *)malloc(sizeof(json_value));
      if (!value)
      {
          fprintf(stderr, "Failed to allocate json_value\n");
          exit(1);
      }
    #endif
    memset(value, 0, sizeof(json_value));
    return value;
}

void json_free(json_value* value)
{
  if (value)
    delete value;
}

void json_destroy(json_value* value)
{
  if (!value)
    return;

  switch(value->type)
  {
  case JSON_OBJECT:
  case JSON_ARRAY:
  {
    json_value *next = value->first_child;

    while (next)
    {
      json_value* node = next;
      next = next->next_sibling;
      json_destroy(node);
    }

    json_free(value);
    break;
  }
  case JSON_STRING:
  case JSON_INT:
  case JSON_FLOAT:
  case JSON_BOOL:
  case JSON_NULL:
    json_free(value);
    break;

  default:
    // throw_exception("Type "+ Stringify(value->type) +" not recognized!");
    break;
  }

}

void json_append(json_value *lhs, json_value *rhs)
{
  rhs->parent = lhs;
  if (lhs->last_child)
  {
    lhs->last_child = lhs->last_child->next_sibling = rhs;
  }
  else
  {
    lhs->first_child = lhs->last_child = rhs;
  }
}

#define ERROR(it, desc)\
  *error_pos = it;\
  *error_desc = (char *)desc;\
  *error_line = 1 - escaped_newlines;\
  for (char *c = it; c != source; --c)\
    if (*c == '\n') ++*error_line;\
  return 0

#define CHECK_TOP() if (!top) { json_free(root); ERROR(it, "Unexpected character");}

json_value *json_parse(char *source, char **error_pos, char **error_desc, int *error_line)
{
  json_value *root = 0;
  json_value *top = 0;

    char *name = 0;
    char *it = source;

  int escaped_newlines = 0;

  while (*it)
  {
    switch (*it)
    {
    case '{':
    case '[':
      {
        // create new value
        json_value *object = json_alloc();

        // name
        object->name = name;
        name = 0;

        // type
        object->type = (*it == '{') ? JSON_OBJECT : JSON_ARRAY;

        // skip open character
        ++it;

        // set top and root
        if (top)
        {
          json_append(top, object);
        }
        else if (!root)
        {
          root = object;
        }
        else
        {
          json_free(object);
          ERROR(it, "Second root. Only one root allowed");
        }
        top = object;
      }
      break;

    case '}':
    case ']':
      {
        if (!top || top->type != ((*it == '}') ? JSON_OBJECT : JSON_ARRAY))
        {
          json_free(top);
          ERROR(it, "Mismatch closing brace/bracket");
        }

        // skip close character
        ++it;

        // set top
        top = top->parent;
      }
      break;

    case ':':
      if (!top || top->type != JSON_OBJECT)
      {
        json_free(top);
        ERROR(it, "Unexpected character");
      }
      ++it;
      break;

    case ',':
      CHECK_TOP();
      ++it;
      break;

    case '"':
      {
        CHECK_TOP();

        // skip '"' character
        ++it;

        char *first = it;
        char *last = it;
        while (*it)
        {
          if ((unsigned char)*it < 0x20)
          {
            json_free(top);
            ERROR(first, "Control characters not allowed in strings");
          }
          else if (*it == '\\')
          {
            switch (it[1])
            {
            case '"':
              *last = '"';
              break;
            case '\\':
              *last = '\\';
              break;
            case '/':
              *last = '/';
              break;
            case 'b':
              *last = '\b';
              break;
            case 'f':
              *last = '\f';
              break;
            case 'n':
              *last = '\n';
              ++escaped_newlines;
              break;
            case 'r':
              *last = '\r';
              break;
            case 't':
              *last = '\t';
              break;
            case 'u':
              {
                unsigned int codepoint;
                if (hatoui(it + 2, it + 6, &codepoint) != it + 6)
                {
                  json_free(top);
                  ERROR(it, "Bad unicode codepoint");
                }

                if (codepoint <= 0x7F)
                {
                  *last = (char)codepoint;
                }
                else if (codepoint <= 0x7FF)
                {
                  *last++ = (char)(0xC0 | (codepoint >> 6));
                  *last = (char)(0x80 | (codepoint & 0x3F));
                }
                else if (codepoint <= 0xFFFF)
                {
                  *last++ = (char)(0xE0 | (codepoint >> 12));
                  *last++ = (char)(0x80 | ((codepoint >> 6) & 0x3F));
                  *last = (char)(0x80 | (codepoint & 0x3F));
                }
              }
              it += 4;
              break;
            default:
              json_free(top);
              ERROR(first, "Unrecognized escape sequence");
            }

            ++last;
            it += 2;
          }
          else if (*it == '"')
          {
            *last = 0;
            ++it;
            break;
          }
          else
          {
            *last++ = *it++;
          }
        }

        if (!name && top->type == JSON_OBJECT)
        {
          // field name in object
          name = first;
        }
        else
        {
          // new string value
          json_value *object = json_alloc();

          object->name = name;
          name = 0;

          object->type = JSON_STRING;
          object->string_value = first;

          json_append(top, object);
        }
      }
      break;

    case 'n':
    case 't':
    case 'f':
      {
        CHECK_TOP();

        // new null/bool value
        json_value *object = json_alloc();

        object->name = name;
        name = 0;

        // null
        if (it[0] == 'n' && it[1] == 'u' && it[2] == 'l' && it[3] == 'l')
        {
          object->type = JSON_NULL;
          it += 4;
        }
        // true
        else if (it[0] == 't' && it[1] == 'r' && it[2] == 'u' && it[3] == 'e')
        {
          object->type = JSON_BOOL;
          object->int_value = 1;
          it += 4;
        }
        // false
        else if (it[0] == 'f' && it[1] == 'a' && it[2] == 'l' && it[3] == 's' && it[4] == 'e')
        {
          object->type = JSON_BOOL;
          object->int_value = 0;
          it += 5;
        }
        else
        {
          json_free(object);
          json_free(top);
          ERROR(it, "Unknown identifier");
        }

        json_append(top, object);
      }
      break;

    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      {
        CHECK_TOP();

        // new number value
        json_value *object = json_alloc();

        object->name = name;
        name = 0;

        object->type = JSON_INT;

        char *first = it;
        while (*it != '\x20' && *it != '\x9' && *it != '\xD' && *it != '\xA' && *it != ',' && *it != ']' && *it != '}')
        {
          if (*it == '.' || *it == 'e' || *it == 'E')
          {
            object->type = JSON_FLOAT;
          }
          ++it;
        }

        if (object->type == JSON_INT && atoi(first, it, &object->int_value) != it)
        {
          json_free(object);
          json_free(top);
          ERROR(first, "Bad integer number");
        }

        if (object->type == JSON_FLOAT && atof(first, it, &object->float_value) != it)
        {
          json_free(object);
          json_free(top);
          ERROR(first, "Bad float number");
        }

        json_append(top, object);
      }
      break;

    default:
      json_free(top);
      ERROR(it, "Unexpected character");
    }

    // skip white space
    while (*it == '\x20' || *it == '\x9' || *it == '\xD' || *it == '\xA')
    {
      ++it;
    }
  }

  if (top)
  {
    json_free(top);
    ERROR(it, "Not all objects/arrays have been properly closed");
  }

  return root;
}
