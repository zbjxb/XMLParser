#include "core.h"
#include <vector>

void IgnoreDecl();				// 忽略xml声明
void IgnoreValid();				// 忽略文档验证信息
void IgnoreComment();			// 忽略xml注释

int Look;						// 碗
FILE *File = NULL;				// 锅

struct Attrib_st {
	std::string attrib_name;
	std::string attrib_value;
};

typedef std::vector<Attrib_st> Attribs;
typedef std::vector<Node_st> Nodes;

struct Node_st {
	std::string elem_name;
	std::string elem_value;
	Attribs attribs;
	Nodes childs;
} XML;

Node_st* Parse(const char* file) {
	Init(file);
	GetElement(&XML);
	fclose(File);
	if (!XML.childs.empty()) {
		return &XML.childs[0];
	}
	else {
		return NULL;
	}
}

void Init(const char* file) {
	// 开锅
	File = fopen(file, "rb");
	if (File == NULL) {
		Expected("File open normally");
	}
	// 先捞一碗
	GetChar();
	// 过滤老鼠屎
	SkipWhite();
}

void GetElement(Node_st *parent) {
	std::string elem_name, elem_value;
	do {
		Node_st node;
		Match('<');
		while (Ignored()) {
			Match('<');
		}
		node.elem_name = GetElemName();
		while (Look != '/' && Look != '>') {
			// 解析属性
			GetAttrib(&node);
		}
		if (Look == '/') {
			Match('/');
			Match('>');
		}
		else if (Look == '>') {
			Match('>');
			if (Look != '<') {
				node.elem_value.append(GetElemValue());
			}
			fpos_t pos;
			fgetpos(File, &pos);
			Match('<');
			while (Ignored()) {
				if (Look != '<' && Look != EOF) {
					parent->elem_value.append(GetElemValue());
				}
				fgetpos(File, &pos);
				Match('<');
			}
			if (Look != '/') {
				// 子节点
				fsetpos(File, &pos);
				Look = '<';
				GetElement(&node);
				if (Look != '<') {
					parent->elem_value.append(GetElemValue());
				}
				Match('<');
				while (Ignored()) {
					if (Look != '<' && Look != EOF) {
						parent->elem_value.append(GetElemValue());
					}
					fgetpos(File, &pos);
					Match('<');
				}
			}

			Match('/');
			MatchString(node.elem_name.c_str());
			Match('>');
		}
		else {
			Expected("Closed Brackets needed");
		}
		parent->childs.push_back(node);

		if (Look != '<' && Look != EOF) {
			parent->elem_value.append(GetElemValue());
		}
		if (Look != EOF) {
			fpos_t pos;
			fgetpos(File, &pos);
			Match('<');
			while (Ignored()) {
				if (Look != '<' && Look != EOF) {
					parent->elem_value.append(GetElemValue());
				}
				if (Look != EOF) {
					fgetpos(File, &pos);
					Match('<');
				}
			}
			if (Look == EOF) {
				return;
			}
			if (Look != '/') {
				fsetpos(File, &pos);
				Look = '<';
			}
			else {
				fsetpos(File, &pos);
				Look = '<';
				break;
			}
		}

	} while (Look != EOF);
}

std::string GetElemName() {
	if (!isalpha(Look)) {
		Expected("Name");
	}
	char name[30] = {0};
	char *p = name;
	while (isalnum(Look) || (!isspace(Look) && (Look != '>'))) {
		*p = Look;
		p++;
		GetChar();
	}
	SkipWhite();
	return name;
}

std::string GetElemValue() {
	std::string elem_value;
	while (Look != '<') {
		elem_value.append(1, Look);
		GetChar();
	}
	return elem_value;
}

std::string GetAttribName() {
	if (!isalpha(Look)) {
		Expected("Name");
	}
	char name[30] = {0};
	char *p = name;
	while (isalnum(Look) || (!isspace(Look) && (Look != '='))) {
		*p = Look;
		p++;
		GetChar();
	}
	SkipWhite();
	return name;
}

std::string GetAttribValue() {
	std::string attrib_value;
	if (Look == '\'' || Look == '"') {
		char mode = Look;
		Match(mode);
		while (Look != mode) {
			attrib_value.append(1, Look);
			GetChar();
		}
		Match(mode);
	}
	else {
		Expected("Attribute Value");
	}
	return attrib_value;
}

void GetAttrib(Node_st *node) {
	Attrib_st attrib;
	attrib.attrib_name = GetAttribName(); 
	Match('=');
	attrib.attrib_value = GetAttribValue();
	node->attribs.push_back(attrib);
}

void GetChar() {
	Look = fgetc(File);
	//printf("%c",Look);
}

void Match(char ch) {
	if (Look == ch) {
		GetChar();
		SkipWhite();
	}
	else {
		char info[] = {"'''''''"};
		info[3] = ch;
		Expected(info);
	}
}

void MatchString(const char* str) {
	for (const char *p = str; *p != 0; p++) {
		Match(*p);
	}
}

void SkipWhite() {
	do {
		switch (Look) {
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				GetChar();
				continue;
			default:
				return;
		}
	} while (true);
}

void Abort(const char* s) {
	printf("\nError: %s.\n", s);

	if (File != NULL) {
		printf("Crash location: %d\n", ftell(File));
	}

	printf("Press ENTER to exit...");
	system("pause");
	exit(-1);
}

void Expected(const char* s) {
	char info[20] = {0};
	size_t nLen = strlen(s);
	strcpy(info, s);
	strcpy(&info[nLen], " Expected");
	Abort(info);
}

void IgnoreComment() {
	while (Look != '>') {
		GetChar();
	}
	Match('>');
}

void IgnoreValid() {
	while (Look != '>') {
		GetChar();
		if (Look == '[') {
			while (Look != ']') {
				GetChar();
			}
		}
	}
	Match('>');
}

void IgnoreDecl() {
	IgnoreComment();
}

bool Ignored() {
	if (Look == '?') {
		IgnoreDecl();
	}
	else if (Look == '!') {
		Match('!');
		if (Look == '-') {
			IgnoreComment();
		}
		else {
			IgnoreValid();
		}
	}
	else {
		return false;
	}
	return true;
}
