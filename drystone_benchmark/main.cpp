#include "mbed.h"
#include "drystone.h"

DigitalOut myled(LED1);
Timer timer;
Serial pc(USBTX, USBRX);  //serial channel over HDK USB interface

int main() {
    double benchtime, dps, VAX_DMIPS;
    unsigned long loops;
    
    pc.baud(9600);
    pc.printf("Dhrystone Benchmark Tester \r\n");
    timer.start();
    while(1) {
        loops = 0l;
        myled = 1;
        timer.reset();
        do {
            Proc0();
            Proc0();
            loops += LOOPS;
            benchtime = timer.read();
        } while (benchtime <= 20.000);
        myled = 0;
        dps = (double)loops / benchtime;
        VAX_DMIPS = dps/1757.0;
        pc.printf("Dhrystone time for %ld passes = %.3f sec\r\n", loops, benchtime);
        pc.printf("KL25Z drystone benchmark is at %.0f dhrystones/second\r\n", dps);
        pc.printf("Kl25Z drystone has %.0f  VAX DMIPS\r\n", VAX_DMIPS);
        wait(1.0);
    }
}
