/*****************************************************************************/
/** RCX  *********************************************************************/
/**                                                                         **/

typedef struct RCX {
  Macro** M_macros;
  int nMacros;
  Macro* M_currentMacro;
  int nCurrentMacro;
  void (*turnoff)();
  void (*run)(struct RCX* rcx);
  void (*stop)(struct RCX* rcx);
  struct RCXState* myState;
} RCX;

void RCXexecutor(RCX* rcx)
{
  if (rcx->M_currentMacro != NULL)
  {
    rcx->M_currentMacro->nCurrentCmd = 0;

    while (rcx->M_currentMacro->nCurrentCmd < rcx->M_currentMacro->nCmds)
    {
      AbstractCommand* ac =
        rcx->M_currentMacro->AC_cmds[rcx->M_currentMacro->nCurrentCmd];
      if (ac->type == eCmdGoto)
      {
        GotoCommand* cmd = (GotoCommand*)ac;
        if (cmd->iGoto < rcx->M_currentMacro->nCmds)
          rcx->M_currentMacro->nCurrentCmd = cmd->iGoto;
  		}
      else
      {
        ac->execute(ac);
        ++(rcx->M_currentMacro->nCurrentCmd);
      }
    }

    motor_a_dir(off);
    motor_b_dir(off);
    motor_c_dir(off);
  }
}

int taskRun();
int taskTimer();
int taskButtonRun();
int taskButtonOnOff();
int taskButtonPrgm();
int taskButtonView();

pid_t tRun = -1;
pid_t tTimer = -1;
pid_t tButtonView = -1;
pid_t tButtonRun = -1;
pid_t tButtonOnOff = -1;
pid_t tButtonPrgm = -1;

wakeup_t button_release_wakeup(wakeup_t data);

void RCXrun(RCX* rcx)
{
  tTimer = execi(&taskTimer, 1, DEFAULT_STACK_SIZE);
  tButtonView = execi(&taskButtonView, 1, DEFAULT_STACK_SIZE);
  tButtonRun = execi(&taskButtonRun, 1, DEFAULT_STACK_SIZE);
  tButtonOnOff = execi(&taskButtonOnOff, 1, DEFAULT_STACK_SIZE);
  tButtonPrgm = execi(&taskButtonPrgm, 1, DEFAULT_STACK_SIZE);
}

void RCXstop(RCX* rcx)
{
  if (tTimer != -1)
  {
    kill(tTimer);
    tTimer = -1;
  }
  if (tRun != -1)
  {
    kill(tRun);
    tRun = -1;
  }
  if (tButtonView != -1)
  {
    kill(tButtonView);
    tButtonView = -1;
  }
  if (tButtonPrgm != -1)
  {
    kill(tButtonPrgm);
    tButtonPrgm = -1;
  }
  if (tButtonRun != -1)
  {
    kill(tButtonRun);
    tButtonRun = -1;
  }
  if (tButtonOnOff != -1)
  {
    kill(tButtonOnOff);
    tButtonOnOff = -1;
  }
}

void RCXturnoff()
{
  extern RCX MyRCX;
  MyRCX.stop(&MyRCX);
}

void RCX_constructor(RCX* rcx)
{
  rcx->M_macros = (Macro**)malloc(sizeof(Macro*));
  rcx->nMacros = 1;
  rcx->nCurrentMacro = 1;
  rcx->M_currentMacro = (Macro*)malloc(sizeof(Macro));
  Macro_constructor(rcx->M_currentMacro);
  rcx->M_macros[0] = rcx->M_currentMacro;
  rcx->turnoff = RCXturnoff;
  rcx->run = RCXrun;
  rcx->stop = RCXstop;
  rcx->myState = NULL;
}

/**                                                                         **/
/** RCX **********************************************************************/
/*****************************************************************************/

