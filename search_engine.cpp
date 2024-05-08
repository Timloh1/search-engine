#include "search_engine.h"
//
void filling_freq_dictionary(string str, int doc_id, int size) {
	str = " " + str + " ";
	int i = 1;

	for (;;) {
		bool begin_word = (str[i] >= 'a' && str[i] <= 'z') && str[i - 1] == ' ';
		bool end_word = (str[i - 1] >= 'a' && str[i - 1] <= 'z') && str[i] == ' ';

		if (begin_word) {
			str = str.substr(i);
			i = 0;
		}
		if (end_word) {
			string word = str.substr(0, str.length() - str.substr(i - 1).length() + 1);

			access__freq_dictionary.lock();
			if (_freq_dictionary.count(word) > 0) {
				_freq_dictionary[word][doc_id].doc_id = doc_id;
				_freq_dictionary[word][doc_id].count += 1;
			}
			else {
				vector<Entry> newVec(size);
				_freq_dictionary.insert({ word, newVec });
				_freq_dictionary[word][doc_id].doc_id = doc_id;
				_freq_dictionary[word][doc_id].count += 1;
			}
			access__freq_dictionary.unlock();

			str = str.substr(i);
			i = 0;
		}
		if (str.length() == 1) break;

		++i;
	}
}
//
Entry::Entry() {}
Entry::Entry(size_t _doc_id, size_t _count) : doc_id(_doc_id), count(_count) {}
bool Entry::operator==(const Entry& other) const 
{
	return (doc_id == other.doc_id && count == other.count);
}
RelativeIndex::RelativeIndex() {}
RelativeIndex::RelativeIndex(size_t _doc_id, float _rank) : doc_id(_doc_id), rank(_rank)
{}
bool RelativeIndex::operator ==(const RelativeIndex& other) const {
	return (doc_id == other.doc_id && rank == other.rank);
}
//
string version = to_string(APP_VERSION_MAJOR) + '.' + to_string(APP_VERSION_MINOR) + '.' + to_string(APP_VERSION_PATCH);
map<string, vector<Entry>> _freq_dictionary;
mutex access__freq_dictionary;
//
const char* NoFileConfig::what() const noexcept
{
	return "config file is empty";
}
const char* NoFileRequests::what() const noexcept
{
	return "requests file is empty";
}
const char* VersionsNotMatch::what() const noexcept
{
	return "the versions do not match";
}
//
vector<string> ConverterJSON::GetTextDocuments() {
	try {
		ifstream file("config.json");

		if (!file.is_open()) throw NoFileConfig();
		nlohmann::json dict;
		file >> dict;
		file.close();

		if (dict["config"] == nullptr) throw NoFileConfig();

		try {
			if (dict["config"]["version"] != version) throw VersionsNotMatch();
		}
		catch (const VersionsNotMatch& e) {
			cout << e.what() << endl;
		}

		vector<string> output;

		for (string new_path : dict["files"]) {
			try {
				ifstream new_file(new_path);

				if (!new_file.is_open()) throw NoFileInConfig();
				else {
					string str;
					getline(new_file, str);
					output.push_back(str);
					new_file.close();
				}
			}
			catch (const NoFileInConfig& e) {
				cout << "the file at the address \"" << new_path << "\" was not found" << endl;
			}
		}

		return output;
	}
	catch (const NoFileConfig& e) {
		cout << e.what() << endl;
	}
}
int ConverterJSON::GetResponsesLimit() {
	try {
		ifstream file("config.json");

		if (!file.is_open()) throw NoFileConfig();
		nlohmann::json dict;
		file >> dict;
		file.close();
		if (dict["config"] == nullptr) throw NoFileConfig();

		return dict["config"]["max_responses"];
	}
	catch (const NoFileConfig& e) {
		cout << e.what() << endl;
		assert(0);
	}
}
vector<string> ConverterJSON::GetRequests() {
	try {
		ifstream file("requests.json");

		if (!file.is_open()) throw NoFileRequests();
		nlohmann::json dict;
		file >> dict;
		file.close();

		vector<string> output;

		for (string request : dict["requests"]) output.push_back(request);

		return output;
	}
	catch (const NoFileRequests& e) {
		cout << e.what() << endl;
		assert(0);
	}
}
void ConverterJSON::putAnswers(vector<vector<RelativeIndex>> answers) {
	ofstream file("answers.json");
	file.clear();

	nlohmann::json dict;

	auto answer_null = [](vector<RelativeIndex>& answer) {
		for (auto c : answer) {
			if (c.rank != 0.f) return true;
		}
		return false;
	};

	for (int i = 0; i < answers.size(); ++i) {
		string request = "request" + to_string(i);
		if (answer_null(answers[i])) {
			dict["answers"][request] = nlohmann::json::array();
			dict["answers"][request].push_back({
				{"result", true}
			});

			dict["answers"][request][1]["relevance"] = nlohmann::json::array();
			for (int j = 0; j < answers[i].size(); ++j) {
				dict["answers"][request][1]["relevance"].push_back({
					{"docid", answers[i][j].doc_id}, {"rank", answers[i][j].rank}
				});
			}
		}
		else {
			dict["answers"][request] = { {"result", false} };
		}
	}

	file << dict.dump(4);

	file.close();
}
string ConverterJSON::getName() {
	try {
		ifstream file("config.json");

		if (!file.is_open()) throw NoFileConfig();

		nlohmann::json dict;
		file >> dict;

		if (dict["config"] == nullptr) throw NoFileConfig();

		file.close();

		return dict["config"]["name"];
	}
	catch (const NoFileConfig& e) {
		cout << e.what() << endl;
		assert(0);
	}
}
//
int InvertedIndex::getCountDocs() {
	return (int) docs.size();
}
vector<Entry> InvertedIndex::GetWordCount(const string& word) {
	return freq_dictionary[word];
}
map<string, vector<Entry>> InvertedIndex::getFreqDictionary() {
	return freq_dictionary;
}
void InvertedIndex::UpdateDocumentBase(vector<string> input_docs) {
	docs = input_docs;
	int size = docs.size();
	vector<thread> threads;

	for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
		string str = docs[doc_id];
		threads.emplace_back(filling_freq_dictionary, str, doc_id, size);
	}

	for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
		threads[doc_id].join();
	}

	freq_dictionary = _freq_dictionary;
}
//
SearchServer::SearchServer(InvertedIndex _index) : index(_index)
{}
vector<vector<RelativeIndex>> SearchServer::search(const vector<string>& queries_input) {
	vector<multimap<size_t, ParamWords>> requests;
	multimap<size_t, ParamWords> words;

	for (size_t words_id = 0; words_id < queries_input.size(); ++words_id) {
		words.clear();
		string str = queries_input[words_id];		
		str = " " + str + " ";
		int i = 1;

		for (;;) {
			bool begin_word = (str[i] >= 'a' && str[i] <= 'z') && str[i - 1] == ' ';
			bool end_word = (str[i - 1] >= 'a' && str[i - 1] <= 'z') && str[i] == ' ';

			if (begin_word) {
				str = str.substr(i);
				i = 0;
			}
			if (end_word) {
				string word = str.substr(0, str.length() - str.substr(i - 1).length() + 1);

				if (index.getFreqDictionary().count(word) > 0) {
					int counter = 0;
					vector<int> param;
					for (int j = 0; j < index.getCountDocs(); ++j) {
						param.push_back(index.getFreqDictionary()[word][j].count);
						counter += index.getFreqDictionary()[word][j].count;
					}
					ParamWords newParamWords;
					newParamWords.count = param;
					newParamWords.word = word;
					words.insert({ counter, newParamWords });
				}
				else {
					vector<int> param;
					for (int j = 0; j < index.getCountDocs(); ++j)
						param.push_back(0);
					ParamWords newParamWords;
					newParamWords.count = param;
					newParamWords.word = word;
					words.insert({ 0, newParamWords });
				}

				str = str.substr(i);
				i = 0;
			}
			if (str.length() == 1) break;

			++i;
		}
		requests.push_back(words);
	}

	vector<vector<RelativeIndex>>output(requests.size());

	for (int i = 0; i < requests.size(); ++i) {
		output[i].resize(index.getCountDocs());
		vector<int> counter(index.getCountDocs());

		for (int j = 0; j < index.getCountDocs(); ++j) {
			for (auto it = requests[i].begin(); it != requests[i].end(); ++it)
				counter[j] += it->second.count[j];
		}

		int max_counter = counter[0];
		for (int j = 0; j < index.getCountDocs(); ++j)
			if (max_counter < counter[j]) max_counter = counter[j];

		for (int j = 0; j < index.getCountDocs(); ++j) {
			if (max_counter > 0) output[i][j].rank = (float) counter[j] / (float) max_counter;
			else output[i][j].doc_id = 0.f;
			output[i][j].rank = j;
		}
	}

	return output;
}
//