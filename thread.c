#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

void *thread_Fun(void *arg) {
    printf("new thread running...\n");
    int res;
    // 设置线程可取消状态
    // 当前线程会处理来自其他线程发送的cancel信号
    res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    if (res != 0) {
        printf("pthread_setcancelstate failed\n");
        return 0;
    }

    // 线程收到信号后立即执行
    res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (res != 0) {
        printf("modify pthread_setcanceltype failed\n");
        return 0;
    }
    // pthread_testcancel();
    // sleep(10);
    // pthread_testcancel();
    while (1);
    return "new thread quit!!!";
}

int main()
{
    pthread_t mythread;
    int res;
    res = pthread_create(&mythread, NULL, &thread_Fun, NULL);
    if (res != 0) {
        printf("thread create failed!!!");
        return 0;
    }

    // 睡眠会影响主线程关闭子线程，子线程先退出了
    sleep(1);
    // 主线程主动关闭子线程
    res = pthread_cancel(mythread);

    if (res != 0) {
        printf("stop mythread failed!!!!\n");
        return 0;
    }

    printf("main thread send cancel signal\n");

    void *mess;
    // 阻塞主线程，等待mythread 线程结束，释放自己占用的资源
    res = pthread_join(mythread, &mess);
    if (res != 0) {
        printf("1. wait thread failed\n");
        return 0;
    }

    // // 打印会导致段错误 segmentfault
    // printf("pthread_join: %s\n", (char*)mess);

    // // 输出获取到的mythread线程的返回值
    // res = pthread_join(mythread, &mess);
    // if (res == ESRCH) {
    //     printf("2. wait thread filed, thread is not exist\n");
    //     return 0;
    // }

    if (mess == PTHREAD_CANCELED) {
        printf("mythread was be forced stop!!!!\n");;
    } else {
        printf("error\n");
    }
    return 0;
}
