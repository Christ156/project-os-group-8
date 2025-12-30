#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

/* Updated Process Structure */
struct Process {
    int id, at, bt;
    int ct, tat, wt, rt; // Added rt for Response Time
    int done;
    int firstResponse;   // Flag to track if the process has started for the first time
};

/* Updated Print function to include Response Time */
void printResults(struct Process p[], int n, float totalWT, float totalTAT, float totalRT) {
    printf("\nProcess\tAT\tBT\tCT\tTAT\tWT\tRT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n", p[i].id, p[i].at, p[i].bt, p[i].ct, p[i].tat, p[i].wt, p[i].rt);
    }

    printf("\nAverage Waiting Time    = %.2f", totalWT / n);
    printf("\nAverage Turnaround Time = %.2f", totalTAT / n);
    printf("\nAverage Response Time   = %.2f\n", totalRT / n);
}

void fcfs(struct Process p[], int n) {
    // Sort by Arrival Time
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
    float totalWT = 0, totalTAT = 0, totalRT = 0;

    printf("\n--- FCFS ---\nGantt Chart:\n");
    for(int i = 0; i < n; i++) {
        if(currentTime < p[i].at) {
            printf("| IDLE (%d) ", p[i].at);
            currentTime = p[i].at;
        }
        
        // Response Time for FCFS is current time - arrival
        p[i].rt = currentTime - p[i].at;
        totalRT += p[i].rt;

        currentTime += p[i].bt;
        p[i].ct = currentTime;
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;

        totalWT += p[i].wt;
        totalTAT += p[i].tat;
        printf("| P%d (%d) ", p[i].id, p[i].ct);
    }
    printf("|\n");
    printResults(p, n, totalWT, totalTAT, totalRT);
}

void sjfNonPreemptive(struct Process p[], int n) {
    int completed = 0, currentTime = 0;
    float totalWT = 0, totalTAT = 0, totalRT = 0;

    for(int i = 0; i < n; i++) p[i].done = 0;

    printf("\n--- SJF (Non-Preemptive) --\nGantt Chart:\n");
    while(completed < n) {
        int idx = -1, minBT = INT_MAX;

        for(int i = 0; i < n; i++) {
            if(!p[i].done && p[i].at <= currentTime) {
                if (p[i].bt < minBT) {
                    minBT = p[i].bt;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            printf("| IDLE (%d) ", currentTime + 1);
            currentTime++;
            continue;
        }

        // Response Time: First time it gets the CPU
        p[idx].rt = currentTime - p[idx].at;
        totalRT += p[idx].rt;

        currentTime += p[idx].bt;
        p[idx].ct = currentTime;
        p[idx].tat = p[idx].ct - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        p[idx].done = 1;
        completed++;

        totalWT += p[idx].wt;
        totalTAT += p[idx].tat;
        printf("| P%d (%d) ", p[idx].id, p[idx].ct);
    }
    printf("|\n");
    printResults(p, n, totalWT, totalTAT, totalRT);
}

void sjfPreemptive(struct Process p[], int n) {
    int completed = 0, currentTime = 0;
    float totalWT = 0, totalTAT = 0, totalRT = 0;
    int remainingBT[n];
    
    for(int i = 0; i < n; i++) {
        remainingBT[i] = p[i].bt;
        p[i].done = 0;
        p[i].firstResponse = 0; // Initialize RT flag
    }

    printf("\n--- SJF (Preemptive / SRTF) ---\nGantt Chart:\n");
    int lastProcess = -1;

    while(completed < n) {
        int idx = -1, minBT = INT_MAX;

        for(int i = 0; i < n; i++) {
            if(!p[i].done && p[i].at <= currentTime) {
                if(remainingBT[i] < minBT) {
                    minBT = remainingBT[i];
                    idx = i;
                }
            }
        }

        if(idx == -1) {
            currentTime++;
            continue;
        }

        // CAPTURE RESPONSE TIME: Only the first time it is picked
        if (p[idx].firstResponse == 0) {
            p[idx].rt = currentTime - p[idx].at;
            p[idx].firstResponse = 1;
            totalRT += p[idx].rt;
        }

        if(lastProcess != idx) {
            printf("| P%d (%d) ", p[idx].id, currentTime);
            lastProcess = idx;
        }

        remainingBT[idx]--;
        currentTime++;

        if(remainingBT[idx] == 0) {
            p[idx].ct = currentTime;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
            p[idx].done = 1;
            completed++;
            totalWT += p[idx].wt;
            totalTAT += p[idx].tat;
        }
    }
    printf("| (%d) |\n", currentTime);
    printResults(p, n, totalWT, totalTAT, totalRT);
}

void roundRobin(struct Process p[], int n, int tq) {
    int completed = 0, currentTime = 0;
    float totalWT = 0, totalTAT = 0, totalRT = 0;
    int remainingBT[n], queue[n * 20], head = 0, tail = 0;
    bool inQueue[n];

    for (int i = 0; i < n; i++) {
        remainingBT[i] = p[i].bt;
        inQueue[i] = false;
        p[i].firstResponse = 0; // Initialize RT flag
    }

    // Initial check for processes at time 0
    for (int i = 0; i < n; i++) {
        if (p[i].at <= currentTime) {
            queue[tail++] = i;
            inQueue[i] = true;
        }
    }

    printf("\n--- Round Robin (TQ = %d) ---\nGantt Chart:\n", tq);
    while (completed < n) {
        if (head == tail) {
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

        // CAPTURE RESPONSE TIME: First time the process hits the CPU
        if (p[i].firstResponse == 0) {
            p[i].rt = currentTime - p[i].at;
            p[i].firstResponse = 1;
            totalRT += p[i].rt;
        }

        int executeTime = (remainingBT[i] > tq) ? tq : remainingBT[i];
        int startTime = currentTime;
        currentTime += executeTime;
        remainingBT[i] -= executeTime;

        printf("| P%d (%d) ", p[i].id, currentTime);

        // Arrival check during execution
        for (int j = 0; j < n; j++) {
            if (!inQueue[j] && p[j].at <= currentTime && p[j].at > startTime) {
                queue[tail++] = j;
                inQueue[j] = true;
            }
        }

        if (remainingBT[i] > 0) {
            queue[tail++] = i;
        } else {
            p[i].ct = currentTime;
            p[i].tat = p[i].ct - p[i].at;
            p[i].wt = p[i].tat - p[i].bt;
            completed++;
            totalWT += p[i].wt;
            totalTAT += p[i].tat;
        }
    }
    printf("|\n");
    printResults(p, n, totalWT, totalTAT, totalRT);
}

int main() {
    int n, choice, tq;
    printf("Enter number of processes: ");
    scanf("%d", &n);

    struct Process processes[n];
    for(int i = 0; i < n; i++){
        processes[i].id = i + 1;
        printf("Enter arrival time and burst time for P%d: ", i + 1);
        scanf("%d %d", &processes[i].at, &processes[i].bt);
    }

    do {
        printf("\n===== CPU Scheduling Menu =====\n");
        printf("1. FCFS\n2. SJF (Non-Preemptive)\n3. SJF (Preemptive/SRTF)\n4. Round Robin\n5. Exit\nEnter choice: ");
        scanf("%d", &choice);

        struct Process temp[n];
        for (int i = 0; i < n; i++) temp[i] = processes[i];

        switch (choice) {
            case 1: fcfs(temp, n); break;
            case 2: sjfNonPreemptive(temp, n); break;
            case 3: sjfPreemptive(temp, n); break;
            case 4: 
                printf("Enter Time Quantum: ");
                scanf("%d", &tq);
                roundRobin(temp, n, tq); 
                break;
        }
    } while (choice != 5);
    return 0;
}
