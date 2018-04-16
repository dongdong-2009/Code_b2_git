
#include "app/cctv/tvss_agent/src/TVSSHelper.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_IRS_App
{
	//
    // Trying to ensure that strtok() and hence tokenise() is used not used in a
    // re-entrant situation, we use a non-reentrant thread lock which asserts if
    // tokenise is used incorrectly.  Because the strtok data that we are preventing
    // reentrant access to is static, we also make the lock so.
    static TA_Base_Core::NonReEntrantThreadLockable s_strtokLock;

	int TVSSHelper::tokenise(const char *s, std::vector<std :: string> &tokens, const char *delimiters, int maxtokens)
    {
        TA_Base_Core::ThreadGuard strtokGuard(s_strtokLock);

        if (maxtokens<2)
        {
            tokens.push_back(std::string(s));
            return tokens.size();
        }

        int numchars = strlen(s)+1;

        if (numchars > 1024)
        {
            std::cerr << "WARNING: truncating input to 1024 characters" << std::endl;
            numchars=1024;
        }

        char s_[1024];
        strncpy(s_,s,1024); // pads with zeros if numchars<1024
        s_[numchars] = 0;
        int numtokens=0;

        char * token = strtok(s_, delimiters);
        while (( token != 0 ) && (numtokens < maxtokens-1))
        {
            tokens.push_back(token);
            numtokens++;

            token = strtok(0, delimiters);
        }

        //
        // add the remainder of the string, if there is any left and we stopped
        // because we were about to exceed the maxtokens limit.
        if ((token!=0) && (numtokens >= maxtokens-1))
        {
            int tokenlen = strlen(token);
            // remove the string terminator and replace it with the first delimiter
            // BUT ONLY if the end of the string is not nigh anyway
            if ( (token + tokenlen) < (s_ + numchars -1) )
            {
                token[tokenlen] = delimiters[0];
            }

            tokens.push_back(token);
        }

        return tokens.size();
    }

	TVSSHelper::TVSSHelper()
	{
	}
	TVSSHelper::~TVSSHelper()
	{
	}
}