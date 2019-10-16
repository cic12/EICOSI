#ifndef MOTOR_H_
#define MOTOR_H_

#include <iostream>
#include "Definitions.h"
#include "mpcThread.h"

#include "hebi.h"
#include "lookup.hpp"
#include "group_command.hpp"
#include "group_feedback.hpp"
#include "log_file.hpp"

extern double demandedCurrent;
extern short inputCurrent;
extern double currentPosition, homePosition, previousPosition;
extern testParams test0;

extern bool mpc_initialised;
extern bool mpc_complete;
extern double motor_comms_count;

void enableDevice();
void disableDevice();
void openDevice();
void closeDevice();
void getOperationMode();
void currentMode();
void setCurrent(short TargetCurrent);
void getCurrentPosition(long& CurrentPosition);
void getCurrentVelocity(long& CurrentVelocity);
void definePosition(long& HomePosition);

#endif