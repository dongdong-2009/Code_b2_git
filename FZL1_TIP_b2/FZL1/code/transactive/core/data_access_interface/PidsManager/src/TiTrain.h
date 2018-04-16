#include <string>
#pragma once;

class TiTrain
{
public:
    TiTrain(unsigned long key);
    TiTrain(unsigned long key,
               unsigned long trainNo,
               std::string displayText);
    unsigned long getKey();
    unsigned long getTrainNo();
	std::string getDisplayText();

private:
    void reload();
    unsigned long m_key;
    unsigned long m_trainNo;
	std::string m_displayText;
    bool m_valid;
};