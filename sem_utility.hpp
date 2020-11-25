#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <iostream>
#include "single_instance.hpp"
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};
class sem_utility {
public:
    bool init_sem(int sem_id, int val) {
        union semun tmp;
        tmp.val = val;
        if (semctl(sem_id, 0, SETVAL, tmp)) {
            return false;
        }
        return true;
    }
    int create_sem(key_t key, int val = 1) {
        int sem_id = semget(key, 1, IPC_CREAT | 0666);
        if (sem_id < 0) {
            std::cerr << "semget for key:" << key << " failed." << std::endl;
            return -1;
        }
        if (!init_sem(sem_id, val)) {
            return -1;
        }
        return sem_id;
    }
    bool sem_p(int sem_id) {
        struct sembuf sbuf;
        sbuf.sem_num = 0;
        sbuf.sem_op = -1;
        sbuf.sem_flg = SEM_UNDO;

        if (semop(sem_id, &sbuf, 1) < 0) {
            std::cerr << "sem P failed." << std::endl;
            return false;
        }
        return true;
    }
    bool sem_v(int sem_id) {
        struct sembuf sbuf;
        sbuf.sem_num = 0;
        sbuf.sem_op = 1;
        sbuf.sem_flg = SEM_UNDO;

        if (semop(sem_id, &sbuf, 1) < 0) {
            std::cerr << "sem V failed." << std::endl;
            return false;
        }
        return true;
    }
    bool del_sem(int sem_id) {
        union semun tmp;
        if (semctl(sem_id, 0, IPC_RMID, tmp)) {
            return false;
        }
        return true;
    }
    inline int get_sem_id(key_t key) {
        return semget(key, 0, 0);
    }
};

#define  G_SEM_UTILITY single_instance<sem_utility>::instance()