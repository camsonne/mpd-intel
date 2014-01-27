/* logger.cpp
 *
 * Singleton
 */

#include <time.h>

#include "Logger.h"
#include "DaqControl.h"

Logger* Logger::fLog = 0;
Logger* Logger::GetLogger() { return fLog; }

bool Logger::Open(string fname)
{
  //	string fname = DaqCtrl.GetLogFile();
  if( fname.length() == 0 )
    return false;
  if( fLevel > 0 )
    {
      //		logfile.open(fname.data(), fstream::app);
      logfile.open(fname.data());
      if( logfile.good() )
	return true;
      else
	return false;
    }
  else
    return true;
}

void Logger::Write(string s)
{
  time_t now;
  struct tm *timestruct;
  char *ts;

  time(&now);
  timestruct = localtime(&now);
  ts = asctime(timestruct);
  ts[strlen(ts)-1] = 0;

  switch( fLevel )
    {
    case 0: break;
    case 1:
    case 2:	
      logfile << ts << "   ";
      logfile << s;
      logfile << endl;
      break;
    }
}

//

void Logger::WritePar(string s, int v) {
  
  char logstring[512];
  
  sprintf(logstring,"%s = %d",s.data(),v);
  
  Write(logstring);
  
}

void Logger::WritePar(string s, float v) {
  
  char logstring[512];
  
  sprintf(logstring,"%s = %f",s.data(),v);
  
  Write(logstring);
  
}


void Logger::WritePar(string s, string v) {

  char logstring[512];
  
  sprintf(logstring,"%s = %s",s.data(),v.data());

  Write(logstring);

}
