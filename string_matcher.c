#include <stdbool.h>
#include <string.h>
#include <ctype.h>

bool __has_substring(const char* haystack, const char* needle) {
  size_t haystack_length = strlen(haystack);
  size_t needle_length   = strlen(needle);

  if(needle_length == 0) return true;
  if(haystack_length < needle_length) return false;

  size_t haystack_index = 0;
  size_t needle_index   = 0;

  while(haystack_index < haystack_length) {
    if(tolower(haystack[haystack_index]) == tolower(needle[needle_index])) {
      needle_index++;

      if(needle_index < needle_length) haystack_index++;
      else return true;
    } else {
      needle_index = 0;
      haystack_index++;
    }
  }

  return false;
}