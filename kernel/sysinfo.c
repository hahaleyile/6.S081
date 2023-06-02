#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"
#include "defs.h"

uint64
sys_sysinfo(void)
{
  struct sysinfo info;
  uint64 out;
  if (argaddr(0, &out) < 0)
    return -1;

  struct proc *p = myproc();
  uint64 freemem, nproc;
  freemem = get_freemem() * PGSIZE;
  nproc = get_usedproc();
  info.freemem = freemem;
  info.nproc = nproc;

  if (copyout(p->pagetable, out, (char *)&info, sizeof(struct sysinfo)) < 0)
    return -1;
  return 0;
}
