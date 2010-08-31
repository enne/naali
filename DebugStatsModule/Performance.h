#ifndef PERFORMANCE_H_
#define PERFORMANCE_H_

#ifdef Q_WS_WIN

#include <pdh.h>
#include <PDHMsg.h>
#include "GlobalAllocObject.h"

#endif 

#include <string>
#include <sstream>
#include <iomanip>



#include <QString>

namespace PDH
{

#ifndef Q_WS_WIN
    
    struct HCOUNTER {};
    typedef unsigned int PDH_STATUS;
    typedef unsigned int DWORD;
    typedef std::string LPCTSTR;
    typedef char TCHAR;

#define ERROR_SUCCESS 1
#define PDH_FMT_DOUBLE 2
#define PDH_FMT_LONG 3

    struct PDH_FMT_COUNTERVALUE 
    { 
        PDH_FMT_COUNTERVALUE() : doubleValue(0.0), longValue(0) { }

        double doubleValue; 
        long longValue;
    };
    
    struct HQUERY {};
  
 
#endif

/**
 * @brief PerformanceMonitor is as interface class for M$ performance monitor API. 
 */
 
class PerformanceMonitor 
    {
    public: 
        PerformanceMonitor();
        virtual ~PerformanceMonitor();

        /**
         * Reads from M$ performance API that how many threads are used to run this application. 
         * @return viewer thread count. 
         */
        int GetThreadCount();

        /**
         * Reads from M$ performace API that how many disk bytes are average read from all disk. 
         */
        long GetAvgDiskRead();
        
        /**
         * Reads from M$ performace API that how many disk bytes are average write to all disk. 
         */
        long GetAvgDiskWrite();
       

    };

#ifdef Q_WS_WIN
  class Counter : public SharedGlobalObject<HCOUNTER>
#else
  class Counter 
#endif  
  {
    public:
        PDH_STATUS GetFormatted(DWORD fmt, PDH_FMT_COUNTERVALUE*  fmtValue) const
        {
#ifdef Q_WS_WIN
            DWORD  ctrType;
            return PdhGetFormattedCounterValue (*(get()->handle()), fmt, &ctrType, fmtValue);
#else
            return 0;
#endif
        }
   
       double asDouble() const
       {
          PDH_FMT_COUNTERVALUE fmtValue;
          PDH_STATUS pdhStatus = GetFormatted(PDH_FMT_DOUBLE, &fmtValue);
          if (pdhStatus != ERROR_SUCCESS) 
            return -1.0;

          return fmtValue.doubleValue ;
       }

       std::string asDoubleString() const 
       {
          std::stringstream st;
          PDH_FMT_COUNTERVALUE fmtValue;
          PDH_STATUS pdhStatus = GetFormatted(PDH_FMT_DOUBLE, &fmtValue);
          if (pdhStatus == ERROR_SUCCESS) 
             st << std::setprecision(20) << fmtValue.doubleValue ;
          else 
              return "DebugStats::Perfomance.h : asDoubleString() error value were not converted";

          return st.str();
       }

       long asLong() const 
       {
          PDH_FMT_COUNTERVALUE fmtValue;
          PDH_STATUS      pdhStatus = GetFormatted(PDH_FMT_LONG, &fmtValue);
           if (pdhStatus != ERROR_SUCCESS) 
             return -1;

          return fmtValue.longValue;
       }

       std::string asLongString() const 
       {
          std::stringstream st;
          PDH_FMT_COUNTERVALUE   fmtValue;
          PDH_STATUS pdhStatus = GetFormatted(PDH_FMT_LONG, &fmtValue);
           if (pdhStatus == ERROR_SUCCESS) 
              st << fmtValue.longValue;
           else 
               return std::string("DebugStats::Perfomance.h : asLongString() error value were not converted");

          return st.str();
       }
    };


class Query
{
       HQUERY h;
    public:
        Query()   
        { 
#ifdef Q_WS_WIN
            if ( PdhOpenQuery (0, 0, &h) != ERROR_SUCCESS )
                std::cout<<" DebugStats::Perfomance.h : Error in opening pdhquery";
#endif           
        }
        ~Query()  
        { 
#ifdef Q_WS_WIN        
            PdhCloseQuery (h); 
#endif
        }

       operator HQUERY() { return h;}
       
       Counter AddCounter(TCHAR* objectName, TCHAR* counterName )
       {

          Counter pCounterHandle;

#ifdef Q_WS_WIN

          
          PDH_COUNTER_PATH_ELEMENTS pdhCpe;
          PDH_STATUS pdhStatus;

          TCHAR szAvailBytes[256] = TEXT("");
          DWORD dwBufferSize = sizeof(szAvailBytes);

          pdhCpe.szMachineName = NULL;
          pdhCpe.szObjectName = objectName;

          pdhCpe.szInstanceName = NULL;
          pdhCpe.szParentInstance = NULL;
          pdhCpe.dwInstanceIndex = -1;
          pdhCpe.szCounterName = counterName;
         
          if ((pdhStatus=PdhMakeCounterPath(&pdhCpe,szAvailBytes,&dwBufferSize, 0)) == ERROR_SUCCESS) 
          {
              pdhStatus = PdhAddCounter (h,szAvailBytes, 0, pCounterHandle);
              if ( pdhStatus != ERROR_SUCCESS )
              {
                  std::cout<<" DebugStats::Perfomance.h : Error counter were not added"<<std::endl;
              }
              
                   
          
          }
#endif
          
          return pCounterHandle;
       }
       
       PDH_STATUS Collect() const 
       {
#ifdef Q_WS_WIN
           return PdhCollectQueryData (h); 
#else 
           return 0;
#endif
       }
};

}// namespace PDH

#endif //PERFORMANCE_H_