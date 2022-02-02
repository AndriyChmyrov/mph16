/****************************************************************************\
*																			*
*		    Thorlabs MPH16 motorized pinhole interface						*
*			Andriy Chmyrov © 18.03.2019										*
*																			*
\****************************************************************************/

#ifndef NDEBUG
#define DEBUG(text) mexPrintf("%s:%d - %s\n",__FILE__,__LINE__,text);	// driver actions

#define MEXMESSAGE(error) mexMessage(__FILE__,__LINE__,error)
#define MEXENTER mexEnter(__FILE__,__LINE__)
#else
#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG(text)

#define MEXMESSAGE(error) mexMessage(error)
#define MEXENTER mexEnter()
#endif
#define MEXLEAVE mexLeave()

#define MATLAB_DEFAULT_RELEASE R2018a

#ifndef NDEBUG
void mexEnter(const char* file, const int line);
void mexMessage(const char* file, const int line, long i32Status);
#else
void mexEnter(void);
void mexMessage(long i32Status);
#endif
void mexLeave(void);
void mexCleanup(void);

const mwSize StringLength = 256;

#define MPH16_ALIGNMENT_MODE_ON 1
#define MPH16_ALIGNMENT_MODE_OFF 0

extern long mph16, mph16s;		// Pinholes
extern double pos_limits[2];

double	getScalar(const mxArray* array);
//mxArray*	getParameter(const char* name);
mxArray* getParameter(const char* name, const mxArray* field = NULL);
void	setParameter(const char* name, const mxArray* field);

#ifdef __cplusplus 
extern "C" bool utIsInterruptPending();
#else
extern bool utIsInterruptPending();
#endif