#ifndef STRING
#define STRING

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define STRINGIFY(arg) # arg

#define F_STR "%.*s"
#define F_STR_ARG(str) ((str) ? (int)(str)->count : 0), ((str) ? (str)->data : nullptr)


struct String_T
{
	size_t count;
	char   data[];
};

typedef String_T* String;

String inline
makeStringFromCString(const char *c_string)
{
	if(!c_string) return nullptr;

	size_t count = strlen(c_string);
	String temp = (String)malloc(sizeof(String_T) + count * sizeof(char));
	if(!temp) return nullptr;

	temp->count = count;
	memcpy(&temp->data, c_string, count * sizeof(char));

	return temp;
}

String inline
makeStringFromCStringNullTerminated(const char *c_string)
{
	if(!c_string) return nullptr;

	size_t count = strlen(c_string) + 1;
	String temp = (String)malloc(sizeof(String_T) + count * sizeof(char));
	if(!temp) return nullptr;

	temp->count = count;
	memcpy(&temp->data, c_string, count * sizeof(char));

	return temp;
}

String inline
makeStringWithLength(size_t string_length)
{
	String t = (String)malloc(sizeof(String) + sizeof(char) * string_length);
	if(!t) return nullptr;
	t->count = string_length;
	return t;
}

String inline
makeStringCopy(String string)
{
	if(!string) return nullptr;

	String temp = (String)malloc(sizeof(String_T) + string->count * sizeof(char));
	if(!temp) return nullptr;

	temp->count = string->count;
	memcpy(temp->data, string->data, string->count * sizeof(char));
	return temp;
}

String inline
makeStringCopyNullTerminated(String string)
{
	if(!string) return nullptr;
	if(string->data[string->count - 1] == 0) return makeStringCopy(string);

	String temp = (String)malloc(sizeof(String_T) + (string->count + 1) * sizeof(char));
	if(!temp) return nullptr;

	temp->count = string->count + 1;
	memcpy(temp->data, string->data, string->count * sizeof(char));
	temp->data[temp->count - 1] = 0;
	return temp;
}

String inline
makeSubstring(String parent, size_t start, int length = -1)
{
	if(!parent) return nullptr;
	if(length == 0 || start < 0 || start >= parent->count) return nullptr;
	if(length == -1)
	{
		length = (int)(parent->count - (size_t)start);
	}
	if(length <= 0) return nullptr;
	String s = makeStringWithLength((size_t)length);
	memcpy(s->data, parent->data + start, sizeof(char) * (size_t)(length));
	return s;
}

bool 
areEqual(String a, String b)
{
	// Remeber Strings are pointers, so if we pass in the same Pointer twice we know it is the same string.
	if(a == b) return true;
	if(a == nullptr || b == nullptr) return false;
	if(a->count != b->count) return false;
	for(size_t i = 0; i < a->count; i++)
	{
		if(a->data[i] != b->data[i]) return false;
	}
	return true;
}



// 
// Using Macros is meh and error prone. But this is c++ and that is how we can do this the cheapest.
// Because we are using alloca here this might not be okay in embedded systems which might be short on stack space. 
// ~Qwendo 10.8.2022
// 
#define MACRO_LocalMakeStringFromCString(ident, c_string) \
	do{(ident) = nullptr;\
	if((c_string))\
	{\
		int count = strlen((c_string));\
		(ident) = (String)alloca(sizeof(String_T) + count * sizeof(char));\
		if((ident))\
		{\
			(ident)->count = count;                                  \
			memcpy(&(ident)->data, (c_string), count * sizeof(char));\
		}\
	}while(0)


#define MACRO_LocalMakeStringFromCStringNullTerminated(ident, c_string) \
	do{\
	(ident) = nullptr;\
	if((c_string))\
	{\
		int count = strlen((c_string));\
		(ident) = (String)alloca(sizeof(String_T) + count * sizeof(char));\
		if((ident))\
		{\
			if(c_string[count - 1] != 0) (ident)->count += 1;         \
			(ident)->count = count;\
			if(c_string[count - 1] != 0) (ident)->data[count] = 0;    \
			memcpy(&(ident)->data, (c_string), count * sizeof(char));\
		}\
	}\
	}while(0)
#define MACRO_LocalMakeStringCopyNullTerminated(cpy, other)\
	do{(cpy) = nullptr;\
	cpy = (String)alloca(sizeof(String_T) + (other->count + 1) * sizeof(char));\
	if(cpy)\
	{\
		cpy->count = other->count + 1;\
		memcpy(cpy->data, other->data, other->count * sizeof(char));\
		cpy->data[cpy->count - 1] = 0;\
	}\
	}while(0)
//#define MACRO_Literal(lit) {sizeof(lit) - 1, (lit)}

int
countCharacterAppearances(String str, char c)
{
	int n = 0;
	for(size_t i = 0; i < str->count; i++)
	{
		if(str->data[i] == c) n += 1;
	}
	return n;
}

String append(String first, String second)
{
	size_t new_length = 0;
	size_t first_length = 0;
	if(first)
	{
		new_length += first->count;
		first_length += first->count;
	}
	if(second)
	{
		new_length += second->count;
	}

	String result = makeStringWithLength(new_length);
	if(!result) return nullptr;
	if(first) memcpy(result->data, first->data, first->count);
	if(second) memcpy(result->data + first_length, second->data, second->count);

	return result;

}

size_t indexOfFirstDifference(String a, String b)
{
	if(!a || !b) return 0;
	int idx = 0;
	while(a->data[idx] == b->data[idx])
	{
		idx += 1;
	}
	return idx;
}

#endif //STRING
