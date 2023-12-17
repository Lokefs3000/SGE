#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <map>

enum class DataVariantType {
	None,
	Int,
	Float,
	String
};

struct DataVariant {
	int ValInt;
	float ValFloat;
	std::string ValString;
	DataVariantType ValType;
};

class DataElement {
private:
	std::shared_ptr<DataElement> m_Self;
	std::map<std::string, DataVariant> m_Values;
	std::map<std::string, std::shared_ptr<DataElement>> m_Children;
	std::shared_ptr<DataElement> m_Parent = NULL;
	std::string m_Name = "";
public:
	DataElement();
	~DataElement();

	void Self(std::shared_ptr<DataElement> self);

	void SetParent(std::shared_ptr<DataElement> element);
	void SetName(std::string name);
	void AddChild(std::shared_ptr<DataElement> element);
	void RemoveChild(std::shared_ptr<DataElement> element);
	void AddValue(std::string name, DataVariant data);
	void RemoveValue(std::string name);

	std::shared_ptr<DataElement> GetChild(std::string name);
	DataVariant& GetValue(std::string name);
	std::string GetName();
	std::shared_ptr<DataElement> GetParent();
};

class DataParser {
public:
	static std::shared_ptr<DataElement> ParseTextData(std::string path);
	static std::shared_ptr<DataElement> ParseSourceData(std::string& source);
};