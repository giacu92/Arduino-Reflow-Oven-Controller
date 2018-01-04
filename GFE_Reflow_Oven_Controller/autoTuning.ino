void AutoTuneHelper(boolean start)
{
  if(start)
    ATuneModeRemember = reflowOvenPID.GetMode();
  else
    reflowOvenPID.SetMode(ATuneModeRemember);
}
