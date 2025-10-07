#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <cassert>

//MACROS PARA DIRECTX Y COSAS DE WINDOWS
#define SAFE_RELEASE(p) { if(p){p->Release(); p = nullptr;} }
#define SHOW_ERROR(msg) {MessageBox(nullptr,msg, L"Error",MB_OK);}

//SMART POINTERS
template <typename T>
using UPtr = std::unique_ptr<T>;

template <typename T>
using SPtr = std::shared_ptr<T>;

template <typename T>
using WPtr = std::weak_ptr<T>;

using std::make_shared;
using std::make_unique;

//STL STUFF
template <typename T>
using Vector = std::vector<T>;

template <typename K, typename V>
using UnorderedMap = std::unordered_map<K, V>;

template <typename K, typename V>
using Map = std::map<K, V>;

using String = std::string;

//FUNCTIONS
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

// UINTS
// Para que el valor de los ints sea siempre el mismo no importa la arquitectura.
using uint8		= std::uint8_t;
using uint16	= std::uint16_t;
using uint32	= std::uint32_t;
using uint64	= std::uint64_t;

using int8		= std::int8_t;
using int16		= std::int16_t;
using int32		= std::int32_t;
using int64		= std::int64_t;