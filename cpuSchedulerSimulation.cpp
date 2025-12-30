#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

struct Process {
    int id, at, bt;
    int ct, tat, wt;
    int done;
};

void printResults(struct Process p[], int n, float totalWT, float totalTAT){
    printf("\nProcess\tAT\tBT\tCT\tTAT\tWT\n");
    for (int i = 0; i < n; i++){
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",p[i].id, p[i].at, p[i].bt, p[i].ct, p[i].tat, p[i].wt);
    }

    // TODO: Add Average Response Time
    printf("\nAverage Turnaround Time = %.2f", totalTAT /n);
    printf("\nAverage Waiting Time = %.2f\n", totalWT / n);
}

void fcfs(struct Process p[], int n) {
    for(int i = 0; i < n-1; i++){
        for(int j = 0; j < n-i-1; j++){
            if(p[j].at > p[j+1].at){
                struct Process temp = p[j];
                p[j] = p[j+1];
                p[j+1] = temp;
            }
        }
    }

    int currentTime = 0;
    float totalWT = 0;
    float totalTAT = 0;

    printf("\n--- FCFS ---\n");
    printf("Gantt Chart:\n");

    for(int i = 0; i < n; i++){
        if(currentTime < p[i].at){
            printf("| IDLE (%d) ", p[i].at);
            currentTime = p[i].at;
        }

        currentTime += p[i].bt;
        p[i].ct = currentTime;
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;

        totalWT += p[i].wt;
        totalTAT += p[i].tat;

        printf("| Process %d (%d) ", p[i].id, p[i].ct);
    }
    printf("|\n");

    printResults(p, n, totalWT, totalTAT);
}

void sjf(struct Process p[], int n){
    int completed = 0;
    int currentTime = 0;
    float totalWT = 0;
    float totalTAT = 0;

    for(int i = 0; i < n; i++){
        p[i].done = 0;
    }

    printf("\n--- SJF (Non-Preemptive) --\n");
    printf("Gantt Chart:\n");

    while(completed < n) {
        int idx = -1;
        int minBT = INT_MAX;

        for(int i = 0; i < n; i++){
            if(!p[i].done && p[i].at <= currentTime){
                if (p[i].bt < minBT){
                    minBT = p[i].bt;
                    idx = i;
                }
            }
        }

        if (idx == -1){
            printf("| IDLE (%d) ", currentTime + 1);
            currentTime++;
            continue;
        }

        currentTime += p[idx].bt;
        
        p[idx].ct = currentTime;
        p[idx].tat = p[idx].ct - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        p[idx].done = 1;
        completed++;

        totalWT += p[idx].wt;
        totalTAT += p[idx].tat;

        printf("| Process %d (%d) ", p[idx].id, p[idx].ct);
    }

    printf("|\n");
    printResults(p, n, totalWT, totalTAT);
}

void srt(struct Process p[], int n){
    int completed = 0;
    int currentTime = 0;
    float totalWT = 0;
    float totalTAT = 0;

    int remainingBT[n];
    for(int i = 0; i < n; i++){
        remainingBT[i] = p[i].bt;
        p[i].done = 0;
    }

    printf("\n--- SRT (preemptive) ---\n");
    printf("Gantt Chart:\n");

    int lastProcess = -1;

    while(completed < n){
        int idx = -1;
        int minBT= INT_MAX;

        for(int i = 0; i < n; i++){
            if(!p[i].done && p[i].at <= currentTime && remainingBT[i] > 0){
                if(remainingBT[i] < minBT){
                    minBT = remainingBT[i];
                    idx = i;
                }
            }
        }

        if(idx == -1){
            if(lastProcess != -2){
                printf(" | IDLE (%d) ", currentTime + 1);
                lastProcess = -2;
            }

            currentTime++;
            continue;
        }

        if(lastProcess != idx){
            printf("| Process %d (%d) ",p[idx].id, currentTime + 1);
            lastProcess = idx;
        }

        remainingBT[idx]--;
        currentTime++;

        if(remainingBT[idx] == 0){
            p[idx].ct = currentTime;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
            p[idx].done = 1;
            completed++;

            totalWT += p[idx].wt;
            totalTAT += p[idx].tat;
        }
    }

    printf("|\n");
    printResults(p, n, totalWT, totalTAT);
}

void roundRobin(struct Process p[], int n, int tq) {
    int completed = 0;
    int currentTime = 0;
    float totalWT = 0, totalTAT = 0;

    int remainingBT[n];
    bool inQueue[n];
    int queue[n * 10];
    int head = 0, tail = 0;

	//Inisialisasi
    for (int i = 0; i < n; i++) {
        remainingBT[i] = p[i].bt;
        inQueue[i] = false;
        p[i].done = 0;
    }

    for (int i = 0; i < n; i++) {
        if (p[i].at <= currentTime) {
            queue[tail++] = i;
            inQueue[i] = true;
        }
    }

    printf("\n--- Round Robin (Time Quantum = %d) ---\n", tq);
    printf("Gantt Chart:\n");

    while (completed < n) {
        if (head == tail) {
            printf("| IDLE (%d) ", currentTime + 1);
            currentTime++;
            for (int i = 0; i < n; i++) {
                if (p[i].at <= currentTime && !inQueue[i]) {
                    queue[tail++] = i;
                    inQueue[i] = true;
                }
            }
            continue;
        }

        int i = queue[head++];
        
        int executeTime = (remainingBT[i] > tq) ? tq : remainingBT[i];
        
        int startTime = currentTime;
        currentTime += executeTime;
        remainingBT[i] -= executeTime;

        printf("| P%d (%d) ", p[i].id, currentTime);

        for (int j = 0; j < n; j++) {
            if (!inQueue[j] && p[j].at <= currentTime && p[j].at > startTime) {
                queue[tail++] = j;
                inQueue[j] = true;
            }
        }

        if (remainingBT[i] > 0) {
            queue[tail++] = i;
        } else {
            // Jika proses selesai
            p[i].ct = currentTime;
            p[i].tat = p[i].ct - p[i].at;
            p[i].wt = p[i].tat - p[i].bt;
            p[i].done = 1;
            completed++;

            totalWT += p[i].wt;
            totalTAT += p[i].tat;
        }
    }

    printf("|\n");
    printResults(p, n, totalWT, totalTAT);
}

int main(){
    int n, option, tq;

    printf("Enter how many Process do you want?: ");
    scanf("%d", &n);

    struct Process processes[n];

    for(int i = 0; i < n; i++){
        processes[i].id = i + 1;
        printf("Enter Arrival Time and Burst Time - Process %d [AT BT]: ", i + 1);
        scanf("%d %d", &processes[i].at, &processes[i].bt);
    }

    do {
        printf("\n===== CPU Scheduling Simulation =====\n");
        printf("1. FCFS\n");
        printf("2. SJF (Non-Preemptive)\n");
        printf("3. SRT (Preemptive)\n");
        printf("4. Round Robin\n");
        printf("5. Exit\n");
        printf("Enter your option: ");
        scanf("%d", &option);

        struct Process temp[n];
        for (int i = 0; i < n; i++) {
            temp[i] = processes[i];
        }

        if (option == 1) {
            fcfs(temp, n);
        }else if (option == 2) {
            sjf(temp, n);
        }else if (option == 3) {
            srt(temp, n);
        }else if (option == 4) {
            printf("Enter Time Slice: ");
            scanf("%d", &tq);
            roundRobin(temp, n, tq);
        }else if (option == 5) {
            printf("Thank you!\n");
        }else {
            printf("Invalid option! Try again....\n");
        }
    } while (option != 5);

    return 0;
}
