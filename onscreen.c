///////////////////////////////////////////////////////////////////////////////
// onscreen.c
//
// "On Screen" programmer for RCX
// version 0.01 Copyright (c) 1999 by David R. Van Wagner ALL RIGHTS RESERVED
//
// allows simple steps to be defined directly on the RCX for test purposes
///////////////////////////////////////////////////////////////////////////////

#include "conio.h"
#include "stdlib.h"
#include "unistd.h"
#include "direct-button.h"
#include "direct-lcd.h"
#include "direct-motor.h"
#include "sys/tm.h"

//#include <malloc.h>
//#include <stdio.h>
//#define NULL ((void*)0)

#define DEBUG(s) //cputs(s),lcd_refresh(),delay(100)

#include "abstractcmd.h"
#include "macro.h"
#include "rcx.h"
#include "cursor.h"
#include "rcxstate.h"

RCX MyRCX;

wakeup_t button_press_wakeup(wakeup_t data) {
  return PRESSED(button_state(),data);
}

wakeup_t button_release_wakeup(wakeup_t data) {
  return RELEASED(button_state(),data);
}

int taskButtonRun()
{
  do
  {
    wait_event(&button_release_wakeup,BUTTON_RUN);  // wait for release
    msleep(200); // debounce
    wait_event(&button_press_wakeup,BUTTON_RUN);    // wait for press
    msleep(200); // debounce

    MyRCX.myState = MyRCX.myState->run(&MyRCX);
  } while (1);
}

int taskButtonView()
{
  do
  {
    wait_event(&button_press_wakeup,BUTTON_VIEW);    // wait for press
    msleep(200); // debounce
    wait_event(&button_release_wakeup,BUTTON_VIEW);  // wait for release
    msleep(200); // debounce

    MyRCX.myState = MyRCX.myState->view(&MyRCX);
  } while (1);
}

int taskButtonPrgm()
{
  do
  {
    wait_event(&button_press_wakeup,BUTTON_PROGRAM);    // wait for press
    msleep(200); // debounce
    wait_event(&button_release_wakeup,BUTTON_PROGRAM);  // wait for release
    msleep(200); // debounce

    MyRCX.myState = MyRCX.myState->prgm(&MyRCX);
  } while (1);
}

int taskButtonOnOff()
{
  do
  {
    wait_event(&button_press_wakeup,BUTTON_ONOFF);    // wait for press
    msleep(200); // debounce
    wait_event(&button_release_wakeup,BUTTON_ONOFF);  // wait for release
    msleep(200); // debounce

    MyRCX.myState = MyRCX.myState->onOff(&MyRCX);
  } while (1);
}

int taskTimer()
{
  do
  {
    msleep(167);
    MyRCX.myState = MyRCX.myState->tick(&MyRCX);
  } while (1);
}

int taskRun()
{
  RCXexecutor(&MyRCX);
  tRun = -1;
  return 0;
}

#define MOTOR_A 1
#define MOTOR_B 2
#define MOTOR_C 4
#define MOTOR_AB (MOTOR_A+MOTOR_B)
#define MOTOR_AC (MOTOR_A+MOTOR_C)
#define MOTOR_BC (MOTOR_B+MOTOR_C)
#define MOTOR_ABC (MOTOR_A+MOTOR_B+MOTOR_C)

#define FWD(m,p) OUTPUT(m,p,m)
#define BACK(m,p) OUTPUT(m,p,m^7)
#define BRAKE(m) OUTPUT(m,-1,m)

void DELAY(int d)
{
  DelayCommand* cmd = (DelayCommand*)malloc(sizeof(DelayCommand));
  DelayCommand_constructor(cmd, d);
  MyRCX.M_currentMacro->add(MyRCX.M_currentMacro, (AbstractCommand*)cmd);
}

void OUTPUT(int iMotors, int iPower, int iFwds)
{
  OutputCommand* cmd = (OutputCommand*)malloc(sizeof(OutputCommand));
  OutputCommand_constructor(cmd, iMotors, iPower, iFwds);
  MyRCX.M_currentMacro->add(MyRCX.M_currentMacro, (AbstractCommand*)cmd);
}

void GOTO(int iGoto)
{
  GotoCommand* cmd = (GotoCommand*)malloc(sizeof(GotoCommand));
  GotoCommand_constructor(cmd, iGoto);
  MyRCX.M_currentMacro->add(MyRCX.M_currentMacro, (AbstractCommand*)cmd);
}

int main()
{
  RCXStateConstructors();

  RCX_constructor(&MyRCX);

  DELAY(20);
  FWD(MOTOR_ABC,8);
  DELAY(20);
  BRAKE(MOTOR_ABC);
  DELAY(20);
  BACK(MOTOR_ABC,8);
  DELAY(20);
  BRAKE(MOTOR_ABC);
  DELAY(20);
  GOTO(0);
  DELAY(0);
  DELAY(0);
  DELAY(0);
  DELAY(0);
  DELAY(0);
  DELAY(0);

  lcd_clear();

  MyRCX.myState = &RCXBrowseState;
  cputs("READY");

  MyRCX.run(&MyRCX);
  RCXCursor.set(-1);

  tm_start();

  return 0;
}

