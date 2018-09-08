#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/types.h>
asmlinkage long sys_set_myFlag (pid_t pid, int flag){
   printk("Start...\n");
    bool process_isFound=false;
    uid_t uid=sys_getuid();
    gid_t gid=sys_getgid();
    struct task_struct *process;

    if(uid==0 && gid==0){ // if the both uid and gid equal to 0, it means this user is root 
        printk("Process has root privileges\n");
        
        if(flag==0 || flag==1){  // arguments are taken correctly 
            printk("Arguments are correct\n");
            process=find_task_by_vpid(pid);
            printk("find_task_by_pid------Process is searchiing...\n");
            if(process==NULL){
                process_isFound=false;
                printk("find_task_by_pid------Process is NOT found...\n");
            }               
            else{
                printk("find_task_by_pid-----Process is found and Process command:%s\n",process->comm);
                process_isFound=true;
                process->myFlag=flag;
                printk("find_task_by_pid------PID %d, myFlag %d\n",process->pid,process->myFlag);
            }
                
      //////////////////////////////////////////////////////////////////////////
                /*
                   for_each_process(process) {
                     printk("on for_each_process....\n ");
                        if(process->pid==pid){
                            printk("in for_each_process, process is found....\n ");
                        process_isFound=true;
                        printk("PID %d, myFlag %d",process->pid,process->myFlag);
                        break;
                    }
                 }  */              
     //////////////////////////////////////////////////////////////////////////
               /* process=next_task(&init_task);   
                for(process;process=!&init_task;next_task(process)){
                     printk("in for loop....\n ");
                    if(process->pid==pid){
                         printk("for loop, process is found....\n ");
                        process_isFound=true;
                        printk("PID %d, myFlag %d",process->pid,process->myFlag);
                        break;
                    }
                }  */
                if (process_isFound== true){ 
                    printk("Last...Process is found...\n");
                    printk("Exit...\n");
                    return 0;
                     }
                else{
                    printk("Last...Process is NOT found...\n");
                    printk("Exit...\n");
                    return -ESRCH;// ESRCH-> errno=3,No such process
                     }
        }
        else{
            printk("EINVAL->errno=22,Invalid argument...\n");
            printk("Exit...\n");
            return -EINVAL; //EINVAL->errno=22,Invalid argument
        }                               
    }
    else{
        printk("EPERM errno->1, Operation not permitted...\n");
        printk("Exit...\n");
        return -EPERM;// EPERM errno->1, Operation not permitted/////return ERR_PTR(-ESRCH); 
    }
    }