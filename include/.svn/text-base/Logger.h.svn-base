/* logger.h
 *
 * This is a singleton
 */

#ifndef __LOGGER__H
#define __LOGGER__H

#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

class Logger
{
 private:

  ofstream logfile;

  static Logger *fLog;

  int fLevel; // log level

 public:
  Logger() { 

    if (fLog) {
      cout << __FUNCTION__ << ": logger is a singleton, cannot be allocated twice!" << endl;
      return;
    }

    fLog = this;

    fLevel = 1;

    cout << __FUNCTION__ << ": allocated" << endl;

  };

  ~Logger(void) { };
  
  bool Open(string fname);
  void Write(string s);
  void Close(void) { logfile.close(); }

  void WritePar(string s, int v);
  void WritePar(string s, float v);
  void WritePar(string s, string v);

  static Logger *GetLogger();

  void SetVerboseLevel(int lev) { fLevel = lev; };

};

#endif 
