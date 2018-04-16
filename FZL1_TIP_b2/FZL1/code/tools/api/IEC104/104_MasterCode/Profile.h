/******************************************************************************
                                  Profile.h
                               ---------------

******************************************************************************/

#ifndef __PROFILE_H
#define __PROFILE_H

#include "Btype.h"

#ifdef __OS_WIN32_
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <sys/types.h>
#include <fcntl.h>


#include <stdlib.h>

#include "Object.h"
#include "Bexception.h"

#define PROFILE_NAME_LEN 32
#define PROFILE_VALUE_LEN 1024
#define PROFILE_HASHTABLE_SIZE 17

#define READ_LINE_BEGIN    0x00000001
#define READ_SECTION_BEGIN 0x00000002
#define READ_SECTION_NAME  0x00000004
#define READ_VALUE_NAME    0x00000008
#define READ_VALUE         0x00000010
#define READ_COMMENT       0x00000020
#define READ_LINE_END      0x00000040
#define READ_EQUEL         0x00000080
#define READ_ERROR         0x00000100
#define READ_IN_SECTION    0x80000000

#define PROFILE_DEFAULT    0
#define PROFILE_CREATE     1

class ProfileValue
{
 public:
  ProfileValue() {
    value = 0;
  }

  char value_name[PROFILE_NAME_LEN + 1];
  char* value;
  ProfileValue* next;
};

class Section
{
 public:
  Section() {
    b_i32 i;

    for(i = 0; i < PROFILE_HASHTABLE_SIZE; i++)
      value_list[i] = 0;
  }

  ~Section() {
    ProfileValue* temp_value;
    b_i32 i;

    for( i = 0; i < PROFILE_HASHTABLE_SIZE; i++) {
      temp_value = value_list[i];
      while(temp_value) {
	value_list[i] = temp_value->next;
	bheap_free((pointer_type)(temp_value->value));
	bheap_free((pointer_type)temp_value);
	temp_value = value_list[i];
      }
    }
  }

  char section_name[PROFILE_NAME_LEN + 1];
  ProfileValue* value_list[PROFILE_HASHTABLE_SIZE];
  Section* next;
};

class Profile : public Object
{
 public:
  Profile(char* name, b_i32 option) {
    b_i32 i;

    if(option == PROFILE_DEFAULT) {
      for(i = 0; i < PROFILE_HASHTABLE_SIZE; i++)
	section_table[i] = 0;

#ifdef __OS_WIN32_
      profile_fd = _open(name, _O_RDONLY);
#else
      profile_fd = open(name, O_RDONLY);
#endif

      if(profile_fd < 0)
	throw ProfileException();

      if(!read_profile()) {
#ifdef __OS_WIN32_
	_close(profile_fd);
#else
	close(profile_fd);
#endif

	throw ProfileException();
      }
#ifdef __OS_WIN32_
      _close(profile_fd);
#else
      close(profile_fd);
#endif

    } else if(option == PROFILE_CREATE) {
      b_i32 i;

      for(i = 0; i < PROFILE_HASHTABLE_SIZE; i++)
	section_table[i] = 0;
    }
  }

  Profile(char* name) {
    b_i32 i;

    for(i = 0; i < PROFILE_HASHTABLE_SIZE; i++)
      section_table[i] = 0;

#ifdef __OS_WIN32_
    profile_fd = _open(name, _O_RDONLY);
#else
    profile_fd = open(name, O_RDONLY);
#endif

    if(profile_fd < 0) {
      throw ProfileException();
    }

    if(!read_profile()) {
#ifdef __OS_WIN32_
      _close(profile_fd);
#else
      close(profile_fd);
#endif
      throw ProfileException();
    }
#ifdef __OS_WIN32_
    _close(profile_fd);
#else
    close(profile_fd);
#endif
  }

  virtual ~Profile() {
    clear_table();
  }

  b_bool read_profile();

  char* get_value(char* section, char* name);
  b_bool get_int_value(char* section, char* name, b_i32* value);
  b_bool get_int64_value(char* section, char* name, b_i64* value);
  b_bool get_float_value(char* section, char* name, float* value);
  b_bool get_double_value(char* section, char* name, double* value);

  b_bool add_value(char* section, char* name, char* value);
  b_bool add_int(char* section, char* name, b_i32 value);
  b_bool add_double(char* section, char* name, double value);

  b_bool write_profile(char* file_name);
  b_bool write_profile(FILE* profile);
  b_bool write_section(FILE* profile, Section* section);

 protected:
  b_i32 profile_fd;
  Section* section_table[PROFILE_HASHTABLE_SIZE];

  void clear_table();
  b_i32 hash_key(b_ui8* name);
  Section* get_section_by_name(char* name);
  ProfileValue* get_profilevalue_by_name(Section* section, char* name);
};

#endif
