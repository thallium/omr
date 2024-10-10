#if defined (LINUX)
#define _GNU_SOURCE
#include <sys/resource.h>
#endif /* defined (LINUX) */

#include <stdlib.h>

#include "omrport.h"
#include "omrportpriv.h"
#include "omrthread.h"

int32_t omrthread_get_self_thread_time(struct OMRPortLibrary *portLibrary, omrthread_thread_time_t *threadTime) {
	omrthread_t self = omrthread_self();

	int64_t cpuTime = omrthread_get_cpu_time(self);
	if (-1 == cpuTime) {
		return -1;
	}

 #if defined (LINUX)
	struct rusage rUsage = {0};
	int64_t userTime = -1;
	int rc = getrusage(RUSAGE_THREAD, &rUsage);
	if (0 == rc) {
		userTime = (SEC_TO_NANO_CONVERSION_CONSTANT * (int64_t)rUsage.ru_utime.tv_sec) +
				   (MICRO_TO_NANO_CONVERSION_CONSTANT * (int64_t)rUsage.ru_utime.tv_usec);
	} else {
		userTime = rc;
	}
#else /* defined (LINUX) */
	int64_t userTime = omrthread_get_user_time(self);
#endif /* defined (LINUX) */

	if (-1 == userTime) {
		return -1;
	}

	threadTime->sysTime = cpuTime - userTime;
	threadTime->userTime = userTime;
	return 0;
}
