#include "epdk.h"

char *IntegerToAscii(__u32 num, __s8 pointSeat, __s8 *buf, __u8 bufSize)
{
	__bool	pointWrite_flag;
	__s8		ch;

	if(buf == NULL || bufSize == 0)
	{
		return NULL;
	}

	buf[--bufSize] = '\0';

	if(num == 0)
	{
		buf[--bufSize] = '0';
	}
	else
	{
		if(pointSeat > 0)
		{
			pointWrite_flag = 0;
		}

		while(bufSize)
		{
			if(pointSeat >= 0)
			{
				pointSeat--;
			}

			ch = num % 10 + '0';

			if((ch != '0') || (pointSeat < 0))
			{
				pointWrite_flag = 1;
			}

			if(pointWrite_flag)
			{
				buf[--bufSize] = ch;	// ����λ�洢�����������

				if(pointSeat == 0)
				{
					buf[--bufSize] = '.';
				}
			}

			num /= 10;

			if((num == 0) && (pointSeat < 0))
			{
				break;  // ��Чλ��д��������Ϊ0
			}
		}
	}

	return &buf[bufSize];
}
