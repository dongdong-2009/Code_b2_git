#pragma once

class ViTriggeringMap
{
    public:
        ViTriggeringMap(unsigned long triggeringEntityKey);
        ViTriggeringMap(unsigned long triggeringEntityKey,
			            unsigned long cameraEntityKey);
        ~ViTriggeringMap(void);
        unsigned long getTriggeringEntityKey();
        unsigned long getCameraEntityKey();
	private:
		void reload();
		unsigned long m_triggeringEntityKey;
		unsigned long m_cameraEntityKey;
		bool m_valid;
};
