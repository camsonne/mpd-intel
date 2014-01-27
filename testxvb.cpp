#include <Vme_gexvb.h>
#include <iostream>

using namespace std;

int Bus::fBus_Idx=-1;

int main()
{
  
  int stat=0;
  volatile unsigned int laddr;
  volatile unsigned int tAddr;
  int r=1;
  tAddr=r<<27;
  cout<<endl <<"0x"<<hex<<tAddr <<endl;
    
  int baseadd =tAddr;
  long int * data = new long int;
  long int * rdata = new long int;
  VmeXVB toto;

  stat = vmeBusToLocalAdrs(0x09,(char *)tAddr,(char **)&laddr);// 0x09 is for A32
  if (stat != 0)
    {
      printf("%s: ERROR: Error in vmeBusToLocalAdrs res=%d \n",__FUNCTION__,stat);
      return -1;
      exit(0);
    };


 //  *data=0xabcdef;
//  toto.Write(baseadd +0x1000000,data);
// toto.Read(baseadd +0x1000000,rdata); 
//  cout<<"Address 0x01000000: 0x"<<hex<<*rdata<<endl;


//   *data=0x2;
// toto.Write(baseadd +0x2000000,data);
// toto.Read(baseadd +0x2000000,rdata); 
// cout<<"Address 0x02000000: 0x"<<hex<<*rdata<<endl;
 
//   *data=0x4;
// toto.Write(0x09,4,baseadd +0x2000004,data);
// toto.Read(0x09,4,baseadd +0x2000004,rdata); 
// cout<<"Address 0x02000000: 0x"<<hex<<*rdata<<endl;

//   *data=0x6;
// toto.Write(0x09,4,baseadd +0x2000008,data);
// toto.Read(0x09,4,baseadd +0x2000008,rdata); 
// cout<<"Address 0x02000000: 0x"<<hex<<*rdata<<endl;


 //////////Fill up FIFO once with ADC samples/////// 
long int r_config=baseadd + 0x4000000 +0x80030;
// cout<<endl<<hex<<"0x"<<*rdata<<endl;
 toto.Read(r_config,rdata);
 *data=2;
 *data|=*rdata;
 toto.Write(r_config,data);
toto.Read(r_config,rdata);
 cout<<"Read Config is set to : "<<hex<<"0x"<<*rdata<<endl;

//////////Fill up FIFO once with ADC samples/////// 
// cout<<endl<<hex<<"0x"<<*rdata<<endl;
 *data=1<<15;
 *data|=*rdata;
 toto.Write(r_config,data);
toto.Read(r_config,rdata);
 cout<<"Read Config is set to : "<<hex<<"0x"<<*rdata<<endl;



//////////Generates 1 APV trig every input raising edge/////// 
long int trig_gen_config=baseadd + 0x4000000 +0x80034;
 *data=1<<12;
 *data|=*rdata;
 toto.Write(trig_gen_config,data);
 toto.Read(trig_gen_config,rdata);
 cout<<"Trig Gen Config is set to : "<<hex<<"0x"<<*rdata<<endl;

 for(int i=0;i<10;i++){
   toto.Read(baseadd,rdata);
   cout<<endl<<"FIFO DATA: 0x"<<hex<<*rdata<<endl;
 }
}