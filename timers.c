/*
 * timers.c
 *
 *  Created on: Apr 17, 2024
 *      Author: mrcloutier
 */

#include "peripherals.h"

void displayTime(long unsigned int inTime){

    int dispSize = 4;

    char secondsChar[4] = {' '};
    char minutesChar[4] = {' '};
    char hoursChar[4] = {' '};
    char daysChar[4] = {' '};
    char monthsChar[4] = {' '};

    char disp[17] = {' '};
    int dispDispSize = 17;


    int seconds = 0;
    int minutes = 0;
    int hours = 0;
    int days = 0;
    int months = 0;

    /*float calcSeconds = 0;
    float calcMinutes = 0;
    float calcHours = 0;
    float calcDays = 0;
    float calcMonths = 0;

    calcSeconds = (double)inTime;
    calcMinutes = calcSeconds / 60;
    calcHours = calcMinutes / 60;
    calcDays = calcHours / 24;
    calcMonths = calcDays / 30;*/

    seconds = inTime % 60;
    minutes = (inTime / 60) % 60;
    hours = ((inTime / 60)/60) % 24;
    days = (((inTime / 60)/60) / 24) + 1;
    if(days > 365){
        days = days - 365;
    }
    float calcDays = days;

    secondsChar[2] = (seconds % 10) + 48;
    secondsChar[1] = (seconds / 10) + 48;
    minutesChar[2] = (minutes % 10) + 48;
    minutesChar[1] = (minutes / 10) + 48;
    hoursChar[2] = (hours % 10) + 48;
    hoursChar[1] = (hours / 10) + 48;
    daysChar[2] = (days % 10) + 48;
    daysChar[1] = (days / 10) + 48;
    monthsChar[2] = (months % 10) + 48;
    monthsChar[1] = (months / 10) + 48;

    double dayAdjust = 1;

    if(calcDays < 31 + dayAdjust){
        disp[1] = 'J';
        disp[2] = 'A';
        disp[3] = 'N';
    }

    else if(calcDays < 59 + dayAdjust){
            disp[1] = 'F';
            disp[2] = 'E';
            disp[3] = 'B';
            days = days - 31;
        }
    else if(calcDays < 90 + dayAdjust){
                disp[1] = 'M';
                disp[2] = 'A';
                disp[3] = 'R';
                days = days - 59;
            }
    else if(calcDays < 120 + dayAdjust){
            disp[1] = 'A';
            disp[2] = 'P';
            disp[3] = 'R';
            days = days - 90;
        }
    else if(calcDays < 151 + dayAdjust){
            disp[1] = 'M';
            disp[2] = 'A';
            disp[3] = 'Y';
            days = days - 120;
        }
    else if(calcDays < 181 + dayAdjust){
                disp[1] = 'J';
                disp[2] = 'U';
                disp[3] = 'N';
                days = days - 151;
            }
    else if(calcDays < 212 + dayAdjust){
                disp[1] = 'J';
                disp[2] = 'U';
                disp[3] = 'L';
                days = days - 181;
            }
    else if(calcDays < 243 + dayAdjust){
                disp[1] = 'A';
                disp[2] = 'U';
                disp[3] = 'G';
                days = days - 212;
            }
    else if(calcDays < 273 + dayAdjust){
                disp[1] = 'S';
                disp[2] = 'E';
                disp[3] = 'P';
                days = days - 243;
            }
    else if(calcDays < 304 + dayAdjust){
                disp[1] = 'O';
                disp[2] = 'C';
                disp[3] = 'T';
                days = days - 273;
            }
    else if(calcDays < 334 + dayAdjust){
                disp[1] = 'N';
                disp[2] = 'O';
                disp[3] = 'V';
                days = days - 304;
            }
    else if(calcDays < 365 + dayAdjust){
                disp[1] = 'D';
                disp[2] = 'E';
                disp[3] = 'C';
                days = days - 334;
            }
    else{
               days = days - 365;
            }

    daysChar[2] = (days % 10) + 48;
    daysChar[1] = (days / 10) + 48;

    disp[4] = ' ';
    disp[5] = daysChar[1];
    disp[6] = daysChar[2];
    disp[7] = ' ';
    disp[8] = hoursChar[1];
    disp[9] = hoursChar[2];
    disp[10] = ':';
    disp[11] = minutesChar[1];
    disp[12] = minutesChar[2];
    disp[13] = ':';
    disp[14] = secondsChar[1];
    disp[15] = secondsChar[2];

    /*Graphics_clearDisplay(&g_sContext);

    Graphics_drawStringCentered(&g_sContext, secondsChar, dispSize, 48, 15, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, minutesChar, dispSize, 48, 25, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, hoursChar, dispSize, 48, 35, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, daysChar, dispSize, 48, 45, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, monthsChar, dispSize, 48, 55, TRANSPARENT_TEXT);

    Graphics_flushBuffer(&g_sContext);*/


    Graphics_drawStringCentered(&g_sContext, disp, dispDispSize, 48, 15, TRANSPARENT_TEXT);

    Graphics_flushBuffer(&g_sContext);

}

void displayTemp(float inAvgTempC){

    char dispTempC[7] = {' '};
    char dispTempF[7] = {' '};

    int dispSize = 7;

    float tempC = inAvgTempC;
    float tempF = (tempC * 1.8) + 32;

    int cel = (int)(tempC * 10);
    int fah = (int)(tempF * 10);

    dispTempC[1] = (cel / 100) + 48;
    dispTempC[2] = ( (cel / 10) % 10) + 48;
    dispTempC[3] = 46;
    dispTempC[4] = (cel % 10) + 48;

    dispTempF[1] = (fah / 100) + 48;
    dispTempF[2] = ((fah / 10) % 10) + 48;
    dispTempF[3] = 46;
    dispTempF[4] = (fah % 10) + 48;

    Graphics_drawStringCentered(&g_sContext, dispTempC, dispSize, 48, 25, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, dispTempF, dispSize, 48, 35, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "C", AUTO_STRING_LENGTH, 75, 25, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "F", AUTO_STRING_LENGTH, 75, 35, TRANSPARENT_TEXT);

    Graphics_flushBuffer(&g_sContext);
}



