/* configure.cpp
*/

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string.h>

#include <libconfig.h++>

using namespace std;
using namespace libconfig;

Config cfg;

int nav(const Setting& ss) {

  //  cout << " Setting Name  : " << ss.getName() << endl;

  int ll = ss.getLength();

  cout << " Setting Path  : " << ss.getPath();

  Setting::Type tt = ss.getType();
  cout << " - " << tt << " - ";

  int i32;
  double dou;
  std::string str;
  bool boo;
  
  switch (tt) {
  case Setting::TypeInt:
  case Setting::TypeInt64:
    i32=0;
    ss.lookupValue(ss.getPath(), i32);
    cout << " = " << i32;
    break;
  case Setting::TypeFloat:
    dou = 0.;
    ss.lookupValue(ss.getPath(), dou);
    cout << " = " << dou;
    break;
  case Setting::TypeString:
    ss.lookupValue(ss.getPath(), str);
    cout << " = " << str;
    break;
  case Setting::TypeBoolean:
    ss.lookupValue(ss.getPath(), boo);
    cout << " = " << boo;
    break;
  default:
    cout << "<" << ll << ">";
    // loop could be here
    break;
  }
  cout << endl;

  for (int i=0;i<ll;i++) {
    nav(ss[i]);
  }

  return ll;

}

/*
 *
 *
 */

int getInt(const char *path, int &ret) {

  int value;

  if (cfg.lookupValue(path,value)) {
    ret = 0;
    return value;
  } else {
    ret = -1;
    cout << " ERROR " << endl;

    exit(-1);
  }

}

// =======================================================

int getX(const char *path, void *value, int nlev, int idx[6], const char *rdx[6]) {

  int *ival;
  float *fval;
  std::string *sval;
  bool *bval;

  int ret=1;

  const Setting& ss = cfg.getRoot();
  Setting *se;
  Setting *bs;

  for (int i=ni, i
  if (bus<0) {
    se=&ss;
  } else {
    se=&(ss["bus"])
      }

  int lb, lm, la;

  lb = ss["bus"].getLength();
  if (bus < lb) {
    lm = ss["bus"][bus]["mpd"].getLength();
    if (mpd<lm) {
      la = ss["bus"][bus]["mpd"][mpd]["apv"].getLength();
      if (apv < la) {
	if (ss["bus"][bus]["mpd"][mpd]["apv"][apv].exists(path)) {
	  bs = &(ss["bus"][bus]["mpd"][mpd]["apv"][apv][path]);	
	  ret = 0;
	}
      } else {
	ret = 2;  // apv out of range
      }
    } else {
      ret = 3; // mpd out of range
    } 
  } else {
    ret = 4; // bus out of range
  }

  if (ret != 0) {   // try default
    if (ss["default"]["bus"]["mpd"]["apv"].exists(path)) {
      bs = &(ss["default"]["bus"]["mpd"]["apv"][path]);
      ret = 1; // no specific value (try default)
    } else {
      cout << __FUNCTION__ << ": [" << bus << "][" << mpd << "][" << apv << "]." << path 
	   << " name does not exist " << -ret << endl;
      exit(-ret);
    }
  }

  Setting::Type tt = bs->getType();
  switch (tt) {
  case Setting::TypeInt:
  case Setting::TypeInt64:
    ival = (int *) value;
    *ival = 0;
    cfg.lookupValue(bs->getPath(), *ival);
    break;
  case Setting::TypeFloat:
    fval = (float *) value;
    *fval = 0.;
    cfg.lookupValue(bs->getPath(), *fval);
    break;
  case Setting::TypeString:
    sval = (std::string *) value;
    *sval = "";
    cfg.lookupValue(bs->getPath(), *sval);
    break;
  case Setting::TypeBoolean:
    bval =(bool *) value;
    *bval = 0;
    cfg.lookupValue(bs->getPath(), *bval);
    break;
  default:
    cout << __FUNCTION__ << ": [" << bus << "][" << mpd << "][" << apv << "]." << path 
	 << " error: type not recognized" << endl;
    exit(-5);
    break;
  }	
  return ret;
  
}



/* -------------------------------------------------------
 * 
 * return value of variable "path" from [bus][mpd][apv] setting,
 * according to type
 * called by getInt, getFloat, getString, getBool ...
 *
 * return 0 if succeded
 * return>0 default value returned (1=no specifc value, 2=apt, out of bound, 3= mpd out of bound)
 * return<0 error (no specific, no default, value has no meaning
 * 
 */

int get(int bus, int mpd, int apv, const char *path, void *value) {

  int *ival;
  float *fval;
  std::string *sval;
  bool *bval;

  int ret;

  const Setting& ss = cfg.getRoot();
  Setting *bs;

  int lb, lm, la;

  ret = 1;

  lb = ss["bus"].getLength();
  if (bus < lb) {
    lm = ss["bus"][bus]["mpd"].getLength();
    if (mpd<lm) {
      la = ss["bus"][bus]["mpd"][mpd]["apv"].getLength();
      if (apv < la) {
	if (ss["bus"][bus]["mpd"][mpd]["apv"][apv].exists(path)) {
	  bs = &(ss["bus"][bus]["mpd"][mpd]["apv"][apv][path]);	
	  ret = 0;
	}
      } else {
	ret = 2;  // apv out of range
      }
    } else {
      ret = 3; // mpd out of range
    } 
  } else {
    ret = 4; // bus out of range
  }

  if (ret != 0) {   // try default
    if (ss["default"]["bus"]["mpd"]["apv"].exists(path)) {
      bs = &(ss["default"]["bus"]["mpd"]["apv"][path]);
      ret = 1; // no specific value (try default)
    } else {
      cout << __FUNCTION__ << ": [" << bus << "][" << mpd << "][" << apv << "]." << path 
	   << " name does not exist " << -ret << endl;
      exit(-ret);
    }
  }

  Setting::Type tt = bs->getType();
  switch (tt) {
  case Setting::TypeInt:
  case Setting::TypeInt64:
    ival = (int *) value;
    *ival = 0;
    cfg.lookupValue(bs->getPath(), *ival);
    break;
  case Setting::TypeFloat:
    fval = (float *) value;
    *fval = 0.;
    cfg.lookupValue(bs->getPath(), *fval);
    break;
  case Setting::TypeString:
    sval = (std::string *) value;
    *sval = "";
    cfg.lookupValue(bs->getPath(), *sval);
    break;
  case Setting::TypeBoolean:
    bval =(bool *) value;
    *bval = 0;
    cfg.lookupValue(bs->getPath(), *bval);
    break;
  default:
    cout << __FUNCTION__ << ": [" << bus << "][" << mpd << "][" << apv << "]." << path 
	 << " error: type not recognized" << endl;
    exit(-5);
    break;
  }	
  return ret;
  
}

/*
 * type explicit methos for [mpd][apv]
 */

int getInt(int bus, int mpd, int apv, const char *name, int &ret) {
  int val;
  ret = get(bus, mpd, apv, name, &val);
  return val;
}

float getFloat(int bus, int mpd, int apv, const char *name, int &ret) {
  float val;
  ret = get(bus, mpd, apv, name, &val);
  return val;
}

std::string getString(int bus, int mpd, int apv, const char *name, int &ret) {
  std::string val;
  ret = get(bus, mpd, apv, name, &val);
  return val;
}

bool getBool(int bus, int mpd, int apv, const char *name, int &ret) {
  bool val;
  ret = get(bus, mpd, apv, name, &val);
  return val;
}



/*
 * return Int value from [mpd][apv] setting
 * 
 * ret=0 ok, value found
 * ret>0 default value returned
 * ret<0 error, value has no meaning
 * 
 */
/*
int getInt(int mpd, int apv, const char *path, int &ret) {

  int value;

  const Setting& ss = cfg.getRoot();

  int lm, la;

  ret = -1;

  lm = ss["mpd"].getLength();
  if (mpd<lm) {
    la = ss["mpd"][mpd]["apv"].getLength();
    if (apv < la) {
      if (ss["mpd"][mpd]["apv"][apv].lookupValue(path, value)) {
	ret = 0;
	return value;
      } else {
	ret = 1; // no specific setting
      }
    } else {
      ret = 2; // apv out of range
    }
  } else {
    ret = 3; // mpd out of range
  }

  // use default
  if (ss["default"]["mpd"]["apv"].lookupValue(path, value)) {
    return value;
  } 

  // return "error" value
  ret = -ret;
  cout << __FUNCTION__ << ": Error " << ret << endl;

  return -999999; // error

}
*/

int main() {


  try
    {
      cfg.readFile("../cfg/config_new.txt");
    }
  catch(const FileIOException &fioex)
    {
      std::cerr << "I/O error while reading file." << std::endl;
      return(EXIT_FAILURE);
    }
  catch(const ParseException &pex)
    {
      std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
		<< " - " << pex.getError() << std::endl;
      return(EXIT_FAILURE);
    }
  
  const Setting& root = cfg.getRoot();

  cout << root.getLength() << endl;
  
  //  cout << " Root Name  : " << root.getName() << endl;

  nav(root);

  cout << " Nav Done " << endl;

  int ret;

  cout << " ## " << getInt(0,1,18,"Ipsf", ret) << endl;

  cout << " --  " << getInt(0,0,4,"i2c",ret) << endl;
  cout << " **  " << getInt(0,3,4,"iii",ret) << endl;

  //  cout << getInt(root["default"]["mpd"],"rotary") << endl;

  //  cout << getInt(root["mpd"][1]["apv"][2],"adc") << endl;

  return 0;
  
}
