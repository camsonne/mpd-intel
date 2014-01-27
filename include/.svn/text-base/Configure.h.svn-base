/* configure.h
 * Use libconfig
 */

#ifndef __CONFIGURE__H
#define __CONFIGURE__H

#include <libconfig.h++>

class GI_MPD;
class DaqControl;

class DaqConfig
{
	public:
		DaqConfig();
		~DaqConfig();

		int Read(char *configfile);
		int GeneralScan(void);
		int ComponentDefaultScan(GI_MPD *gMPD);
		int ApvAddrScan(GI_MPD *gMPD);
		int ParseInlineParam(int argc, char *argv[]);
		void Save(char *outfile);

	private:
		void GetController(void);
		void GetLogFile(void);
		void GetLogLevel(void);
		void GetDaqMode(void);
		void GetDaqFormat(void);
		void GetProgressStep(void);
		void GetTimePreset(void);
		void GetHistoTime(void);
		void GetHistoChannel(void);
		void GetHistoBoard(void);
		void GetEventPreset(void);
		void GetFilePrefix(void);
		void GetMaxRetry(void);
		void GetLatencies(void);
		void GetTrigNumber(void);

		DaqControl *fDaqCtrl;

};

#endif
