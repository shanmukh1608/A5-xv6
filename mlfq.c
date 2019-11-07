#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

struct proc *procQueue[5][NPROC + 1];
int head[5];
int tail[5];
int size[5];

void addToQueue(struct proc *p)
{
    // if (p != 0)
        // printf("%d %d %d\n", p->state, p->currentQueue, p->pid);
    
    if (!size[p->currentQueue]) //queue is empty
        head[p->currentQueue] = tail[p->currentQueue] = 0;
    else
        tail[p->currentQueue] = (tail[p->currentQueue] + 1) % (NPROC + 1);

    procQueue[p->currentQueue][tail[p->currentQueue]] = p;
    size[p->currentQueue]++;
}

struct proc *removeFromQueue(int currQueue)
{
    if (!size[currQueue])
        return 0;
    struct proc *p = procQueue[currQueue][head[currQueue]];
    head[p->currentQueue] = (head[p->currentQueue] + 1) % (NPROC + 1);
    size[currQueue]--;
    return p;
}

struct proc *nextProcessToRun()
{
    for (int priority = 0; priority < NPRIOR; priority++)
    {
        if (size[priority] == 0)
            continue;
        // cprintf("P=%d", priority);
        // struct proc *p=removeFromQueue(priority);
        // cprintf("Retrieved %d %d %d\n", p->state, p->currentQueue, p->pid);
        // return p;
        return removeFromQueue(priority);
    }

    return 0;
}

void aging()
{
    struct proc *p;
    for (int i = 1; i < NPRIOR; i++)
    {
        for (int j = 0; j < size[i]; j++)
        {
            int index = (head[i] + j);
            index %= (NPROC + 1);
            p = procQueue[i][index];
            if (p != 0)
            {
                if (p->lastScheduledTime + TSTARV <= ticks)
                {
                    // size[i]--;
                    for (int k = j; k != tail[i]; k = (k + 1) % (NPROC + 1))
                        procQueue[i][k] = procQueue[i][k + 1];
                    tail[i] = (tail[i] - 1 + NPROC + 1) % (NPROC + 1);
                    size[i]--;
                    cprintf("process %d promoted from %d at time %d\n", p->pid, i, ticks);
                    if (p->currentQueue > 0)
                        p->currentQueue--;
                    addToQueue(p);
                }
            }
        }
    }
}

// void reAdjust()
// {
//     struct proc *p;
//     for (int i = 1; i <= 4; i++)
//     {
//         int to_insert = front[i];
//         int temp = size[i];
//         for (int j = 0; j < temp; j++)
//         {
//             p = mlfq_queue[i][(front[i] + j) % ((NPROC + 5))];
//             if (should_be_promoted(p))
//             {
//                 size[i]--;
//                 // cprintf("process %d promoted from %d\n", p->pid, i);
//                 promote_and_add_to_mlfq(p);
//             }
//             else
//             {
//                 mlfq_queue[i][to_insert] = p;
//                 to_insert++;
//                 to_insert %= (NPROC + 5);
//             }
//         }
//         rear[i] = (front[i] + size[i] - 1) % (NPROC + 5);
//     }
// }

void displayAll()
{
    for (int i = 0; i < 5; i++)
    {
        if (size[i] == 0)
            continue;
        cprintf("size=%d queue %d: ", size[i], i);
        for (int j = 0; j < size[i]; j++)
        {
            cprintf("%d ", procQueue[i][(head[i] + j) % ((NPROC + 5))]->pid);
        }
        cprintf("\n");
    }
}