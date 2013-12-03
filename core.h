#ifndef CORE_H__
#define CORE_H__

#include <stdio.h>
#include <ctype.h>
#include <string>

struct Node_st;

void GetChar();									// 从锅里捞一碗
void SkipWhite();								// 过滤老鼠屎
void Abort(const char*);						// 出错停止
void Expected(const char*);						// 提示信息
void Match(char);								// 期待某个字符，不符合期待就失败
void MatchString(const char*);					// 期待指定字符串
std::string GetElemName();						// 获取元素名
std::string GetElemValue();						// 获取元素值
void GetElement(Node_st *parent);				// 获取元素
std::string GetAttribName();					// 获取属性名
std::string GetAttribValue();					// 获取属性值
void GetAttrib(Node_st *node);					// 获取属性
bool Ignored();									// 是否有忽略不常用的xml信息，比如xml声明、文档验证信息、注释
void Init(const char* file);					// 解析前初始化工作
Node_st* Parse(const char* file); 				// 解析并返回整个XML树

#endif // CORE_H__