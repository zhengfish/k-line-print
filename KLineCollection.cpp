#include "StdAfx.h"
#include "KLineCollection.h"
#include "TickReader.h"


KLineCollection::KLineCollection(void)
{

}

KLineCollection::~KLineCollection(void)
{

}

KLine KLineCollection::GetKLineByTime(int nTime)
{
	return (*this)[m_mapTime2Idx[nTime]];
}

//	统一接口用于创建索引
void KLineCollection::AddToTail(KLine kline)
{
	m_mapTime2Idx[kline.time] = this->size();
	push_back(kline);
}

void KLineCollection::GetPriceVolRange(int nStartIdx, int nEndIdx, int& nHighPr, int& nLowPr, int& nMaxVol)
{
	int high = 0, low = 0, maxvol = 0;

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

	/* 分钟K线的数值 */
	int kOpen, kClose, kHigh, kLow, kVol;
	float totalPrice = 0, totalVol = 0, kCount = 0;

	/* 昨天本合约的日K */
	AddToTail(prevDayLine);

	kOpen = kClose = kHigh = kLow = ticks[0].price;
	nLastSecond = ticks[0].time;
	kVol = 0;

	// 首K线是前日的日K，不能计算进总成交量

	for(int i = 1; i < ticks.size(); i++)
	{
		int nCurSecond = ticks[i].time;
		int price = ticks[i].price;
		int vol = ticks[i].vol;

		totalPrice += (price*vol);
		totalVol += vol;

		if((nCurSecond / seconds) != (nLastSecond / seconds))
		{
			/* K 已完成，写入文件 */ 
			KLine kline;

			int tmphour, tmpmin, tmpsec;

			//	还原时间
			tmphour = nCurSecond / 3600;
			tmpmin = nCurSecond % 3600 / 60;
			tmpsec = nCurSecond % 3600 % 60;

			kline.time = tmphour * 10000 + tmpmin * 100 + tmpsec;
			kline.high = kHigh;
			kline.low = kLow;
			kline.open = kOpen;
			kline.close = kClose;
			kline.vol = kVol;

			//	计算均价线
			kline.avg = totalPrice / totalVol;

			AddToTail(kline);

			kCount++;

			/* 新起K线 */
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
