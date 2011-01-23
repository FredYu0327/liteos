/* The LiteOS Operating System Kernel */
/*
   The following is the license of LiteOS.
   This file is part of LiteOS.
   Copyright Qing Cao, 2007-2008, University of Illinois , qcao2@uiuc.edu
   LiteOS is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   LiteOS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with LiteOS.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "syscall.h"
#include "../micaz/leds.h"
#include "../../system/threads.h"
#include "../../system/socket.h"
#include "../../filesys/filesocket.h"
#include "../../filesys/fsapi.h"
#include "../../filesys/stdfsa.h"
#include "../../system/packethandler.h"
#include "../micaz/ioeeprom.h"
#include "../../system/stdserialhandler.h"
#include "../../utilities/math.h"
#include "../../io/cc2420/cc2420controlm.h"
#include "../../utilities/eventlogger.h"
#include "../micaz/adcsocket.h"
#include "../../system/threaddata.h"
#include "../../utilities/math.h"
#include "../../system/scheduling.h"
#include "../../system/bytestorage.h"
#include "../../system/nodeconfig.h"
#include "../../system/generictimer.h"
#include "../micaz/sounder.h"
#include "./bootloader.h"
uint16_t variable_debug;
volatile uint16_t *syscallptr;
extern volatile uint16_t *old_stack_ptr;

/**\defgroup syscall System calls
This module defines system calls and is the interface between the OS and the user applications 
\note Do not compile directly under Os. That will mess up with the memory locations!!!
*/
//Note that -Os will destroy the locations!
//EA00
void thread_yield_logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_YIELDFUNCTION, currentindex);
    thread_yield();
}

/**\ingroup syscall 
Yield current thread.
*/
void yieldfunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void yieldfunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_YIELDFUNCTION
    thread_yield_logger();
#endif
#else
    thread_yield();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void Leds_greenToggle_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GREENTOGGLEFUNCTION, currentindex);
    Leds_greenToggle();
}

/**\ingroup syscall 
Toggle the green LED.
*/
void greentogglefunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void greentogglefunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GREENTOGGLEFUNCTION
    Leds_greenToggle_Logger();
#endif
#else
    Leds_greenToggle();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void Leds_redToggle_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_REDTOGGLEFUNCTION, currentindex);
    Leds_redToggle();
}

/**\ingroup syscall 
Toggle the red LED. 
*/
void redtogglefunction() __attribute__ ((section(".systemcall"))) __attribute__
    ((naked));
void redtogglefunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_REDTOGGLEFUNCTION
    Leds_redToggle_Logger();
#endif
#else
    Leds_redToggle();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getThreadAddress_avr()
{
    thread **returnthreadaddr;

    returnthreadaddr = getThreadAddress();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0"
                  "\n\t"::"r" (returnthreadaddr));
}

//-------------------------------------------------------------------------
void getThreadAddress_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETCURRENTTHREADADDRESS, currentindex);
    getThreadAddress_avr();
}

//EA09

/**\ingroup syscall 
This function is going to put the address into two registers, R24 and R25
*/
void getCurrentThreadAddress() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getCurrentThreadAddress()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETCURRENTTHREADADDRESS
    getThreadAddress_Logger();
#endif
#else
    getThreadAddress_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getRadioMutexAddress_avr()
{
    mutex *msendaddr = getRadioMutexAddress();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0"
                  "\n\t"::"r" (msendaddr));
}

//-------------------------------------------------------------------------
void getRadioMutexAddress_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETRADIOMUTEX, currentindex);
    getRadioMutexAddress_avr();
}

/**\ingroup syscall 
Get the mutex of radio send address.
*/
void getRadioMutex() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getRadioMutex()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETRADIOMUTEX
    getRadioMutexAddress_Logger();
#endif
#else
    getRadioMutexAddress_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void SocketRadioSend_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETRADIOSENDFUNCTION, currentindex);
    SocketRadioSend();
}

/**\ingroup syscall 
Call the radio send function indirectly. 
*/
void getRadioSendFunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getRadioSendFunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETRADIOSENDFUNCTION
    SocketRadioSend_Logger();
#endif
#else
    SocketRadioSend();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//bug
void mutexUnlockFunction()
{
    void *addr;
    asm volatile ("mov %A0, r20" "\n\t" "mov %B0, r21" "\n\t":"=r" (addr):);

    Mutex_unlock((volatile mutex *)addr);
}

//-------------------------------------------------------------------------
void mutexUnlockFunction_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_MUTEXUNLOCKFUNCTION, currentindex);
    mutexUnlockFunction();
}

/**\ingroup syscall 
Unlock the mutex. 
*/
void unlockMutex() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void unlockMutex()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_MUTEXUNLOCKFUNCTION
    mutexUnlockFunction_Logger();
#endif
#else
    mutexUnlockFunction();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getThreadIndexAddress_avr()
{
    int index;

    index = getThreadIndexAddress();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (index));
}

//-------------------------------------------------------------------------
void getThreadIndexAddress_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETCURRENTTHREADINDEX, currentindex);
    getThreadIndexAddress_avr();
}

/**\ingroup syscall
Get the index number of the current thread in the thread table. 
This function is going to put the address into two registers, R24 and R25.
*/
void getCurrentThreadIndex() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getCurrentThreadIndex()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETCURRENTTHREADINDEX
    getThreadIndexAddress_Logger();
#endif
#else
    getThreadIndexAddress_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getFilePathAddress_avr()
{
    void *filepathaddr;

    filepathaddr = getFilePathAddress();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0"
                  "\n\t"::"r" (filepathaddr));
}

//-------------------------------------------------------------------------
void getFilePathAddress_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETFILEPATHADDR, currentindex);
    getFilePathAddress_avr();
}

/**\ingroup syscall 
Get the file path locator, such as /abc/efg, etc. 
*/
void getFilePathAddr() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getFilePathAddr()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETFILEPATHADDR
    getFilePathAddress_Logger();
#endif
#else
    getFilePathAddress_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getFileModeAddress_avr()
{
    void *addr;

    addr = getFileModeAddress();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (addr));
}

//-------------------------------------------------------------------------
void getFileModeAddress_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETFILEMODEADDR, currentindex);
    getFileModeAddress_avr();
}

/**\ingroup syscall 
Get the file mode for read or write. 
*/
void getFileModeAddr() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getFileModeAddr()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETFILEMODEADDR
    getFileModeAddress_Logger();
#endif
#else
    getFileModeAddress_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getFileMutexAddress_avr()
{
    void *addr;

    addr = getFileMutexAddress();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (addr));
}

//-------------------------------------------------------------------------
void getFileMutexAddress_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETFILEMUTEXADDR, currentindex);
    getFileMutexAddress_avr();
}

/**\ingroup syscall 
Get the file operation mutex. 
*/
void getFileMutexAddr() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getFileMutexAddr()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETFILEMUTEXADDR
    getFileMutexAddress_Logger();
#endif
#else
    getFileMutexAddress_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void openFileTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_OPENFILESYSCALL, currentindex);
    openFileTask();
}

/**\ingroup syscall 
Open a file. The file handle is stored in the current thread table as well as internally by the kernel. 
*/
void openFileSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void openFileSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_OPENFILESYSCALL
    openFileTask_Logger();
#endif
#else
    openFileTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void closeFileTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_CLOSEFILESYSCALL, currentindex);
    closeFileTask();
}

/**\ingroup syscall 
Close a file. 
*/
void closeFileSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void closeFileSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_CLOSEFILESYSCALL
    closeFileTask_Logger();
#endif
#else
    closeFileTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void readFileTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_READFILESYSCALL, currentindex);
    readFileTask();
}

/**\ingroup syscall 
Read from a file. 
*/
void readFileSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void readFileSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_READFILESYSCALL
    readFileTask_Logger();
#endif
#else
    readFileTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void writeFileTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_WRITEFILESYSCALL, currentindex);
    writeFileTask();
}

/**\ingroup syscall 
Write to a file. 
*/
void writeFileSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void writeFileSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_WRITEFILESYSCALL
    writeFileTask_Logger();
#endif
#else
    writeFileTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void seekFileTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_SEEKFILESYSCALL, currentindex);
    seekFileTask();
}

/**\ingroup syscall 
Change file opener handle address. 
*/
void seekFileSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void seekFileSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_SEEKFILESYSCALL
    seekFileTask_Logger();
#endif
#else
    seekFileTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void ADCLight_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_ADCLIGHTSYSCALL, currentindex);
    ADCLight();
}

/**\ingroup syscall 
Get the reading from light sensor and store the result in the thread table. 
*/
void ADCLightSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void ADCLightSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_ADCLIGHTSYSCALL
    ADCLight_Logger();
#endif
#else
    ADCLight();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void ADCTemp_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_ADCTEMPSYSCALL, currentindex);
    ADCTemp();
}

/**\ingroup syscall 
Get the reading from temperature and store the result in the thread table. 
*/
void ADCTempSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void ADCTempSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_ADCTEMPSYSCALL
    ADCTemp_Logger();
#endif
#else
    ADCTemp();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void ADCMagX_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_ADCMAGXSYSCALL, currentindex);
    ADCMagX();
}

/**\ingroup syscall 
Get the reading from the X axis for the magnetic sensor and store the result in the thread table.  
*/
void ADCMagXSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void ADCMagXSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_ADCMAGXSYSCALL
    ADCMagX_Logger();
#endif
#else
    ADCMagX();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void ADCMagY_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_ADCMAGYSYSCALL, currentindex);
    ADCMagY();
}

/**\ingroup syscall 
Get the reading from the Y axis for the magnetic sensor and store the result in the thread table. 
*/
void ADCMagYSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void ADCMagYSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_ADCMAGYSYSCALL
    ADCMagY_Logger();
#endif
#else
    ADCMagY();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void ADCAccX_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_ADCACCXSYSCALL, currentindex);
    ADCAccX();
}

/**\ingroup syscall 
Get the reading from the X axis for the accelerator and store the result in the thread table. 
*/
void ADCAccXSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void ADCAccXSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_ADCACCXSYSCALL
    ADCAccX_Logger();
#endif
#else
    ADCAccX();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void ADCAccY_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_ADCACCYSYSCALL, currentindex);
    ADCAccY();
}

/**\ingroup syscall 
Get the reading from the Y axis for the accelerator and store the result in the thread table. 
*/
void ADCAccYSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void ADCAccYSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_ADCACCYSYSCALL
    ADCAccY_Logger();
#endif
#else
    ADCAccY();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void postNewTask_avr()
{
    void (*fp) (void);
    uint16_t priority;
    asm volatile ("mov %A0, r20" "\n\t" "mov %B0, r21" "\n\t":"=r" (fp):);
    asm volatile ("mov %A0, r22" "\n\t" "mov %B0, r23"
                  "\n\t":"=r" (priority):);
    postTask(fp, priority);
}

//-------------------------------------------------------------------------
void postNewTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_POSTTASKSYSCALL, currentindex);
    postNewTask_avr();
}

/**\ingroup syscall 
Posttask here for backward compatibility
Bug to be fixed here. 
*/
void postTaskSysCall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void postTaskSysCall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_POSTTASKSYSCALL
    postNewTask_Logger();
#endif
#else
    postNewTask_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getRadioInfo_avr()
{
    void *addr;

    addr = getRadioInfo();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (addr));
}

//-------------------------------------------------------------------------
void getRadioInfo_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETCURRENTRADIOINFOADDRESS, currentindex);
    getRadioInfo_avr();
}

/**\ingroup syscall 
Get the Radio info address for populate it to send the radio packet info to the kernel. 
*/
void getCurrentRadioInfoAddress() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getCurrentRadioInfoAddress()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETCURRENTRADIOINFOADDRESS
    getRadioInfo_Logger();
#endif
#else
    getRadioInfo_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getHandleInfo_avr()
{
    void *addr;

    addr = getHandleInfo();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (addr));
}

//-------------------------------------------------------------------------
void getHandleInfo_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETCURRENTRADIOHANDLEADDRESS, currentindex);
    getHandleInfo_avr();
}

/**\ingroup syscall
Get the radio handle for registering a receiving handle. 
*/
void getCurrentRadioHandleAddress() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getCurrentRadioHandleAddress()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETCURRENTRADIOHANDLEADDRESS
    getHandleInfo_Logger();
#endif
#else
    getHandleInfo_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void syscall_registerEvent_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETCURRENTRADIOHANDLE, currentindex);
    registerReceiverHandle_syscall();
}

/**\ingroup syscall 
Register a receiving handle for incoming packet. 
*/
void setCurrentRadioHandle() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void setCurrentRadioHandle()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETCURRENTRADIOHANDLE
    syscall_registerEvent_Logger();
#endif
#else
    registerReceiverHandle_syscall();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void postNewThreadTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_POSTTHREADTASK, currentindex);
    postNewThreadTask();
}

/**\ingroup syscall 
Trigger the thread scheduling task. 
*/
void postThreadTask() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void postThreadTask()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_POSTTHREADTASK
    postNewThreadTask_Logger();
#endif
#else
    postNewThreadTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void debugInfoVariable()
{
    asm volatile ("mov %A0, r8" "\n\t" "mov %B0, r9"
                  "\n\t":"=r" (variable_debug):);
    variable_debug = variable_debug + 2;
    asm volatile ("mov %A0, r10" "\n\t" "mov %B0, r11"
                  "\n\t":"=r" (variable_debug):);
    variable_debug = variable_debug + 2;
    asm volatile ("mov %A0, r12" "\n\t" "mov %B0, r13"
                  "\n\t":"=r" (variable_debug):);
    variable_debug = variable_debug + 2;
    //asm volatile("ret"::); 
}

//-------------------------------------------------------------------------
void debugInfoVariable_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_DEBUGINFOTASK, currentindex);
    debugInfoVariable();
}

/**\ingroup syscall 
for system call debugging purposes
*/
void debugInfoTask() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void debugInfoTask()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_DEBUGINFOTASK
    debugInfoVariable_Logger();
#endif
#else
    debugInfoVariable();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void Leds_yellowToggle_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_YELLOWTOGGLEFUNCTION, currentindex);
    Leds_yellowToggle();
}

/**\ingroup syscall 
Toggle the yellow LED. 
*/
void yellowtogglefunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void yellowtogglefunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_YELLOWTOGGLEFUNCTION
    Leds_yellowToggle_Logger();
#endif
#else
    Leds_yellowToggle();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void Leds_redOn_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_REDONFUNCTION, currentindex);
    Leds_redOn();
}

/**\ingroup syscall 
Turn the red LED on. 
*/
void redonfunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void redonfunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_REDONFUNCTION
    Leds_redOn_Logger();
#endif
#else
    Leds_redOn();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void Leds_redOff_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_REDOFFFUNCTION, currentindex);
    Leds_redOff();
}

/**\ingroup syscall 
Turn the red LED off. 
*/
void redofffunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void redofffunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_REDOFFFUNCTION
    Leds_redOff_Logger();
#endif
#else
    Leds_redOff();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void Leds_yellowOn_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_YELLOWONFUNCTION, currentindex);
    Leds_yellowOn();
}

/**\ingroup syscall 
Turn the yellow LED on.
*/
void yellowonfunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void yellowonfunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_YELLOWONFUNCTION
    Leds_yellowOn_Logger();
#endif
#else
    Leds_yellowOn();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void Leds_yellowOff_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_YELLOWOFFFUNCTION, currentindex);
    Leds_yellowOff();
}

/**\ingroup syscall 
Turn the yellow LED off. 
*/
void yellowofffunction() __attribute__ ((section(".systemcall"))) __attribute__
    ((naked));
void yellowofffunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_YELLOWOFFFUNCTION
    Leds_yellowOff_Logger();
#endif
#else
    Leds_yellowOff();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void Leds_greenOn_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GREENONFUNCTION, currentindex);
    Leds_greenOn();
}

/**\ingroup syscall 
Turn the green LED on. 
*/
void greenonfunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void greenonfunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GREENONFUNCTION
    Leds_greenOn_Logger();
#endif
#else
    Leds_greenOn();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void Leds_greenOff_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GREENOFFFUNCTION, currentindex);
    Leds_greenOff();
}

/**\ingroup syscall 
Turn the green LED off. 
*/
void greenofffunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void greenofffunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GREENOFFFUNCTION
    Leds_greenOff_Logger();
#endif
#else
    Leds_greenOff();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void break_point_function_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_BREAKFUNCTION, currentindex);
    break_point_function();
}

/**\ingroup syscall
Break the current thread. 
*/
void breakfunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void breakfunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_BREAKFUNCTION
    break_point_function_Logger();
#endif
#else
    break_point_function();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getSerialMutexAddress_avr()
{
    void *addr;

    addr = getSerialMutexAddress();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (addr));
}

//-------------------------------------------------------------------------
void getSerialMutexAddress_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETSERIALMUTEX, currentindex);
    getSerialMutexAddress_avr();
}

/**\ingroup syscall 
Get the serial mutex. 
*/
void getSerialMutex() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getSerialMutex()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETSERIALMUTEX
    getSerialMutexAddress_Logger();
#endif
#else
    getSerialMutexAddress_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getSerialInfo_avr()
{
    void *addr;

    addr = getSerialInfo();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (addr));
}

//-------------------------------------------------------------------------
void getSerialInfo_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETCURRENTSERIALINFOADDRESS, currentindex);
    getSerialInfo_avr();
}

/**\ingroup syscall
Get the serial handle for sending a serial packet. 
*/
void getCurrentSerialInfoAddress() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getCurrentSerialInfoAddress()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETCURRENTSERIALINFOADDRESS
    getSerialInfo_Logger();
#endif
#else
    getSerialInfo_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void SocketSerialSend_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETSERIALSENDFUNCTION, currentindex);
    SocketSerialSend();
}

/**\ingroup syscall 
Send a message through the serial port. 
*/
void getSerialSendFunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getSerialSendFunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETSERIALSENDFUNCTION
    SocketSerialSend_Logger();
#endif
#else
    SocketSerialSend();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getSerialHandleInfo_avr()
{
    void *addr;

    addr = getSerialHandleInfo();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (addr));
}

//-------------------------------------------------------------------------
void getSerialHandleInfo_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETCURRENTSERIALHANDLEADDRESS, currentindex);
    getSerialHandleInfo_avr();
}

/**\ingroup syscall 
Get the serial receiving handle. 
*/
void getCurrentSerialHandleAddress() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getCurrentSerialHandleAddress()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETCURRENTSERIALHANDLEADDRESS
    getSerialHandleInfo_Logger();
#endif
#else
    getSerialHandleInfo_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void syscall_registerEventSerial_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_SETCURRENTSERIALHANDLEADDRESS, currentindex);
    syscall_registerEventSerial();
}

/**\ingroup syscall 
Register a serial event receiving handle. 
*/
void setCurrentSerialHandleAddress() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void setCurrentSerialHandleAddress()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_SETCURRENTSERIALHANDLEADDRESS
    syscall_registerEventSerial_Logger();
#endif
#else
    syscall_registerEventSerial();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getEEPROMHandleInfo()
{
    void *addr;

    addr = getGenericStorageNodeAddress();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (addr));
}

//-------------------------------------------------------------------------
void getEEPROMHandleInfo_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETCURRENTEEPROMHANDLEADDRESS, currentindex);
    getEEPROMHandleInfo();
}

/**\ingroup syscall 
Get the EEPROM handle address for read/write. 
*/
void getCurrentEEPROMHandleAddress() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getCurrentEEPROMHandleAddress()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETCURRENTEEPROMHANDLEADDRESS
    getEEPROMHandleInfo_Logger();
#endif
#else
    getEEPROMHandleInfo();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void readEEPROMTask()
{
    genericReadTask();
}

//-------------------------------------------------------------------------
void readEEPROMTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_READFROMEEPROM, currentindex);
    readEEPROMTask();
}

/**\ingroup syscall 
Read from EEPROM. 
*/
void readFromEEPROM() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void readFromEEPROM()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_READFROMEEPROM
    readEEPROMTask_Logger();
#endif
#else
    readEEPROMTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void writeEEPROMTask()
{
    genericWriteTask();
}

//-------------------------------------------------------------------------
void writeEEPROMTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_WRITETOEEPROM, currentindex);
    writeEEPROMTask();
}

/**\ingroup syscall 
Write to EEPROM. 
*/
void writeToEEPROM() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void writeToEEPROM()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_WRITETOEEPROM
    writeEEPROMTask_Logger();
#endif
#else
    writeEEPROMTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//bug
void getMalloc()
{
}
void getMalloc_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_MALLOCFUNCTION, currentindex);
    getMalloc();
}

/**\ingroup syscall 
Get a chunk of memory and store the return address into registers. 
*/
void mallocFunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void mallocFunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_MALLOCFUNCTION
    getMalloc_Logger();
#endif
#else
    getMalloc();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//bug 
void freeMemory()
{
}
void freeMemory_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_FREEFUNCTION, currentindex);
    freeMemory();
}

/**\ingroup syscall 
Free a chunk of memory by using an address in the memory stored in registers. 
*/
void freeFunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void freeFunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_FREEFUNCTION
    freeMemory_Logger();
#endif
#else
    freeMemory();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void disableSocketRadioState_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_DISABLERADIOSTATE, currentindex);
    restoreRadioState();
}

/**\ingroup syscall 
Reset radio. Useful for sending out packets. Otherwise sometimes the radio will stop working. 
*/
void disableRadioState() __attribute__ ((section(".systemcall"))) __attribute__
    ((naked));
void disableRadioState()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_DISABLERADIOSTATE
    disableSocketRadioState_Logger();
#endif
#else
    restoreRadioState();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getRandomTask()
{
    uint16_t num;

    num = getRandomNumber();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (num));
}

//-------------------------------------------------------------------------

void getRandomTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETRANDOMNUMBER, currentindex);
    getRandomTask();
}

//-------------------------------------------------------------------------

/**\ingroup syscall 
Get a random number from the kernel.
*/
void getRandomNumberSyscall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getRandomNumberSyscall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETRANDOMNUMBER
    getRandomTask_Logger();
#endif
#else
    getRandomTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//because this system call directly links into the radio module, therefore, we do not need to consider the problem of directly using registers. 
void setRadioFrequencyTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_SETRADIOFREQUENCY, currentindex);
#ifdef RADIO_CC2420
    setRadioFrequencyTask();
#endif
}

/**\ingroup syscall 
Set the radio frequency, stored in the registers. 
*/
//inline result_t cc2420controlm_CC2420Control_TuneManual(uint16_t DesiredFreq);
void setRadioFrequency() __attribute__ ((section(".systemcall"))) __attribute__
    ((naked));
void setRadioFrequency()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_SETRADIOFREQUENCY
    setRadioFrequencyTask_Logger();
#endif
#else
#ifdef RADIO_CC2420
    setRadioFrequencyTask();
#endif
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void setRadioChannelTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_SETRADIOCHANNEL, currentindex);
#ifdef RADIO_CC2420
    setRadioChannelTask();
#endif
}

/**\ingroup syscall 
Set the channel, stored in the registers. 
*/
//inline result_t cc2420controlm_CC2420Control_TuneChannel(uint8_t channel); 
void setRadioChannel() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void setRadioChannel()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_SETRADIOCHANNEL
    setRadioChannelTask_Logger();
#endif
#else
#ifdef RADIO_CC2420
    setRadioChannelTask();
#endif
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void setRadioPowerTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_SETRADIOPOWER, currentindex);
#ifdef RADIO_CC2420
    setRadioPowerTask();
#endif
}

/**\ingroup syscall 
Set the radio power, stored in the registers. 
*/
//inline result_t cc2420controlm_CC2420Control_TunePower(uint8_t powerlevel);
void setRadioPower() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void setRadioPower()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_SETRADIOPOWER
    setRadioPowerTask_Logger();
#endif
#else
#ifdef RADIO_CC2420
    setRadioPowerTask();
#endif
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getNodeIdTask()
{
    uint16_t nodeid;

    nodeid = node_readnodeid();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (nodeid));
}

//-------------------------------------------------------------------------

void getNodeIdTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETNODEID, currentindex);
    getNodeIdTask();
}

//-------------------------------------------------------------------------
void getNodeID() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getNodeID()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETNODEID
    getNodeIdTask_Logger();
#endif
#else
    getNodeIdTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void setNodeIdTask()
{
    uint16_t nodeid;
    asm volatile ("mov %A0, r20" "\n\t" "mov %B0, r21" "\n\t":"=r" (nodeid):);

    node_writenodeid(nodeid);
}

//-------------------------------------------------------------------------
void setNodeIdTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_SETNODEID, currentindex);
    setNodeIdTask();
}

//-------------------------------------------------------------------------
void setNodeID() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void setNodeID()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_SETNODEID
    setNodeIdTask_Logger();
#endif
#else
    setNodeIdTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getThreadControlBlock_avr()
{
    void *addr;

    addr = getNewThreadBlock();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (addr));
}

//-------------------------------------------------------------------------
void getThreadControlBlockAddress_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETTHREADCONTROLBLOCK, currentindex);
    getThreadControlBlock_avr();
}

/**\ingroup syscall 
*/
void getThreadControlBlockAddress() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getThreadControlBlockAddress()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETTHREADCONTROLBLOCK
    getThreadControlBlockAddress_Logger();
#endif
#else
    getThreadControlBlock_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getThreadControlMutex_avr()
{
    void *addr;

    addr = getCreateThreadMutex();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (addr));
}

//-------------------------------------------------------------------------
void getThreadControlBlockMutex_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_GETTHREADCONTROLBLOCK, currentindex);
    getThreadControlMutex_avr();
}

/**\ingroup syscall 
*/
void getThreadControlBlockMutex() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getThreadControlBlockMutex()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_GETTHREADCONTROLMUTEX
    getThreadControlBlockMutex_Logger();
#endif
#else
    getThreadControlMutex_avr();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void createThreadSyscallTask_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_CREATETHREAD, currentindex);
    createThreadTask();
}

/**\ingroup syscall 
Set the radio frequency, stored in the registers. 
*/
//inline result_t cc2420controlm_CC2420Control_TuneManual(uint16_t DesiredFreq);
void createThreadSyscall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void createThreadSyscall()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_CREATETHREAD
    createThreadSyscallTask_Logger();
#endif
#else
    createThreadTask();
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getInternalTracingBlockAddress()
{
    void *addr;

    addr = getTracingBlockAddress();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0" "\n\t"::"r" (addr));
}

/**\ingroup syscall 
*/
void getInternalTracingBlockAddressSyscall()
    __attribute__ ((section(".systemcall"))) __attribute__ ((naked));
void getInternalTracingBlockAddressSyscall()
{
    getInternalTracingBlockAddress();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

/**\ingroup syscall 
*/

void enableTracingSyscall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void enableTracingSyscall()
{
    enabletracingfunction();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

/**\ingroup syscall 
*/
void disableTracingSyscall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void disableTracingSyscall()
{
    disabletracingfunction();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

/**\ingroup syscall 
Posttask here for backward compatibility
Bug to be fixed here. 
*/
void postTaskSysCallWithoutAnyLogging()
    __attribute__ ((section(".systemcall"))) __attribute__ ((naked));
void postTaskSysCallWithoutAnyLogging()
{
    postNewTask_avr();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

/**\ingroup syscall 
Trigger the thread scheduling task. 
*/
void postThreadTaskNoLogging() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void postThreadTaskNoLogging()
{
    postNewThreadTask();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

/**\ingroup syscall 
*
*/
void jumpToTracePointSyscall() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void jumpToTracePointSyscall()
{
    apptracepointfunction();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void insertTracePoint()
{
    uint16_t pagenum;
    uint16_t pageoffset;
    asm volatile ("mov %A0, r20" "\n\t" "mov %B0, r21" "\n\t":"=r" (pagenum):);
    asm volatile ("mov %A0, r22" "\n\t" "mov %B0, r23"
                  "\n\t":"=r" (pageoffset):);
    boot_insertTracePoint(pagenum, (uint8_t) pageoffset);
}

/**\ingroup syscall 
*
*/
void insertTracePointToUser() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void insertTracePointToUser()
{
    insertTracePoint();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void insertTracePointLong()
{
    uint16_t pagenum;
    uint16_t pageoffset;
    asm volatile ("mov %A0, r20" "\n\t" "mov %B0, r21" "\n\t":"=r" (pagenum):);
    asm volatile ("mov %A0, r22" "\n\t" "mov %B0, r23"
                  "\n\t":"=r" (pageoffset):);
    boot_insertTracePointLong(pagenum, (uint8_t) pageoffset);
}

/**\ingroup syscall 
*
*/
void insertTracePointToUserLong() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void insertTracePointToUserLong()
{
    insertTracePointLong();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void terminateThreadFunctionAvr()
{
    void (*fp) (void);
    uint8_t currentthreadindex;
    asm volatile ("mov %A0, r20" "\n\t" "mov %B0, r21" "\n\t":"=r" (fp):);

    currentthreadindex = getThreadIndexAddress();
    setThreadTerminateFunction(currentthreadindex, fp);
}

/**\ingroup syscall 
This system call allows the user thread to define a clean-up function that releases the resources currently allocated by the user thread function
*/
void terminateThreadFunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void terminateThreadFunction()
{
    terminateThreadFunctionAvr();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void setupTimerTask()
{
    void (*fp) (void);
    uint8_t currentthreadindex;
    uint16_t period;
    uint16_t type;
    asm volatile ("mov %A0, r18" "\n\t" "mov %B0, r19" "\n\t":"=r" (period):);
    asm volatile ("mov %A0, r20" "\n\t" "mov %B0, r21" "\n\t":"=r" (type):);
    asm volatile ("mov %A0, r22" "\n\t" "mov %B0, r23" "\n\t":"=r" (fp):);

    currentthreadindex = getThreadIndexAddress();
    setTimerCallBackFunction(currentthreadindex, period, type, fp);
}

/**\ingroup syscall 
This system call allows the user thread to define a clean-up function that releases the resources currently allocated by the user thread function
*/
void setTimerFunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void setTimerFunction()
{
    setupTimerTask();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void getStackPtrTask()
{
    void **returnthreadaddr;

    returnthreadaddr = getKernelStackAddress();
    asm volatile ("mov r20, %A0" "\n\t" "mov r21, %B0"
                  "\n\t"::"r" (returnthreadaddr));
}

//Get the address of the kernel stack 
void getStackPtr() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void getStackPtr()
{
    getStackPtrTask();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//Get the address of the kernel stack 
void removeTracePointTask()
{
    uint16_t pagenum;
    uint16_t pageoffset;
    uint8_t *buffer;
    asm volatile ("mov %A0, r20" "\n\t" "mov %B0, r21" "\n\t":"=r" (pagenum):);
    asm volatile ("mov %A0, r22" "\n\t" "mov %B0, r23"
                  "\n\t":"=r" (pageoffset):);
    asm volatile ("mov %A0, r18" "\n\t" "mov %B0, r19" "\n\t":"=r" (buffer):);

    SWAP_STACK_PTR(syscallptr, old_stack_ptr);
    boot_removeTracePoint(pagenum, (uint8_t) pageoffset, buffer);
    SWAP_STACK_PTR(old_stack_ptr, syscallptr);
}

//-------------------------------------------------------------------------
void removeTracePoint() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void removeTracePoint()
{
    removeTracePointTask();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//Get the address of the kernel stack 
void removeTracePointLongTask()
{
    uint16_t pagenum;
    uint16_t pageoffset;
    uint8_t *buffer;
    asm volatile ("mov %A0, r20" "\n\t" "mov %B0, r21" "\n\t":"=r" (pagenum):);
    asm volatile ("mov %A0, r22" "\n\t" "mov %B0, r23"
                  "\n\t":"=r" (pageoffset):);
    asm volatile ("mov %A0, r18" "\n\t" "mov %B0, r19" "\n\t":"=r" (buffer):);

    SWAP_STACK_PTR(syscallptr, old_stack_ptr);
    boot_removeTracePointLong(pagenum, (uint8_t) pageoffset, buffer);
    SWAP_STACK_PTR(old_stack_ptr, syscallptr);
}

//-------------------------------------------------------------------------
void removeTracePointLong() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void removeTracePointLong()
{
    removeTracePointLongTask();
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

/* added by Qi Mi (qm8e@virginia.edu) */
void sounderOn_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_SOUNDERONFUNCTION, currentindex);
    micaz_sounderOn();
}

/**\ingroup syscall 
Turn the sounder on. 
*/
void sounderonfunction() __attribute__ ((section(".systemcall"))) __attribute__
    ((naked));
void sounderonfunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_SOUNDERONFUNCTION
    sounderOn_Logger();         //defined in avr\syscall.c
#endif
#else
    //micaz_sounder();
    micaz_sounderOn();          //defined in micaz\sounder.c
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}

//-------------------------------------------------------------------------
void sounderOff_Logger()
{
    uint8_t currentindex;
    _atomic_t _atomic = _atomic_start();

    currentindex = getThreadIndexAddress();
    _atomic_end(_atomic);
    addTrace(TRACE_SYSCALL_SOUNDEROFFFUNCTION, currentindex);
    micaz_sounderOff();
}

/**\ingroup syscall 
Turn the sounder off. 
*/
void sounderofffunction() __attribute__ ((section(".systemcall")))
    __attribute__ ((naked));
void sounderofffunction()
{
#ifdef TRACE_ENABLE_SYSCALLEVENT
#ifdef TRACE_ENABLE_SYSCALL_SOUNDEROFFFUNCTION
    sounderOff_Logger();        //defined in avr\syscall.c
#endif
#else
    micaz_sounder();
    micaz_sounderOff();         //defined in micaz\sounder.c
#endif
    asm volatile ("nop"::);
    asm volatile ("ret"::);
}
