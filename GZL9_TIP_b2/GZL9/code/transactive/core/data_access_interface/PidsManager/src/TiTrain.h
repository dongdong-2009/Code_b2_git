#include <string>
#pragma once;

class TiTrain
{
public:
    TiTrain(unsigned long key);
    TiTrain(unsigned long key,
               int trainNo,
               std::string displayText);
    unsigned long getKey();
    int getTrainNo();
	std::string getDisplayText();

private:
    void reload();
    unsigned long m_key;
    int m_trainNo;
	std::string m_displayText;
    bool m_valid;
};