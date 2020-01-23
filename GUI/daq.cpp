#include "daq.h"

char errBuff[2048] = { '\0' };
int32 error = 0;
ofstream aiFile;
float64	AIdata[2] = { 0 , 0 }, AIm[2] = { 0 , 0 }, AOdata[2] = { 3.3 , 3.3 }, offset[2] = { -0 , -0 };

struct filter_para {
	double x[3][2];
	double y[3][2];
} low_para1, low_para2, high_para1; // #filter#input; // #filter#input

double lowpass1(double X_in, int emg)
{
	double a1 = -0.043134450881076;
	double a2 = 0.171901801714236;
	double k = 0.282191837708290;

	low_para1.x[0][emg] = X_in;

	low_para1.y[0][emg] = k*(low_para1.x[0][emg] + 2 * low_para1.x[1][emg] + low_para1.x[2][emg]) - a1*low_para1.y[1][emg] - a2*low_para1.y[2][emg];

	for (int i = 2; i>0; i--)
	{
		low_para1.x[i][emg] = low_para1.x[i - 1][emg];
		low_para1.y[i][emg] = low_para1.y[i - 1][emg];
	}
	return low_para1.y[0][emg];
}

double lowpass2(double X_in, int emg)
{
	double a1 = -1.998222847291842;
	double a2 = 0.998224425026401;
	double k = 3.944336396943804e-07;

	low_para2.x[0][emg] = X_in;

	low_para2.y[0][emg] = k*(low_para2.x[0][emg] + 2 * low_para2.x[1][emg] + low_para2.x[2][emg]) - a1*low_para2.y[1][emg] - a2*low_para2.y[2][emg];

	for (int i = 2; i>0; i--)
	{
		low_para2.x[i][emg] = low_para2.x[i - 1][emg];
		low_para2.y[i][emg] = low_para2.y[i - 1][emg];
	}
	return low_para2.y[0][emg];
}

double highpass1(double X_in, int emg)
{
	double a1 = -1.783787708559230;
	double a2 = 0.804982594421399;
	double k = 0.897192575745157;

	high_para1.x[0][emg] = X_in;

	high_para1.y[0][emg] = k * (high_para1.x[0][emg] - 2 * high_para1.x[1][emg] + high_para1.x[2][emg]) - a1 * high_para1.y[1][emg] - a2 * high_para1.y[2][emg];

	for (int i = 2; i > 0; i--)
	{
		high_para1.x[i][emg] = high_para1.x[i - 1][emg];
		high_para1.y[i][emg] = high_para1.y[i - 1][emg];
	}
	return high_para1.y[0][emg];
}

TaskHandle DAQmxAIinit(int32 error, char &errBuff, TaskHandle AItaskHandle, int AIsamplingRate) {

	DAQmxErrChk(DAQmxCreateTask("MMG in", &AItaskHandle));
	DAQmxErrChk(DAQmxCreateAIVoltageChan(AItaskHandle, "Dev1/ai0", "ai0diff", DAQmx_Val_Diff, -0.2, 0.2, DAQmx_Val_Volts, NULL));
	DAQmxErrChk(DAQmxCreateAIVoltageChan(AItaskHandle, "Dev1/ai1", "ai1diff", DAQmx_Val_Diff, -0.2, 0.2, DAQmx_Val_Volts, NULL));
	DAQmxErrChk(DAQmxCfgSampClkTiming(AItaskHandle, "", AIsamplingRate, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 1));
	DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(AItaskHandle, DAQmx_Val_Acquired_Into_Buffer, 1, 0, EveryNCallback, NULL));

Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(&errBuff, 2048);
		DAQmxStopTask(AItaskHandle);
		DAQmxClearTask(AItaskHandle);
		throw errBuff;
	}
	return AItaskHandle;
}

TaskHandle DAQmxAOinit(float64 &AOdata, int32 error, char &errBuff, TaskHandle AOtaskHandle) {

	DAQmxErrChk(DAQmxCreateTask("3V3 Out", &AOtaskHandle));
	DAQmxErrChk(DAQmxCreateAOVoltageChan(AOtaskHandle, "Dev1/ao0", "", -10.0, 10.0, DAQmx_Val_Volts, NULL));
	DAQmxErrChk(DAQmxCreateAOVoltageChan(AOtaskHandle, "Dev1/ao1", "", -10.0, 10.0, DAQmx_Val_Volts, NULL));
	DAQmxErrChk(DAQmxWriteAnalogF64(AOtaskHandle, 1, 1, 10.0, DAQmx_Val_GroupByChannel, &AOdata, NULL, NULL));

Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(&errBuff, 2048);
		DAQmxStopTask(AOtaskHandle);
		DAQmxClearTask(AOtaskHandle);
		throw errBuff;
	}
	return AOtaskHandle;
}

TaskHandle DAQmxAstart(int32 error, char &errBuff, TaskHandle taskHandle)
{
	DAQmxErrChk(DAQmxStartTask(taskHandle));

Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(&errBuff, 2048);
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
		throw errBuff;
	}
	return taskHandle;
}

double clipLimEMG(double emg, double lim) {
	if (abs(emg) > lim) {
		emg = 0;
	}
	return emg;
}

double noiseLimEMG(double emg, double lim) {
	if (abs(emg) < lim) {
		emg = 0;
	}
	return emg;
}

double emgProcess(double AI,int i)
{
	double clip1 = 2;// 0.002;
	double clip2 = 0.02; // 0.00005;
	double clip3 = 1.5;// 0.001;

	return lowpass2(abs(clipLimEMG(noiseLimEMG(lowpass1(clipLimEMG(highpass1((AI + offset[i]),i),clip1),i),clip2),clip3)),i);
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	int32   error = 0;
	char    errBuff[2048] = { '\0' };
	int32   read = 0;

	DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, 1, 10.0, DAQmx_Val_GroupByScanNumber, AIdata, 2, &read, NULL));

	AIm[0] = emgProcess(AIdata[0],0);
	AIm[1] = emgProcess(AIdata[1],1);

	if (read>0) {
		aiFile << AIdata[0] << "," << AIdata[1] << "," << AIm[0] << "," << AIm[1] << "\n";
	}

Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
		throw errBuff;
	}
	return 0;
}