#ifndef __PCF8563_H__
#define __PCF8563_H__

//PCF8563

#define	CHIP_PCF8563            0xA2
#define REG_SEC                 0x02	    //Seconds
#define REG_MIN                 0x03		//Minutes
#define REG_HOUR                0x04	    //Hours
#define REG_DAY                 0x05		//Days
#define REG_WEEK                0x06	    //Weekdays
#define REG_MON                 0x07		//Months/Century
#define REG_YEAR                0x08		//Years
#define REG_MALM                0x09		//Minute alarm
#define REG_HALM                0x0A		//Hour alarm
#define REG_DALM                0x0B		//Day alarm
#define REG_WALM                0x0C		//Weekday alarm

extern unsigned char Second;
extern unsigned char Minute;
extern unsigned char Hour;

extern void PCF865Read(void);

#endif