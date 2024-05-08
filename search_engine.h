#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <exception>
#include <thread>
#include <mutex>
#include <ctime>
#include <cassert>

#include "nlohmann/json.hpp"

using namespace std;

#define APP_VERSION_MAJOR 1
#define APP_VERSION_MINOR 0
#define APP_VERSION_PATCH 0

void filling_freq_dictionary(string str, int doc_id, int size);

struct Entry {
	size_t doc_id = 0, count = 0;

	Entry();
	Entry(size_t _doc_id, size_t _count);
	bool operator==(const Entry& other) const;
};
struct RelativeIndex {
	size_t doc_id = 0;
	float rank = 0;

	RelativeIndex();
	bool operator ==(const RelativeIndex& other) const;
	RelativeIndex(size_t _doc_id, float _rank);
};
struct ParamWords {
	string word = "";
	vector<int> count;
};

extern string version;
extern map<string, vector<Entry>> _freq_dictionary;
extern mutex access__freq_dictionary;

class NoFileConfig : public exception
{
public:
	const char* what() const noexcept override;
};
class NoFileRequests : public exception
{
public:
	const char* what() const noexcept override;
};
class VersionsNotMatch : public exception
{
public:
	const char* what() const noexcept override;
};

class NoFileInConfig : public exception
{};

class ConverterJSON 
{
public:
	ConverterJSON() = default;
	vector<string> GetTextDocuments();
	int GetResponsesLimit();
	vector<string> GetRequests();
	void putAnswers(vector<vector<RelativeIndex>> answers);
	string getName();
};

class InvertedIndex {
	vector<string> docs;
	map<string, vector<Entry>> freq_dictionary;
public:
	InvertedIndex() = default;
	int getCountDocs();
	vector<Entry> GetWordCount(const string& word);
	map<string, vector<Entry>> getFreqDictionary();
	void UpdateDocumentBase(vector<string> input_docs);
};

class SearchServer {
	InvertedIndex index;
public:
	SearchServer(InvertedIndex _index);
	vector<vector<RelativeIndex>> search(const vector<string>& queries_input);
};
