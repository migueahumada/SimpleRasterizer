#pragma once

#include <memory>
#define SAFE_RELEASE(p) { if(p){p->Release(); p = nullptr;} }
#define SHOW_ERROR(msg) {MessageBox(nullptr,msg, L"Error",MB_OK);}

#include <string>
#include <vector>
#include <unordered_map>

template <typename T>
using UPtr = std::unique_ptr<T>;

template <typename T>
using SPtr = std::shared_ptr<T>;

template <typename T>
using WPtr = std::weak_ptr<T>;

template <typename T>
using Vector = std::vector<T>;

template <typename K, typename V>
using UnorderedMap = std::unordered_map<K, V>;

using String = std::string;
using std::make_shared;
using std::make_unique;

static String trim(const String& str)
{
	size_t start = str.find_first_not_of(' ');
	size_t end = str.find_last_not_of(' ');
	return str.substr(start, end - start + 1);
}

static Vector<String> split(const String& str, char delim) 
{
	Vector<String> tokens;
	size_t start = 0;
	size_t end = str.find(delim);
	while (end != String::npos)
	{
		tokens.push_back(trim(str.substr(start, end - start)));
		start = end + 1;
		end = str.find(delim, start);
	}
	tokens.push_back(str.substr(start, end));
	return tokens;
}
