#include "StdAfx.h"
#include "KLineCollection.h"
#include "TickReader.h"


KLineCollection::KLineCollection(void)
{

}

KLineCollection::~KLineCollection(void)
{

}

void KLineCollection::GetPriceVolRange(int nStartIdx, int nEndIdx, int& nHighPr, int& nLowPr, int& nMaxVol)
{
	int high, low, maxvol;

	for(int i = nStartIdx; i <= nEndIdx; i++)
	{
		KLine kline = (*this)[i];
		
		if(i == nStartIdx)
		{
			high = kline.high;
			low = kline.low;
			maxvol = kline.vol;
		}
		else
		{
			if(kline.high > high) high = kline.high;
			if(kline.low < low) low = kline.low;
			if(kline.vol > maxvol) maxvol = kline.vol;
		}
	}

	nHighPr = high;
	nLowPr = low;
	nMaxVol = maxvol;
}

void KLineCollection::Generate(TickCollection& ticks, int seconds, KLine prevDayLine)
{
	int nLastSecond;

	/* ����K�ߵ���ֵ */
	int kOpen, kClose, kHigh, kLow, kVol;
	float totalPrice = 0, totalVol = 0, kCount = 0;

	/* ���챾��Լ����K */
	push_back(prevDayLine);

	kOpen = kClose = kHigh = kLow = ticks[0].price;
	nLastSecond = ticks[0].time;
	kVol = 0;

	// ��K����ǰ�յ���K�����ܼ�����ܳɽ���

	for(int i = 1; i < ticks.size(); i++)
	{
		int nCurSecond = ticks[i].time;
		int price = ticks[i].price;
		int vol = ticks[i].vol;

		totalPrice += (price*vol);
		totalVol += vol;

		if((nCurSecond / seconds) != (nLastSecond / seconds))
		{
			/* K ����ɣ�д���ļ� */ 
			KLine kline;

			kline.time = nCurSecond;
			kline.high = kHigh;
			kline.low = kLow;
			kline.open = kOpen;
			kline.close = kClose;
			kline.vol = kVol;

			//	���������
			kline.avg = totalPrice / totalVol;

			push_back(kline);

			kCount++;

			/* ����K�� */
			kOpen = kClose = kHigh = kLow = price;
			kVol = vol;
		}
		else
		{
			if(price > kHigh) kHigh = price;
			if(price < kLow) kLow = price;
			kClose = price;
			kVol += vol;
		}

		nLastSecond = nCurSecond;		
	}
}