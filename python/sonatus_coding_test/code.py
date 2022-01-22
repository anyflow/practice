import time
import random
from functools import reduce
from concurrent.futures import ThreadPoolExecutor, as_completed


class Job:
    def __init__(self, job_id: int, actual_task, task_params, dependent_jobs: list):
        self.job_id = job_id
        self.duration = -1
        self.status = 'unexecuted'
        self.dependent_jobs = dependent_jobs
        self.actual_task = actual_task
        self.task_params = task_params

    def execute(self):
        start_time = time.time()

        def finalize(status: str) -> None:
            self.status = status
            self.duration = time.time() - start_time
            return self

        if len([j for j in self.dependent_jobs if j.status == 'unexecuted']) > 0:
            raise Exception('Something wrong with arrange()')

        skip = reduce(
            lambda acc, cur: acc | (cur.status in ['failed']),
            self.dependent_jobs,
            False,
        )

        return finalize(
            ('completed' if (self.actual_task)(self.task_params) else 'failed')
            if not skip
            else 'skipped'
        )

    def report(self):
        print(
            '%-6d | %-18d | %-7s | %-s'
            % (
                self.job_id,
                int(self.duration),
                'true' if self.status in ['completed', 'skipped'] else 'false ',
                [j.job_id for j in self.dependent_jobs],
            )
        )


class Scheduler:
    def __init__(self):
        self.report_continue = True

    def __report_periodically(self, executor: ThreadPoolExecutor, jobs: list):
        REPORT_PERIOD = 0.5

        def report():
            print('---------------------------------------------------------')
            print('job id | duration (seconds) | success | dependent job ids')

            [j.report() for j in jobs if j.status != 'unexecuted']

            print('expected final output:')

            report = {'completed': [], 'failed': [], 'skipped': [], 'unexecuted': []}
            [report[j.status].append(j.job_id) for j in jobs]

            [print('  %10s jobs: %s' % (k, v)) for k, v in report.items()]

        def task():
            while True:
                report()

                if not self.report_continue:
                    return

                time.sleep(REPORT_PERIOD)

        executor.submit(task)

    def __execute_in_nonblocking(self, executor: ThreadPoolExecutor, jobs: list):
        tasks = [executor.submit(job.execute) for job in jobs]
        [future.result() for future in as_completed(tasks)]

    def __arrange(self, jobs: list):
        '''Returns jobs list arranged by execution order'''
        ret = []

        iterateds = set()
        target = set(jobs)

        while len(target) > 0:
            extacteds = [
                j for j in target if len(set(j.dependent_jobs) - iterateds) == 0
            ]

            ret.append(extacteds)

            target = target - set(extacteds)
            iterateds = iterateds.union(extacteds)

        return ret

    def __has_cycle(self, jobs):
        def traverse(target, job, has_cycle):
            if job.job_id == target.job_id:
                return True

            for j in job.dependent_jobs:
                has_cycle |= traverse(target, j, has_cycle)

            return has_cycle

        for job in jobs:
            for j in job.dependent_jobs:
                if traverse(job, j, False):
                    return True

        return False

    def run(self, unarranged_jobs: list):
        print('---------------------------------------------------------')
        print(f'jobs list : {len(unarranged_jobs)}')

        # bonus point : detect cycle
        if self.__has_cycle(unarranged_jobs):
            print('cyclic dependencies: true')
            return

        # 1. Checks what jobs are ready to run based on the dependency relationship. Start those jobs.
        arrangeds = self.__arrange(unarranged_jobs)

        MAX_WORKER_COUNT = 10

        with ThreadPoolExecutor(max_workers=MAX_WORKER_COUNT) as executor:
            # 3. The job scheduler should periodically report the completed jobs and the jobs' completion status (success or failure).
            self.__report_periodically(executor, unarranged_jobs)

            # 2. Job execution must not block the job scheduler.
            [self.__execute_in_nonblocking(executor, jobs) for jobs in arrangeds]

            self.report_continue = False

        # 4. The job scheduler should exit after all the jobs have either completed or skipped due to failure of dependency jobs.
        return


def actual_task(params) -> bool:
    if params:
        (sleep_time, success) = params
    else:
        sleep_time = random.choice([1, 2])
        success = random.choice([True, False])
    time.sleep(sleep_time)

    return success


def testcase0():
    ret = [Job(i, actual_task, (1, True), []) for i in range(0, 2)]

    ret[1].task_params = (2, True)

    return ret


def testcase1():
    ret = [Job(i, actual_task, (1, True), []) for i in range(0, 4)]

    ret[2].dependent_jobs = [ret[0], ret[1]]
    ret[3].dependent_jobs = [ret[0], ret[2]]

    return ret


def testcase2():
    ret = [Job(i, actual_task, (1, True), []) for i in range(0, 4)]

    ret[1].task_params = (2, True)
    ret[2].task_params = (1, False)
    ret[2].dependent_jobs = [ret[0], ret[1]]
    ret[3].dependent_jobs = [ret[0], ret[2]]

    return ret


def testcase3():
    ret = [Job(i, actual_task, None, []) for i in range(0, 15)]

    ret[0].dependent_jobs = [ret[1], ret[2], ret[3]]
    ret[1].dependent_jobs = [ret[4], ret[5], ret[6]]
    ret[2].dependent_jobs = [ret[7], ret[8], ret[9]]

    return ret


def testcase4():
    ret = [Job(i, actual_task, None, []) for i in range(0, 10)]

    ret[0].dependent_jobs = []
    ret[1].dependent_jobs = [ret[0]]
    ret[2].dependent_jobs = [ret[0]]
    ret[3].dependent_jobs = [ret[1]]
    ret[4].dependent_jobs = [ret[1]]
    ret[5].dependent_jobs = [ret[1], ret[6]]
    ret[6].dependent_jobs = [ret[2]]
    ret[7].dependent_jobs = [ret[5], ret[6], ret[8]]
    ret[8].dependent_jobs = [ret[6]]
    ret[9].dependent_jobs = [ret[6]]

    return ret


def testcase_cycle():
    ret = [Job(i, actual_task, None, []) for i in range(0, 15)]

    ret[2].dependent_jobs = [ret[1]]
    ret[3].dependent_jobs = [ret[2], ret[4]]
    ret[4].dependent_jobs = [ret[3]]

    return ret


# Scheduler().run(testcase0())
# Scheduler().run(testcase1())
# Scheduler().run(testcase2())
# Scheduler().run(testcase_cycle())
# Scheduler().run(testcase3())
# Scheduler().run(testcase4())
