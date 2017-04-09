/* /////////////////////////////// "ReportTime.h" /////////////////////////////////
 *  File: ReportTime.h
 *  Descriptions:  Helper class for time accounting
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/11
 *  Author: Lingyun Tuo
 */

#ifndef REPORTTIME_H_
#define REPORTTIME_H_

#include <time.h>
#include <sys/timeb.h>

class CReportTime
{
private:
	time_t start, end, total;
	int total_1;

	struct timeb tbstart, tbend;
public:
	CReportTime()
	{
		start = end = total = 0;
		total_1 = 0;
	}
	void Start()	// start timer
	{
		time(&start);
		ftime(&tbstart);
	}
	float End()		// end timer and add to total
	{
		int ms;

		time(&end);
		ftime(&tbend);
		total += end - start;
		ms = (tbend.millitm - tbstart.millitm)+(tbend.time - tbstart.time)*1000;
		total_1 += ms;
		return (float)ms/1000.0f;
	}
	int Total()	// total time 
	{
		return total;
	}
	float Total_1()
	{
		return (float)total_1/1000;
	}
};

#endif /*REPORTTIME_H_*/
