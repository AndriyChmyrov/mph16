// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "mph16.h"


#define VALUE	plhs[0]


/*	Driver mutually exclusive access lock (SEMAPHORE).

The semaphore is granted by the mexEnter() function. The owner of the
semaphore must release it with mexLeave().
*/
static HANDLE driver;


/*	Handle of the DLL module itself.
*/
static HANDLE self;


/*	Library initialization and termination.

This function creates a semaphore when a process is attaching to the
library.
*/
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	UNREFERENCED_PARAMETER(ul_reason_for_call);
	UNREFERENCED_PARAMETER(lpReserved);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		self = hModule;
		driver = CreateSemaphore(NULL, 1, 1, NULL);
		return driver != NULL;
	case DLL_THREAD_ATTACH:
		self = hModule;
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		// next line is actually excessive, because mexCleanup is registered in mexStartup
		//mexCleanup();
		break;
	}
	return TRUE;
}


/*	Request exclusive access.

This function uses a semaphore for granting mutually exclusive access to a
code section between mexEnter()/mexLeave(). If the semaphore is locked, the
function waits up to 10ms before giving up.

The semaphore is created once upon initialization of the library and persists
until the library is unloaded. Code that may raise an exception or lead to a
MEX abort on error must not execute within a mexEnter()/mexLeave section,
otherwise the access may lock permanently.
*/
#ifndef NDEBUG
void mexEnter(const char* file, const int line)
#else
void mexEnter(void)
#endif
{
	switch (WaitForSingleObject(driver, 10))
	{
	case WAIT_ABANDONED:
	case WAIT_OBJECT_0:					// access granted
		return;
	default:
#ifndef NDEBUG
		mexPrintf("%s:%d - ", file, line);
#endif
		mexErrMsgTxt("Locked.");
	}
}


/*	Release exclusive access.
*/
void mexLeave(void)
{
	ReleaseSemaphore(driver, 1, NULL);
}


/*	Check for an error and print the message.

This function is save within a mexEnter()/mexLeave section.
*/
#ifndef NDEBUG
void mexMessage(const char* file, const int line, long i32Status)
#else
void mexMessage(long i32Status)
#endif
{
	if (i32Status == false)
	{
		MEXLEAVE;

		TCHAR	szErrorString[StringLength];
		long res;
		res = GetLastErrorMsg(szErrorString, StringLength);

		size_t   i;
		char cErrorString[StringLength];
		wcstombs_s(&i, cErrorString, (size_t)StringLength, szErrorString, (size_t)StringLength);

#ifndef NDEBUG
		mexPrintf("%s:%d ", file, line);
#endif

		mexErrMsgTxt(cErrorString);
	}
}


/*	Free the device and the driver.
*/
void mexCleanup(void)
{
	while (mph16s > 0)
	{
		mph16 = mph16s;
		MEXMESSAGE(SelectDevice(mph16 - 1));
		MEXMESSAGE(TeardownDevice());
#ifndef NDEBUG
		mexPrintf("Thorlabs MPH16 connection shutted down!\n");
#endif
		mph16s--;
	}
	mph16s = -1;
	mph16 = -1;
}


/*	Initialize device driver and get devices.
*/
long mexStartup(void)
{
	mexAtExit(mexCleanup);
	if (mph16s < 0)
	{
		long lres;
		lres = FindDevices(mph16s);
		if (lres && mph16s)
		{
			mph16 = 1; // select 1st pinhole
			MEXMESSAGE(SelectDevice(mph16 - 1));

			MEXMESSAGE(SetParam(PARAM_PINHOLE_POS, static_cast<double> (0)));
			//long alignment_pos = 3600;
			//MEXMESSAGE(SetParam(PARAM_PINHOLE_ALIGNMENT_POS, static_cast<double>(alignment_pos)));
			long alignment_mode = 0;
			MEXMESSAGE(SetParam(PARAM_PINHOLE_ALIGNMENT_MODE, static_cast<double>(alignment_mode)));

			long paramType, paramAvailable, paramReadOnly;
			double paramMin, paramMax, paramDefault;
			GetParamInfo(PARAM_PINHOLE_POS, paramType, paramAvailable, paramReadOnly, paramMin, paramMax, paramDefault);
			pos_limits[0] = paramMin;
			pos_limits[1] = paramMax;
		}
		return mph16s;
	}
	return -1;
}


MEXFUNCTION_LINKAGE
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	if (nrhs == 0 && nlhs == 0)
	{
		mexPrintf("\nThorlabs MPH16 motorized pinhole interface.\n\n\tAndriy Chmyrov © 18.03.2019\n\n");
		return;
	}

	if (driver == NULL) mexErrMsgTxt("Semaphore not initialized.");

	if (mph16 == -1)
	{
		if (mexStartup() <= 0)
		{
			VALUE = mxCreateDoubleScalar(static_cast<double>(mph16s));
			return;
		}
	}

	int n = 0;
	while (n < nrhs)
	{
		SHORT index;
		int field;
		switch (mxGetClassID(prhs[n]))
		{
		default:
			mexErrMsgTxt("Parameter name expected as string.");
		case mxCHAR_CLASS:
		{
			char read[StringLength];
			if (mxGetString(prhs[n], read, StringLength)) mexErrMsgTxt("Unknown parameter.");
			if (++n < nrhs)
			{
				if (_stricmp("param_info", read) == 0)
				{
					VALUE = getParameter(read,prhs[n]);
					return;
				}

				setParameter(read, prhs[n]);
				break;
			}
			if (nlhs > 1) mexErrMsgTxt("Too many output arguments.");
			VALUE = getParameter(read);
			return;
		}
		case mxSTRUCT_CLASS:
			for (index = 0; index < static_cast<int>(mxGetNumberOfElements(prhs[n])); index++)
				for (field = 0; field < mxGetNumberOfFields(prhs[n]); field++)
					setParameter(mxGetFieldNameByNumber(prhs[n], field), mxGetFieldByNumber(prhs[n], index, field));
			;
		}
		n++;
	}
	switch (nlhs)
	{
	default:
		mexErrMsgTxt("Too many output arguments.");
	case 1:
		VALUE = mxCreateDoubleScalar(static_cast<double>(mph16));
	case 0:;
	}

}
