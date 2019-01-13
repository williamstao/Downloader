#ifndef __I_PROCESS_ADPTER_H
#define __I_PROCESS_ADPTER_H

#include "../common/common.h"
class IProgressAdapter
{
public:
	virtual ~IProgressAdapter();
	virtual void OnTaskProgress(int pos, int total) = 0;
	virtual bool NeedCancel() = 0;
};

class IMultiDownloaderAdapter
{
public:
	virtual ~IMultiDownloaderAdapter();
	virtual bool NeedCancel() = 0;
	virtual void SetCancel(bool isCancel) = 0;
	virtual int DownloadPlistFile(int index, int offset, int num) = 0;
};

#endif