#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

struct sgeValue {
	int value_int;
	float value_flt;
	std::string value_str;
	uint8_t idx;
};

struct sgeElement {
private:
	std::map<std::string, sgeValue> values;
	std::map<std::string, std::unique_ptr<sgeElement>> children;
	std::string name;
	sgeElement* parent = nullptr;
public:
	sgeElement(sgeElement* parent);

	void addChild(sgeElement* el);
	void addValue(std::string name, sgeValue value);

	void setName(std::string name);

	int getType(const char* name);

	int getInt(const char* name);
	float getFloat(const char* name);
	const char* getString(const char* name);

	const char* getName();

	sgeElement* getChild(const char* name);
	sgeElement* getParent();
};

class SGELoader {
private:
	std::vector<std::unique_ptr<sgeElement>> m_Resources;
	std::string m_Def;

	void ltrim(std::string& s);
	void rtrim(std::string& s);
	void trim(std::string& s);
	std::string ltrim_copy(std::string s);
	std::string rtrim_copy(std::string s);
	std::string trim_copy(std::string s);

	std::vector<std::string> splitString(std::string source, std::string delimiter);
	sgeElement* parseNextClass(std::vector<std::string> source, uint32_t& line);
	sgeValue getValueFromString(std::string val);
public:
	SGELoader(const char* def);
	~SGELoader();

	sgeElement* createSGE(const char* file);
	void deleteSGE(sgeElement* el);
};