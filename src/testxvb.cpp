#include <Vme_gexvb.h>
#include <iostream>

using namespace std;

int Bus::fBus_Idx=-1;

int main()
{
  int baseadd =0x20000000;
  long int * data = new long int;
  long int * rdata = new long int;
  VmeXVB toto;
  *data=0x40241234;
  //  toto.Write(0x08000000 + 0x01000000, data);
  toto.Read(baseadd + 0x01000000, rdata);
  cout<<hex<<*rdata<<endl;
  *data=0x12345678;
  //  toto.Write(baseadd + 0x02000000, data);
  toto.Read(baseadd + 0x02000000, rdata);
  cout<<hex<<*rdata<<endl;
  *data=0x12345678;
  // toto.Write(baseadd + 0x02000004, data);
  toto.Read(baseadd + 0x02000004, rdata);
  cout<<hex<<*rdata<<endl;
 
}
