#pragma once
#include <iostream>

/*
 * @File: public.h
 * @Brief: 公共宏定义与辅助工具
 */

#define LOG(str) cout << __FILE__ << " : " << __LINE__ << " " << __TIMESTAMP__ << " : " << str << endl;