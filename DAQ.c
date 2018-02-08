#include <NIDAQmxBase.h>
#include <stdio.h>
#include <sys/time.h>

#define DAQmxErrChk(functionCall) { if( DAQmxFailed(error=(functionCall)) ) { goto Error; } }
char filename[] = "Data.csv";
char *file = filename;

struct timeval start_time, end_time;// Creates a structure data type which holds the start and end times of the calculations.


int main(int argc, char *argv[])
{

    FILE *Data;
    Data = fopen(file, "w" );

    int n;
    int m;
    int ii;


   // Task parameters
    int32       error = 0;
    TaskHandle  taskHandle = 0;
    float64     elapsedTime = 0;
    char        errBuff[2048]={'\0'};
    int32       i,j;
    bool32      done=0;
    float     SampleLength = 10;

    // Channel parameters
    //char        chan[] = "Dev1/ai0, Dev1/ai1";
    int         numChannels = 8;
    char        chan[numChannels*10-1];
    float64     min = -10.0;
    float64     max = 10.0;

    // Timing parameters
    char        clockSource[] = "OnboardClock";
    uInt64      samplesPerChan = 10;
    float64     sampleRate = 2000.0;

    // Data read parameters
    #define     bufferSize (uInt32)10
    float64     data[bufferSize * numChannels];
    int32       pointsToRead = bufferSize;
    int32       pointsRead;
    float64     timeout = 10.0;
    int32       totalRead = 0;

    if (argc > 1)
      sscanf(argv[1], "%f", &SampleLength);

    for ( ii = 0; ii < numChannels*10 - 1; ii += 10 )
      {
	chan[ii] = 'D';
	chan[ii+1] = 'e';
	chan[ii+2] = 'v';
	chan[ii+3] = '1';
	chan[ii+4] = '/';
	chan[ii+5] = 'a';
	chan[ii+6] = 'i';
	chan[ii+7] = (ii/10)+48;
	if( ii <= (numChannels-1)*10 )
          {
	    chan[ii+8] = ',';
	    chan[ii+9] = ' ';
          }
      }
    chan[numChannels*10-2] = '\0';
	
    DAQmxErrChk (DAQmxBaseCreateTask("",&taskHandle));
    DAQmxErrChk (DAQmxBaseCreateAIVoltageChan(taskHandle,chan,"",DAQmx_Val_Cfg_Default,min,max,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxBaseCfgSampClkTiming(taskHandle,clockSource,sampleRate,DAQmx_Val_Rising,DAQmx_Val_ContSamps,samplesPerChan));
    DAQmxErrChk (DAQmxBaseCfgInputBuffer(taskHandle,100000*numChannels)); //use a 100,000 sample DMA buffer
    DAQmxErrChk (DAQmxBaseStartTask(taskHandle));

    // The loop will quit after 10 seconds

    gettimeofday(&start_time, NULL);
    while( elapsedTime  < SampleLength ) {
        DAQmxErrChk (DAQmxBaseReadAnalogF64(taskHandle,pointsToRead,timeout,DAQmx_Val_GroupByScanNumber,data,bufferSize*numChannels,&pointsRead,NULL));
	gettimeofday(&end_time, NULL);

	elapsedTime = (end_time.tv_sec + 1.e-6*end_time.tv_usec)-(start_time.tv_sec + 1.e-6*start_time.tv_usec);
	fprintf(Data, "%lf", elapsedTime);

        totalRead += pointsRead;
	for (n = 0; n < numChannels; n++)
	  {
	    float64 runningSum = 0;
	    for (m = n; m < samplesPerChan*numChannels; m += numChannels)
	      {
		runningSum += data[m];
	      }
	runningSum /= samplesPerChan;
	    fprintf(Data, ",%lf", runningSum);
	  }
	fprintf(Data, "\n");
    }
    fclose(Data);
Error:
    if( DAQmxFailed(error) )
        DAQmxBaseGetExtendedErrorInfo(errBuff,2048);
    if(taskHandle != 0) {
        DAQmxBaseStopTask (taskHandle);
        DAQmxBaseClearTask (taskHandle);
    }
    if( DAQmxFailed(error) )
		printf ("DAQmxBase Error %ld: %s\n", error, errBuff);
    return 0;
}
