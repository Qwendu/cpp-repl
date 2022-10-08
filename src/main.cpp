#include <stdio.h>
#include <stdlib.h>
#include "../common/String.hpp"


String
readFileIntoString(String file_name)
{
	String f_z;
	MACRO_LocalMakeStringCopyNullTerminated(f_z, file_name);
	FILE *f = fopen(f_z->data, "rb");
	fseek(f, 0, SEEK_END);
	size_t len = ftell(f);
	fseek(f, 0, SEEK_SET);

	String ret = makeStringWithLength(len + 1);

	size_t read = fread(ret->data, sizeof(char), ret->count, f);
	if(read != ret->count - 1) abort();
	ret->data[ret->count - 1] = 0;
	ret->count = ret->count - 1;
	fclose(f);
	return ret;
}	

String includes = makeStringFromCString("#include <stdio.h>\n#include <math.h>\n#include \"common/String.hpp\"");
String main_func = makeStringFromCString("int main(int argc, char **argv)\n{\n\tfclose(stdout); fclose(stderr);stdout = fopen(\"output\", \"w+\"); stderr = stdout;");
String footer    = makeStringFromCString("\tfclose(stdout);\nreturn 0;\n}");

String compiler       = makeStringFromCString("clang++");
String compiler_flags = makeStringFromCString("-g");
String output_file    = makeStringFromCStringNullTerminated("output");

int main(int argc, char **argv)
{
	String buffer = makeStringWithLength(1000);

	String program = nullptr;
	String previous_output = nullptr;
	int read = 0;

	while(printf(">"), fgets(buffer->data, sizeof(char) * buffer->count, stdin))
	{
		if(buffer->data[0] == ':')
		{
			if(buffer->data[1] == 'q') break;
			switch(buffer->data[1])
			{
			case 'i':
			{
				continue;
			}break;
			default:
				printf("Unrecognized command '%.*s'\n", (int)buffer->count - 1, buffer->data + 1);
				continue;
			}
		}
		// print program to temporary file
		FILE *repl_file = fopen("repl.cpp", "w+");
		if(!repl_file)
		{
			printf("could not open repl.cpp");
			break;
		}
		String new_line;
		MACRO_LocalMakeStringFromCStringNullTerminated(new_line, buffer->data);
		fprintf(repl_file, 
			"//Includes\n"           F_STR "\n"
			"//main \n"              F_STR "\n"
			"//Start of user code\n" F_STR "\n"
			"//New line\n"           F_STR "\n"
			"//End of user code\n"   F_STR "\n"
			, F_STR_ARG(includes)
			, F_STR_ARG(main_func)
			, F_STR_ARG(program)
			, F_STR_ARG(new_line)
			, F_STR_ARG(footer));
		fclose(repl_file);

		int ret_code = system("clang++ repl.cpp -o repl");
		if(ret_code != 0)
		{
			printf("Error with the last line\n");
			continue;
		}

		ret_code = system("./repl");
		if(ret_code != 0)
		{
			printf("Error with the last line in execution\n");
			continue;
		}

		String output   = readFileIntoString(output_file);
		String new_part = makeSubstring(output, indexOfFirstDifference(output, previous_output));

		free(previous_output);
		previous_output = output;

		if(new_part && new_part->count != 0) printf("<" F_STR "\n", F_STR_ARG(new_part));

		free(new_part);


		String new_program = append(program, new_line);
		free(program);
		program = new_program;

	}
	
	free(program);
	if(previous_output) free(previous_output);

	system("rm repl");
	system("rm repl.cpp");
	system("rm output");
	return 0;
}
