#include "pch.h"
#include "SGELoader.h"

#include <fstream>
#include <sstream>

sgeElement::sgeElement(sgeElement* parent)
{
    this->parent = parent;
}

void sgeElement::addChild(sgeElement* el)
{
    if (children.count(name)) {
        children.erase(el->getName());
        return;
    }

    children.insert(std::make_pair(el->getName(), std::unique_ptr<sgeElement>(el)));
}

void sgeElement::addValue(std::string name, sgeValue value)
{
    if (values.count(name)) {
        values.at(name) = value;
        return;
    }

    values.insert(std::make_pair(name, value));
}

void sgeElement::setName(std::string name)
{
    this->name = name;
}

int sgeElement::getType(const char* name)
{
    if (values.count(name))
        return values.at(name).idx;

    return -1;
}

int sgeElement::getInt(const char* name)
{
    if (values.count(name))
        return values.at(name).value_int;

    return -1;
}

float sgeElement::getFloat(const char* name)
{
    if (values.count(name))
        return values.at(name).value_flt;

    return -1.0f;
}

const char* sgeElement::getString(const char* name)
{
    if (values.count(name))
        return values.at(name).value_str.c_str();

    return "-1";
}

const char* sgeElement::getName()
{
    return name.c_str();
}

sgeElement* sgeElement::getChild(const char* name)
{
    if (children.count(name))
        return children.at(name).get();

    return nullptr;
}

sgeElement* sgeElement::getParent()
{
    return parent;
}

void SGELoader::ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

// trim from end (in place)
void SGELoader::rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

// trim from both ends (in place)
void SGELoader::trim(std::string& s) {
    rtrim(s);
    ltrim(s);
}

// trim from start (copying)
std::string SGELoader::ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
std::string SGELoader::rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
std::string SGELoader::trim_copy(std::string s) {
    trim(s);
    return s;
}

std::vector<std::string> SGELoader::splitString(std::string source, std::string delimiter)
{
    std::string copy = source;
    size_t pos = 0;
    std::string token;
    std::vector<std::string> split;
    while ((pos = copy.find(delimiter)) != std::string::npos) {
        token = copy.substr(0, pos);
        split.push_back(token);
        copy.erase(0, pos + delimiter.length());
    }
    return split;
}

sgeElement* SGELoader::parseNextClass(std::vector<std::string> source, uint32_t& line)
{
    /*sgeElement* el = new sgeElement();
    for (size_t i = line; i < source.size(); i++)
    {
        if (source[i].find("=") != std::string::npos) {
            auto first = source[i].find_first_of("=");

            std::string name = source[i].substr(0, first);
            std::string val = source[i].substr(first + 1);

            if (val == "{") {
                el->setName(name);
                line++;
                sgeElement* el2 = parseNextClass(source, line);
                el->addChild(el2);
            }
            else if (val == "}") {
                line++;
                break;
            }
            else {
                el->addValue(name, getValueFromString(val));
            }
        }

        line++;
    }

    return el;*/
    return nullptr;
}

sgeValue SGELoader::getValueFromString(std::string val)
{
    sgeValue sval;

    try {
        sval.value_int = std::stoi(val);
        sval.idx = 0;
    }
    catch (const std::exception&) {
        try {
            sval.value_flt = std::stof(val);
            sval.idx = 1;
        }
        catch (const std::exception&) {
            sval.value_str = val;
            sval.idx = 2;
        }
    }

    return sval;
}

SGELoader::SGELoader(const char* def)
{
    m_Def = def;
}

SGELoader::~SGELoader()
{
    m_Resources.clear();
}

sgeElement* SGELoader::createSGE(const char* file)
{
    std::ifstream filestream(m_Def + file);
    std::stringstream stream;
    stream << filestream.rdbuf();
    filestream.close();

    uint32_t line = 0;
    std::vector<std::string> lines = splitString(stream.str(), "\n");

    sgeElement* curr = nullptr;
    sgeElement* root = new sgeElement(nullptr);
    root->setName("ROOT");

    for (line = 0; line < lines.size(); line++)
    {
        trim(lines[line]);
        if (lines[line].find("=") != std::string::npos) {
            auto first = lines[line].find_first_of("=");

            std::string name = lines[line].substr(0, first);
            std::string val = lines[line].substr(first + 1);

            if (val == "{") {
                sgeElement* newcurr = new sgeElement(curr == nullptr ? root : curr);
                newcurr->setName(name);

                if (curr == nullptr) {
                    root->addChild(newcurr);
                    curr = newcurr;
                }
                else if (curr != nullptr) {
                    if (curr->getParent() != nullptr) {
                        curr->addChild(newcurr);
                    }
                    else {
                        root->addChild(newcurr);
                    }
                    curr = newcurr;
                }
            }
            else {
                if (curr != nullptr)
                    curr->addValue(name, getValueFromString(val));
            }
        }
        else if (lines[line] == "}") {
            curr = curr->getParent();
        }
    }
    
    m_Resources.push_back(std::unique_ptr<sgeElement>(root));

    return root;
}

void SGELoader::deleteSGE(sgeElement* el)
{
    for (size_t i = 0; i < m_Resources.size(); i++)
    {
        if (m_Resources[i].get() == el) {
            m_Resources.erase(m_Resources.begin() + i);
            break;
        }
    }
}
