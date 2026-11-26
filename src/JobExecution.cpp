#include <JobScheduler/JobExecution.h>
#include <JobScheduler/EErrorCode.h>
#include <JobScheduler/Error.h>

Result<Success> JobExecuter::RunJobSafely(Job& job)
{
    try
    {
        return job.Invoke();
    } catch (const std::exception& e)
    {
        return Error(EErrorCode::Exception, e.what());
    }
    catch (...)
    {
        return Error(EErrorCode::Exception, "Unknown exception");
    }
}
