/*
 * irrigazione.h - Garden irrigation management module (irrigazione is "garden irrigation" in Italian)
 *  Carlo Amaglio, Via Emigli 10 - 25081 Bedizzole (Italy)
 * 
 * Released into the public domain
 */

#ifndef IRRIGAZIONE_H
#define IRRIGAZIONE_H

extern void irrigazioneInit();
extern void irrigazioneLoop();
extern void irrigazioneShowSummary();

extern void irrigazioneShow();
extern void irrigazioneSelect();

extern void irrigazioneOutStateShow(int n);
extern void irrigazioneOutState00Show(void);
extern void irrigazioneOutState01Show(void);
extern void irrigazioneOutState02Show(void);
extern void irrigazioneOutState03Show(void);
extern void irrigazioneOutState04Show(void);
extern void irrigazioneOutState05Show(void);

extern void irrigazioneOutStateSelect(int n);
extern void irrigazioneOutState00Select(void);
extern void irrigazioneOutState01Select(void);
extern void irrigazioneOutState02Select(void);
extern void irrigazioneOutState03Select(void);
extern void irrigazioneOutState04Select(void);
extern void irrigazioneOutState05Select(void);

extern void irrigazioneTimerDetailShow(int n);
extern void irrigazioneTimerDetail00Show(void);
extern void irrigazioneTimerDetail01Show(void);
extern void irrigazioneTimerDetail02Show(void);
extern void irrigazioneTimerDetail03Show(void);
extern void irrigazioneTimerDetail04Show(void);
extern void irrigazioneTimerDetail05Show(void);
extern void irrigazioneTimerDetail06Show(void);
extern void irrigazioneTimerDetail07Show(void);
extern void irrigazioneTimerDetail08Show(void);
extern void irrigazioneTimerDetail09Show(void);

extern void irrigazioneTimerDetailSelect(int n);
extern void irrigazioneTimerDetail00Select(void);
extern void irrigazioneTimerDetail01Select(void);
extern void irrigazioneTimerDetail02Select(void);
extern void irrigazioneTimerDetail03Select(void);
extern void irrigazioneTimerDetail04Select(void);
extern void irrigazioneTimerDetail05Select(void);
extern void irrigazioneTimerDetail06Select(void);
extern void irrigazioneTimerDetail07Select(void);
extern void irrigazioneTimerDetail08Select(void);
extern void irrigazioneTimerDetail09Select(void);

#endif
