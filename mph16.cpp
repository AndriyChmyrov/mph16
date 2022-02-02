// mph16.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "mph16.h"

/*	Selected pinhole.
*/
long mph16 = -1;

/*	Number of available microscopes or -1 if driver not initialized.
*/
long mph16s = -1;

/* Limits of the position index parameter
*/
double pos_limits[2] = {0, 0};

/*	Check for and read a scalar value.
*/
double getScalar(const mxArray* array)
{
	if (!mxIsNumeric(array) || mxGetNumberOfElements(array) != 1) mexErrMsgTxt("Not a scalar.");
	return mxGetScalar(array);
}


/*	Check for and read a matrix
*/
double* getArray(const mxArray* array, int size)
{
	if (!mxIsNumeric(array) || mxGetNumberOfElements(array) != size) mexErrMsgTxt("Supplied wrong number of elements in array.");
	return mxGetPr(array);
}


/*	Get a parameter or status value.
*/
mxArray* getParameter(const char* name, const mxArray* field)
{
	if (_stricmp("alignment_mode", name) == 0)
	{
		double dParam;
		MEXMESSAGE(GetParam(PARAM_PINHOLE_ALIGNMENT_MODE, dParam));
		return mxCreateDoubleScalar(dParam);
	}

	if (_stricmp("alignment_position", name) == 0)
	{
//		MEXMESSAGE(SetParam(PARAM_PINHOLE_ALIGNMENT_MODE, MPH16_ALIGNMENT_MODE_ON));
		double dPos;
		MEXMESSAGE(GetParam(PARAM_PINHOLE_ALIGNMENT_POS_CURRENT, dPos));
		//MEXMESSAGE(GetParam(PARAM_PINHOLE_ALIGNMENT_POS, dPos));
//		MEXMESSAGE(SetParam(PARAM_PINHOLE_ALIGNMENT_MODE, MPH16_ALIGNMENT_MODE_OFF));
		return mxCreateDoubleScalar(dPos);
	}

	if (_stricmp("close", name) == 0)
	{
		mexCleanup();
		return mxCreateDoubleScalar(1);
	}

	if (_stricmp("mph16", name) == 0)
	{
		return mxCreateDoubleScalar(static_cast<double>(mph16));
	}

	if (_stricmp("mph16s", name) == 0)
	{
		return mxCreateDoubleScalar(static_cast<double>(mph16s));
	}

	if (_stricmp("param_info", name) == 0)
	{
		double paramID = getScalar(field);

		long paramType, paramAvailable, paramReadOnly;
		double paramMin, paramMax, paramDefault;
		GetParamInfo(static_cast<long>(paramID), paramType, paramAvailable, paramReadOnly, paramMin, paramMax, paramDefault);

		mxArray* mxRes;
		int nfields = 6;
		const char *field_names[] = { "type", "available", "readOnly", "min", "max", "default" };
		mxRes = mxCreateStructMatrix(1, 1, nfields, field_names);

		for (int ifield = 0; ifield < nfields; ifield++)
		{
			mxArray *pvalue = NULL;
			mwSize dims[2] = { 1, 1 };
			switch (ifield)
			{
			case 0: // "type"
				if (TYPE_LONG == paramType)
					pvalue = mxCreateString("int32");
				else if (TYPE_DOUBLE == paramType)
					pvalue = mxCreateString("double");
				else
					pvalue = mxCreateString("error!");
				break;
			case 1: // "available"
				pvalue = mxCreateNumericArray(2, dims, mxINT32_CLASS, mxREAL);
				*mxGetPr(pvalue) = paramAvailable;
				break;
			case 2: // "readOnly"
				pvalue = mxCreateNumericArray(2, dims, mxINT32_CLASS, mxREAL);
				*mxGetPr(pvalue) = paramReadOnly;
				break;
			case 3: // "min"
				pvalue = mxCreateDoubleScalar(paramMin);
				break;
			case 4: // "max"
				pvalue = mxCreateDoubleScalar(paramMax);
				break;
			case 5: // "default"
				pvalue = mxCreateDoubleScalar(paramDefault);
				break;
			}
			mxSetFieldByNumber(mxRes, 0, ifield, pvalue);
		}

		return mxRes;
	}

	if (_stricmp("position", name) == 0)
	{
		double dPos;
		MEXMESSAGE(GetParam(PARAM_PINHOLE_POS_CURRENT, dPos));
		return mxCreateDoubleScalar(dPos);
	}

#ifndef NDEBUG
	mexPrintf("%s:%d - ", __FILE__, __LINE__);
#endif
	mexPrintf("\"%s\" unknown.\n", name);
	return NULL;
}


/*	Set a measurement parameter.
*/
void setParameter(const char* name, const mxArray* field)
{
	if (mxGetNumberOfElements(field) < 1) return;
	if (_stricmp("mph16", name) == 0) return;
	if (_stricmp("mph16s", name) == 0) return;
	if (mph16 < 1) mexErrMsgTxt("Invalid laser handle.");

	if (_stricmp("alignment_mode", name) == 0)
	{
		double dParam = getScalar(field);
		MEXMESSAGE(SetParam(PARAM_PINHOLE_ALIGNMENT_MODE, dParam));
		return;
	}

	if (_stricmp("position", name) == 0)
	{
		double dParam = getScalar(field);
		if (dParam < pos_limits[0] || dParam > pos_limits[1])
		{
			mexPrintf("Requested position is outside of the limits [%.0f;%.0f]!\n", pos_limits[0], pos_limits[1]);
			return;
		}
		MEXMESSAGE(PreflightPosition());
		MEXMESSAGE(SetParam(PARAM_PINHOLE_POS, dParam));
		MEXMESSAGE(SetupPosition());
		MEXMESSAGE(StartPosition());
		long status = STATUS_READY;
		do
		{
			if (false == StatusPosition(status))
			{
				return;
			}
			if (utIsInterruptPending()) {        // check for a Ctrl-C event
				mexPrintf("Ctrl-C Detected. Exiting from gagedaq MEX function!\n");
				mexPrintf("Current status = %d.\n\n", status);
				return;
			}
		} while (status == STATUS_BUSY);
		MEXMESSAGE(PostflightPosition());
		return;
	}

#ifndef NDEBUG
	mexPrintf("%s:%d - ", __FILE__, __LINE__);
#endif
	mexPrintf("\"%s\" unknown.\n", name);
	return;
}
