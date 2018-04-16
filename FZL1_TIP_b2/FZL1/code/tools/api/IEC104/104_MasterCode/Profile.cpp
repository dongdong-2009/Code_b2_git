#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "Profile.h"


#ifndef FILENAME_LEN
#define FILENAME_LEN 1024
#endif

b_bool Profile::read_profile()
{
  b_ui8 name_buf[PROFILE_NAME_LEN + 1];
  b_ui8 value_buf[PROFILE_VALUE_LEN + 1];
  b_ui16 name_pos = 0;
  b_ui16 value_pos = 0;
  b_ui8 temp_char;
  b_i32 status = READ_LINE_BEGIN;
  b_i32 pos;
  Section* temp_section;
  ProfileValue* temp_value;
  b_bool first = b_false;
#ifdef __OS_WIN32_
  while(_read(profile_fd, (pointer_type)&temp_char, 1) > 0) {
#else
  while(read(profile_fd, &temp_char, 1) > 0) {
#endif
    switch(status & 0x00ffffff) {
    case READ_LINE_BEGIN:
      if(temp_char == '\n')
	goto READ_NEXT;

      if((temp_char == '\r') ||
	 (temp_char == ' ')) {
	status &= ~READ_LINE_BEGIN;
	status |= READ_LINE_END;
	goto READ_NEXT;
      }

      if(temp_char == '#') {
	status &= ~READ_LINE_BEGIN;
	status |= READ_COMMENT;
	goto READ_NEXT;
      }

      if(temp_char == '[') {
	status = READ_SECTION_NAME;
	name_pos = 0;
	goto READ_NEXT;
      }

      if(isalpha(temp_char)||(temp_char == '_')) {
	if(status & READ_IN_SECTION) {
	  status &= ~READ_LINE_BEGIN;
	  status |= READ_VALUE_NAME;

	  name_pos = 1;
	  name_buf[0] = temp_char;
	  goto READ_NEXT;
	}
      }

      status = READ_ERROR;

      break;

    case READ_COMMENT:
      if(temp_char == '\n') {
	status &= ~READ_COMMENT;
	status |= READ_LINE_BEGIN;
      }

      break;

    case READ_SECTION_NAME:
      if(isalpha(temp_char) ||
	 (temp_char == '_')) {
	if(name_pos == PROFILE_NAME_LEN) {
	  status = READ_ERROR;
	  goto READ_NEXT;
	}

	name_buf[name_pos] = temp_char;
	name_pos++;
	goto READ_NEXT;
      }

      if(isdigit(temp_char)) {
	if(name_pos == 0) {
	  status = READ_ERROR;
	  goto READ_NEXT;
	}

	if(name_pos == PROFILE_NAME_LEN) {
	  status = READ_ERROR;
	  goto READ_NEXT;
	}

	name_buf[name_pos] = temp_char;
	name_pos++;
	goto READ_NEXT;
      }

      if(temp_char == ']') {
	status = READ_IN_SECTION|READ_LINE_END;
	name_buf[name_pos] = 0;

	pos = hash_key(name_buf);
	temp_section = section_table[pos];
	while(temp_section) {
	  if(!strncmp(temp_section->section_name, (char*)name_buf,
		      PROFILE_NAME_LEN)) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  temp_section = temp_section->next;
	}

	temp_section = new Section();
	if(!temp_section) {
	  status = READ_ERROR;
	  goto READ_NEXT;
	}

	strncpy(temp_section->section_name, (char*)name_buf,
		PROFILE_NAME_LEN);
	temp_section->next = section_table[pos];
	section_table[pos] = temp_section;

	break;
      }

      break;

    case READ_LINE_END:
      if((temp_char == ' ') ||
	 (temp_char == '\r'))
	goto READ_NEXT;

      if(temp_char == '\n') {
	status &= ~READ_LINE_END;
	status |= READ_LINE_BEGIN;
	goto READ_NEXT;
      }

      status = READ_ERROR;
      break;

    case READ_VALUE_NAME:
      if(isdigit(temp_char) ||
	 isalpha(temp_char) ||
	 temp_char == '_') {
	if(name_pos == PROFILE_NAME_LEN) {
	  status = READ_ERROR;
	  goto READ_NEXT;
	}

	name_buf[name_pos] = temp_char;
	name_pos++;
	goto READ_NEXT;
      }

      if(temp_char == '=') {
	status &= ~READ_VALUE_NAME;
	status |= READ_VALUE;
	value_pos = 0;
	first = b_false;

	name_buf[name_pos] = 0;
	pos = hash_key(name_buf);

	temp_value = temp_section->value_list[pos];
	while(temp_value) {
	  if(!strncmp(temp_value->value_name, (char*)name_buf,
		      PROFILE_NAME_LEN)) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  temp_value = temp_value->next;
	}

	temp_value = (ProfileValue*)bheap_alloc(sizeof(ProfileValue));
	if(!temp_value) {
	  status = READ_ERROR;
	  goto READ_NEXT;
	}
	temp_value->value = 0;

	strncpy(temp_value->value_name, (char*)name_buf,
		PROFILE_NAME_LEN);
	temp_value->next = temp_section->value_list[pos];
	temp_section->value_list[pos] = temp_value;
	goto READ_NEXT;
      }

      if(temp_char == ' ') {
	status &= ~READ_VALUE_NAME;
	status |= READ_EQUEL;


	name_buf[name_pos] = 0;
	pos = hash_key(name_buf);

	temp_value = temp_section->value_list[pos];
	while(temp_value) {
	  if(!strncmp(temp_value->value_name, (char*)name_buf,
		      PROFILE_NAME_LEN)) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  temp_value = temp_value->next;
	}

	temp_value = (ProfileValue*)bheap_alloc(sizeof(ProfileValue));
	if(!temp_value) {
	  status = READ_ERROR;
	  goto READ_NEXT;
	}
	temp_value->value = 0;

	strncpy(temp_value->value_name, (char*)name_buf,
		PROFILE_NAME_LEN);
	temp_value->next = temp_section->value_list[pos];
	temp_section->value_list[pos] = temp_value;
	goto READ_NEXT;
      }

      status = READ_ERROR;
      break;
    
    case READ_EQUEL:
      if(temp_char == ' ')
	goto READ_NEXT;

      if(temp_char == '=') {
	status &= ~READ_EQUEL;
	status |= READ_VALUE;
	value_pos = 0;
	first = b_false;

	goto READ_NEXT;
      }

      status = READ_ERROR;
      break;

    case READ_VALUE:
      if(temp_char == ' ') {
	if(!first) {
	  if(value_pos) {
	    status &= ~READ_VALUE;
	    status |= READ_LINE_END;

	    value_buf[value_pos] = 0;
	    value_pos++;

	    temp_value->value = (char*)bheap_alloc(value_pos);
	    if(!temp_value->value) {
	      status = READ_ERROR;
	      goto READ_NEXT;;
	    }

	    strncpy(temp_value->value, (char*)value_buf,
		    value_pos);
	    goto READ_NEXT;
	  } else
	    goto READ_NEXT;
	} else {
	  if(value_pos == PROFILE_VALUE_LEN) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  value_buf[value_pos] = temp_char;
	  value_pos++;
	  goto READ_NEXT;
	}
      }
      
      if(temp_char == '\r') {
	if(value_pos) {
	  if(first) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  status &= ~READ_VALUE;
	  status |= READ_LINE_END;

	  value_buf[value_pos] = 0;
	  value_pos++;

	  temp_value->value = (char*)bheap_alloc(value_pos);
	  if(!temp_value->value) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  strncpy(temp_value->value, (char*)value_buf,
		  value_pos);

	  goto READ_NEXT;
	} else {
	  if(first) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  status &= ~READ_VALUE;
	  status |= READ_LINE_END;

	  temp_value->value = (char*)bheap_alloc(1);
	  if(!temp_value->value) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  temp_value->value[0] = 0;

	  goto READ_NEXT;
	}
      }

      if(temp_char == '\n') {
	if(value_pos) {
	  if(first) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  status &= ~READ_VALUE;
	  status |= READ_LINE_BEGIN;

	  value_buf[value_pos] = 0;
	  value_pos++;

	  temp_value->value = (char*)bheap_alloc(value_pos);
	  if(!temp_value->value) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  strncpy(temp_value->value, (char*)value_buf,
		  value_pos);

	  goto READ_NEXT;
	} else {
	  if(first) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  status &= ~READ_VALUE;
	  status |= READ_LINE_BEGIN;

	  temp_value->value = (char*)bheap_alloc(1);
	  if(!temp_value->value) {
	    status = READ_ERROR;
	    goto READ_NEXT;
	  }

	  temp_value->value[0] = 0;

	  goto READ_NEXT;
	}
      }

      if(temp_char == '\"') {
	if(first) {
	  status &= ~READ_VALUE;
	  status |= READ_LINE_END;

	  if(value_pos) {
	    value_buf[value_pos] = 0;
	    value_pos++;

	    temp_value->value = (char*)bheap_alloc(value_pos);
	    if(!temp_value->value) {
	      status = READ_ERROR;
	      goto READ_NEXT;
	    }

	    strncpy(temp_value->value, (char*)value_buf,
		    value_pos);

	    goto READ_NEXT;
	  } else {
	    temp_value->value = (char*)bheap_alloc(1);
	    if(!temp_value->value) {
	      status = READ_ERROR;
	      goto READ_NEXT;
	    }

	    temp_value->value[0] = 0;
	    goto READ_NEXT;
	  }
	} else {
	  first = b_true;
	  goto READ_NEXT;
	}
      }

      value_buf[value_pos] = temp_char;
      value_pos++;

      break;
    }

  READ_NEXT:
    if(status == READ_ERROR) {
      clear_table();

      return b_false;
    }
  }

  return b_true;
}

void Profile::clear_table() {
  b_i32 i;
  Section* temp_section;

  for(i = 0; i < PROFILE_HASHTABLE_SIZE; i++) {
    temp_section = section_table[i];
    while(temp_section) {
      section_table[i] = temp_section->next;
      delete temp_section;
      temp_section = section_table[i];
    }

    section_table[i] = (Section*)0;
  }
}

b_i32 Profile::hash_key(b_ui8* name)
{
  b_ui8 result = 0;

  while(*name)
    result = (b_ui8)(result<<5) + result + *name++;

  return (b_i32)(result % PROFILE_HASHTABLE_SIZE);
}

char* Profile::get_value(char* section, char* name)
{
  b_i32 pos;
  Section* temp_section;
  ProfileValue* temp_value;

  pos = hash_key((b_ui8*)(section));
  temp_section = section_table[pos];

  while(temp_section) {
    if(!strncmp(temp_section->section_name, section,
		PROFILE_NAME_LEN)) {
      pos = hash_key((b_ui8*)(name));

      temp_value = temp_section->value_list[pos];
      while(temp_value) {
	if(!strncmp(temp_value->value_name, name,
		    PROFILE_NAME_LEN))
	  return temp_value->value;

	temp_value = temp_value->next;
      }

      return 0;
    }
    temp_section = temp_section->next;
  }

  return 0;
}

b_bool Profile::get_int_value(char* section, char* name, b_i32* value)
{
  char* raw_value = get_value(section, name);

  if(!raw_value)
    return b_false;

  *value = atoi(raw_value);

  return b_true;
}

b_bool Profile::get_int64_value(char* section, char* name, b_i64* value)
{
  char* raw_value = get_value(section, name);

  if(!raw_value)
    return b_false;

#ifdef __OS_WIN32_
  *value = _atoi64(raw_value);
#else
  *value = atoi64(raw_value);
#endif

  return b_true;
}

b_bool Profile::get_float_value(char* section, char* name, float* value)
{
  char* raw_value = get_value(section, name);

  if(!raw_value)
    return b_false;

  *value = (float)atof(raw_value);

  return b_true;
}

b_bool Profile::get_double_value(char* section, char* name, double* value)
{
  char* raw_value = get_value(section, name);

  if(!raw_value)
    return b_false;

  *value = atof(raw_value);

  return b_true;
}

Section* Profile::get_section_by_name(char* name)
{
  b_i32 pos = hash_key((b_ui8*)name);
  Section* temp_section;

  temp_section = section_table[pos];
  while(temp_section) {
    if(!strncmp(temp_section->section_name, name, PROFILE_NAME_LEN))
      break;

    temp_section = temp_section->next;
  }

  return temp_section;
}

ProfileValue* Profile::get_profilevalue_by_name(Section* section, char* name)
{
  b_i32 pos = hash_key((b_ui8*)name);
  ProfileValue* temp_profile_value = section->value_list[pos];

  while(temp_profile_value) {
    if(!strncmp(temp_profile_value->value_name, name, PROFILE_NAME_LEN))
      break;

    temp_profile_value = temp_profile_value->next;
  }

  return temp_profile_value;
}

b_bool Profile::add_value(char* section, char* name, char* value)
{
  Section* temp_section;
  b_bool new_section = b_false;
  ProfileValue* temp_profile_value;
  b_i32 pos;
  
  if(!section || !strlen(section))
    return b_false;

  if(!name || !strlen(name))
    return b_false;

  if(!value || !strlen(value))
    return b_false;

  temp_section = get_section_by_name(section);
  if(!temp_section) {
    temp_section = new Section;
    if(!temp_section)
      return b_false;

    strncpy(temp_section->section_name, section, PROFILE_NAME_LEN);
    new_section = b_true;
  }

  temp_profile_value = get_profilevalue_by_name(temp_section, name);
  if(!temp_profile_value) {
    temp_profile_value = (ProfileValue*)bheap_alloc(sizeof(ProfileValue));
    if(!temp_profile_value) {
      if(new_section)
	delete temp_section;

      return b_false;
    }
	temp_profile_value->value = 0;

    strncpy(temp_profile_value->value_name, name, PROFILE_NAME_LEN);
    temp_profile_value->value = (char*)bheap_alloc(strlen(value) + 1);
    if(!temp_profile_value->value) {
      bheap_free((pointer_type)temp_profile_value);
      if(new_section)
	delete temp_section;

      return b_false;
    }

    strncpy(temp_profile_value->value, value, strlen(value) + 1);

    pos = hash_key((b_ui8*)name);

    temp_profile_value->next = temp_section->value_list[pos];
    temp_section->value_list[pos] = temp_profile_value;

    if(new_section) {
      pos = hash_key((b_ui8*)section);

      temp_section->next = section_table[pos];
      section_table[pos] = temp_section;
    }

    return b_true;
  } else {
    if(new_section)
      delete temp_section;

    return b_false;
  }
}

b_bool Profile::add_int(char* section, char* name, b_i32 value)
{
  b_i8 str_value[1025];

  memset((pointer_type)str_value, 0, 1025);
#ifdef __OS_WIN32_
  _snprintf(str_value, 1024, "%d", value);
#else
  snprintf(str_value, 1024, "%d", value);
#endif

  return add_value(section, name, str_value);
}

b_bool Profile::add_double(char* section, char* name, double value)
{
  b_i8 str_value[1025];

  memset((pointer_type)str_value, 0, 1025);
#ifdef __OS_WIN32_
  _snprintf(str_value, 1025, "%f", value);
#else
  snprintf(str_value, 1025, "%f", value);
#endif

  return add_value(section, name, str_value);
}

b_bool Profile::write_profile(char* file_name)
{
  char temp_name[FILENAME_LEN + 1];
  FILE* profile;
  b_bool ret_value;

  if(!file_name)
    return b_false;

#ifdef __OS_WIN32_
  _snprintf(temp_name, FILENAME_LEN, "%s~", file_name);
#else
  snprintf(temp_name, FILENAME_LEN, "%s~", file_name);
#endif

  profile = fopen(temp_name, "w");
  if(!profile)
    return b_false;

  ret_value = write_profile(profile);
  fclose(profile);

  rename(temp_name, file_name);

  return ret_value;
}

b_bool Profile::write_profile(FILE* profile)
{
  Section* temp_section;
  time_t cur_time = time(0);

  fprintf(profile, "# Produced by Porfile program\n");
  fprintf(profile, "# Date: %s\n", ctime(&cur_time));

  for(int i = 0; i < PROFILE_HASHTABLE_SIZE; i++) {
    temp_section = section_table[i];

    while(temp_section) {
      if(!write_section(profile, temp_section))
	return b_false;
      temp_section = temp_section->next;
    }
  }

  return b_true;
}

b_bool Profile::write_section(FILE* profile, Section* section)
{
  ProfileValue* temp_profile_value;

  fprintf(profile, "#Section: %s\n", section->section_name);
  fprintf(profile, "[%s]\n", section->section_name);

  for(int i = 0; i < PROFILE_HASHTABLE_SIZE; i++) {
    temp_profile_value = section->value_list[i];

    while(temp_profile_value) {
      fprintf(profile, "%s = %s\n", temp_profile_value->value_name,
	      temp_profile_value->value);
      temp_profile_value = temp_profile_value->next;
    }
  }

  fprintf(profile, "\n");

  return b_true;
}
