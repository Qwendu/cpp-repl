#include <stdio.h>
#include <stdlib.h>
#include "../common/String.hpp"

String includes = makeStringFromCString("#include <stdio.h>\r\n#include <math.h>\r\n#include \"common/String.hpp\"");
String main_func = makeStringFromCString("int main(int argc, char **argv)\r\n{");
String footer    = makeStringFromCString("return 0;\r\n}");

String compiler       = makeStringFromCString("clang++");
String compiler_flags = makeStringFromCString("-g");


struct Program
{
	String includes = nullptr;
	String text     = nullptr;
};

int main(int argc, char **argv)
{
	String buffer = makeStringWithLength(1000);

	String program = nullptr;
	String previous_output = nullptr;
	int read = 0;

	while(fgets(buffer->data, sizeof(char) * buffer->count, stdin))
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
		String new_line = makeStringFromCString(buffer->data);
		fprintf(repl_file, 
			"//Includes\r\n"           F_STR "\r\n"
			"//main \r\n"              F_STR "\r\n"
			"//Start of user code\r\n" F_STR "\r\n"
			"//New line\r\n"           F_STR "\r\n"
			"//End of user code\r\n"   F_STR "\r\n"
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
			free(new_line);
			continue;
		}
		FILE *pr = popen("./repl" , "r");
		ret_code = system("./repl");
		if(ret_code != 0)
		{
			printf("Error with the last line in execution\n");
			free(new_line);
			continue;
		}
		pclose(pr);
		program = append(program, new_line);

	}
	return 0;
}
