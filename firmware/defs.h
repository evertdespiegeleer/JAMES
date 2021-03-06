//Development
#define tetheredMode true
#define maxAbsoluteAngle 2 //Max absolute roll and pitch angle before disarming into mode 23 (2 rad = 114 deg)

//COM
#define serialTimeout 20

//SIGNALIZATION
#define signLed1 A2 //Corresponds to L7
#define signLed3 A3 //Corresponds to L8
#define signLed9 A1 //Corresponds to L6
#define blinkPeriod 200 //ms

//BATTERY
#define battery A0
#define analogReadingVoltageFactor 0.02102 // = 1 / 1023 * 5V * ( ( 10kO + 33kO ) / 10 kO )

//ESCs
#define esc1 9
#define esc2 6
#define esc3 5
#define esc4 3
#define disArmedMicroSeconds 500
#define minMicroSeconds 800
#define minMicroSecondsMotorsTurning 870
#define maxMicroSeconds 1800

//PIDs
#define pidSampleTime 20 //ms
#define PIDerrorRateLPFFactor 0.9

//HOP
#define hopEngineSpinupTime 500 //ms ==> When performing a hop, this variable indicated how long the engines should spin up to minMicroSecondsMotorsTurning, before performing the actual hop.

#define reportPerXCycles 12 // Has to be multiple of pingsPerReportCycle
#define pingsPerReportCycle 4 // Report every 12/4 cycles

#define pCX (reportPerXCycles / pingsPerReportCycle)
#define cycleNrOrientationReport 1
#define cycleNrPressureReport 2
#define cycleNrBatteryReport 5
#define cycleNrPIDOutputsReport 7
#define cycleNrESCValsReport 10
