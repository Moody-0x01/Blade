#include <blade.h>

char *file_type_as_str(FileType s) {
	switch (s) {
		case PYTHON:
			return "PYTHON";
		case C:
			return "C";
		case JS:
			return "JAVASCRIPT";
		case MP3:
			return "MP3";
		default:
			return "UNSUPPORTED";
	}
	return "UNSUPPORTED";
}

KeywordList *get_keywords_list(FileType s) {
	static KeywordList list[UNSUP] = {
		[PYTHON] =
		{
			.size = 25,
			._list = {"enumerate", "return", "callable", "str",   "None",
				"int",       "def",    "for",      "if",    "self",
				"while",     "else",   "elif",     "True",  "False",
				"match",     "class",  "str",      "in",    "while",
				"import",    "from",   "lambda",   "break", "continue",
				"yield"},
		},
		[C] =
		{
			.size = 15,
			._list = {"for", "while", "else", "if", "true", "false", "switch",
				"long", "short", "return", "break", "continue", "goto",
				"static", "extern"},
		},

		[JS] =
		{
			.size = 57,
			._list = {"abstract",  "arguments", "await",      "boolean",
				"case",      "catch",     "break",      "byte",
				"char",      "class",     "const",      "continue",
				"do",        "debugger",  "default",    "do",
				"delete",    "else",      "enum",       "eval",
				"double",    "false",     "finally",    "export",
				"extends",   "float",     "for",        "function",
				"goto",      "if",        "implements", "import",
				"number",       "long",      "private",    "interface",
				"let",       "new",       "package",    "instanceof",
				"native",    "static",    "short",      "return",
				"in",        "null",      "throw",      "throws",
				"protected", "typeof",    "with",       "try",
				"public",    "void",      "true",       "yield",
				"super",     "var"},
		},
		[GO] =
		{
			.size = 27,
			._list = {"break",       "case",    "chan",   "const",
				"continue",    "default", "defer",  "else",
				"fallthrough", "for",     "func",   "go",
				"goto",        "if",      "import", "interface",
				"map",         "package", "range",  "return",
				"select",      "struct",  "switch", "type",
				"var",         "true",    "false"},
		},

	};

	if (s < UNSUP)
		return (list + s);
	return NULL;
}

FileType get_file_type(char *spath) {
	int it = (strlen(spath) - 1);
	char extension[32] = {0};

	while (it >= 0 && *(spath + it)) {
		if (spath[it] == '.') {
			strcpy(extension, (spath + it));
			break;
		}

		it--;
	}

	if (*extension == 0)
		return UNSUP;

	if (!strcmp(extension, ".py"))
		return PYTHON;
	if (!strcmp(extension, ".js"))
		return JS;
	if (!strcmp(extension, ".go"))
		return GO;
	if (!strcmp(extension, ".c") || !strcmp(extension, ".h"))
		return C;
	if (!strcmp(extension, ".mp3"))
		return MP3;
	return UNSUP;
}
