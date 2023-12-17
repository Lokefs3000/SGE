#include "pch.h"
#include "DataParser.h"

#include "FileHandler.h"
#include "StringUtility.h"
#include <iostream>

DataElement::DataElement()
{
}

DataElement::~DataElement()
{
    std::cout << "dealloc: " << this << ", name: " << m_Name << ", parent: " << m_Parent << ", pname: " << m_Parent->GetName() << std::endl;
}

void DataElement::Self(std::shared_ptr<DataElement> self)
{
    m_Self = self;
}

void DataElement::SetParent(std::shared_ptr<DataElement> element)
{
    if (element == m_Parent) {
        return;
    }

    if (m_Parent != nullptr) {
        m_Parent->RemoveChild(m_Self);
    }

    m_Parent = element;

    if (m_Parent != nullptr) {
        m_Parent->AddChild(m_Self);
    }
}

void DataElement::SetName(std::string name)
{
    if (m_Parent != nullptr) {
        m_Parent->RemoveChild(m_Self);
    }

    m_Name = name;

    if (m_Parent != nullptr) {
        m_Parent->AddChild(m_Self);
    }
}

void DataElement::AddChild(std::shared_ptr<DataElement> element)
{
    if (!m_Children.count(element->GetName())) {
        m_Children.insert(std::make_pair(element->GetName(), element));
    }
}

void DataElement::RemoveChild(std::shared_ptr<DataElement> element)
{
    if (m_Children.count(element->GetName())) {
        m_Children.erase(element->GetName());
    }
}

void DataElement::AddValue(std::string name, DataVariant data)
{
    if (!m_Values.count(name)) {
        m_Values.insert(std::make_pair(name, data));
    }
}

void DataElement::RemoveValue(std::string name)
{
    if (m_Values.count(name)) {
        m_Values.erase(name);
    }
}

std::shared_ptr<DataElement> DataElement::GetChild(std::string name)
{
    if (m_Children.count(name)) {
        return m_Children.at(name);
    }

    return nullptr;
}

DataVariant& DataElement::GetValue(std::string name)
{
    if (m_Values.count(name)) {
        return m_Values.at(name);
    }

    DataVariant v;
    return v;
}

std::string DataElement::GetName()
{
    return m_Name;
}

std::shared_ptr<DataElement> DataElement::GetParent()
{
    return m_Parent;
}

/////////////////////////////-SEPERATOR-/////////////////////////////

std::shared_ptr<DataElement> DataParser::ParseTextData(std::string path)
{
    std::string src = FileHandler::ReadText(path);
    return ParseSourceData(src);
}

std::shared_ptr<DataElement> DataParser::ParseSourceData(std::string& source)
{
    std::shared_ptr<DataElement> root = std::make_shared<DataElement>();
    root->Self(root);
    root->SetName("ROOT");

    std::vector<std::string> lines = StringUtility::SplitString(source, "\n");

    std::shared_ptr<DataElement> curr = root;

    bool readingString = false;
    std::string stringRead = "";
    std::string stringName = "";

    for (size_t i = 0; i < lines.size(); i++)
    {
        std::string line = StringUtility::Trim(lines[i]);
        auto findSeparator = line.find_first_of(":");

        if (StringUtility::Trim(line) == "}") {
            std::shared_ptr<DataElement> newCurr = curr->GetParent();
            if (newCurr == NULL)
                newCurr = root;
            curr = newCurr;
            continue;
        }

        if (line.empty() || (findSeparator == std::string::npos && !readingString))
            continue;

        std::string name = StringUtility::Trim(line.substr(0, findSeparator));
        std::string value = StringUtility::Trim(line.substr(findSeparator + 1));

        if (readingString) {
            if (value.empty())
                continue;

            auto endFind = line.find_last_of("\"");
            if (endFind != std::string::npos) {
                stringRead += StringUtility::TrimChar(value, '"');
                readingString = false;
                curr->AddValue(stringName, { 0, 0.0f, stringRead, DataVariantType::String });
                continue;
            }

            stringRead += StringUtility::Trim(value);
        }
        else {
            if (name.empty())
                continue;

            if (value == "{") {
                std::shared_ptr<DataElement> newCurr = std::make_shared<DataElement>();
                newCurr->Self(newCurr);

                newCurr->SetName(name);
                newCurr->SetParent(curr);

                curr = newCurr;
            }
            else {
                auto findStr = value.find_first_of("\"");

                if (findStr != std::string::npos) {
                    lines[i] = lines[i].substr(1);
                    i--;
                    readingString = true;

                    stringRead = "";
                    stringName = name;
                }
                else {
                    try
                    {
                        int val = std::stoi(value);
                        curr->AddValue(name, { val, 0.0f, "", DataVariantType::Int });
                    }
                    catch (const std::exception&)
                    {
                        try
                        {
                            float val = std::stoi(value);
                            curr->AddValue(name, { 0, val, "", DataVariantType::Float });
                        }
                        catch (const std::exception&)
                        {
                            //none
                            std::cout << "none: " << line << std::endl;
                        }
                    }
                }
            }
        }
    }

    return root;
}
