#include <stdio.h>
#include <conio.h>

enum AllocationStrategy
{
    FIRST_FIT,
};

class Job
{
public:
    int id;
    size_t size;
    size_t time;
    AllocationStrategy fit;

    Job() {}
    Job(int _id, size_t _size, size_t _time, AllocationStrategy _fit) : id(_id), size(_size), time(_time), fit(_fit) {}
    ~Job() {}
};

class memoryPartition
{
public:
    size_t size;
    size_t time;
    Job *job;
    memoryPartition *next;
    memoryPartition *prev;

    memoryPartition() : job(NULL), next(NULL), prev(NULL) {}
    memoryPartition(size_t _size) : size(_size), time(0), job(NULL), next(NULL), prev(NULL) {}
    ~memoryPartition() {}

    bool isFree()
    {
        return job == NULL;
    }

    void setJob(Job *_job)
    {
        this->job = _job;
        this->time = _job->time;
    }

    size_t getUsed()
    {
        return job ? job->size : 0;
    }

    size_t getUnused()
    {
        return job ? size - job->size : size;
    }

    void getInfo()
    {
        printf("Size: %5zu     Available: %5s     Used: %5zu     Unused: %5zu     Job ID: %2d     Time left: %2zu\n",
               size, isFree() ? "Yes" : "No", getUsed(), getUnused(), job ? job->id : -1, time);
    }
};

class memoryNode
{
public:
    memoryPartition *head, *tail;

    memoryNode() : head(NULL), tail(NULL) {}
    ~memoryNode() {}

    void add(memoryPartition *memblock)
    {
        if (head == NULL)
        {
            head = memblock;
            tail = memblock;
        }
        else
        {
            tail->next = memblock;
            memblock->prev = tail;
            tail = memblock;
        }
    }

    int firstFit(Job *job)
    {
        memoryPartition *curr = head;
        memoryPartition *first = NULL;

        bool allocated = false;
        while (curr)
        {
        	
        	if (curr->isFree() && curr->size <= job->size)
        	{
        		allocated = true;
        		//put back in waiting list
                break;
                //run();               
			}
        	
        	
            if (curr->isFree() && curr->size >= job->size)
            {
                curr->setJob(job);
                allocated = true;
                break;
            }
            curr = curr->next;
        }
        return allocated;
    }
    
    void memInfo()
    {
        int count = 0;
        size_t totalUsed = 0;
        size_t totalUnused = 0;

        memoryPartition *curr = head;

        while (curr)
        {
            curr->getInfo();
            totalUsed += curr->getUsed();
            totalUnused += curr->getUnused();
            if (!curr->isFree())
                count++;
            curr = curr->next;
        }
        printf("Throughput          :  %d processes per unit time\n", count);
        printf("Storage utilization :  Used: %5zu,   Unused: %5zu,   Usage: %.2f\n", totalUsed, totalUnused, (double)totalUsed / (double)(totalUsed + totalUnused) * 100);
        printf("\n");
    }

    void run(Job *process, size_t size)
    {
    	//take from waiting list
        int i = 0;
        size_t time = 0;
        bool allocated = false;
        bool rejected = false;
        bool nextCycle = true;
        printf("T = %3zu, i = %2zu\n", time, i);
        memInfo();
        time++;

        memoryPartition *curr;
        while (nextCycle)
        {
            curr = head;
            while (curr)
            {
                if (curr->time != 0)
                    curr->time--;
                curr = curr->next;
            }

            do
            {
                allocated = false;
                rejected = false;
                if (process[i].size > 9500)
                {
                    printf("Job %d request was rejected! No memory block to store %zu memory\n\n", process[i].id, process[i].size);
                    rejected = true;
                }
                else
                {
                    // to use only a specific placement strategy,
                    // comment the whole switch block and
                    // uncomment line 254 and use desired strategy
                    switch (process[i].fit)
                    {
                    case FIRST_FIT:
                        allocated = firstFit(process + i);
                        break;

                    }
                    // uncomment line below to use a specific placement strategy
                    // allocated = firstFit(process + i);
                }
                if (allocated || rejected)
                    i++;
            } while ((allocated || rejected) && i < size);

            curr = head;
            while (curr)
            {
                if (curr->time == 1)
                    curr->job = NULL;
                curr = curr->next;
            }

            printf("T = %3zu\n", time);
            memInfo();
            time++;

            nextCycle = i < size;
        }
    }
};

// Driver program
int main()
{
    memoryPartition mblocks[] = {
        memoryPartition(9500), memoryPartition(7000), memoryPartition(4500), memoryPartition(8500), memoryPartition(3000),
        memoryPartition(9000), memoryPartition(1000), memoryPartition(5500), memoryPartition(1500), memoryPartition(500)};
//need to do waiting list
    Job process[] = {
        Job(21, 7540, 7, FIRST_FIT), Job(1, 5700, 5, FIRST_FIT), Job(2, 4190, 4, FIRST_FIT), Job(3, 3290, 8, FIRST_FIT), Job(4, 2030, 2, FIRST_FIT), Job(5, 2550, 2, FIRST_FIT),
        Job(8, 740, 10, FIRST_FIT), Job(9, 3930, 7, FIRST_FIT), Job(14, 420, 10, FIRST_FIT), Job(15, 220, 10, FIRST_FIT), Job(18, 1380, 1, FIRST_FIT), Job(6, 6990, 6, FIRST_FIT),
		Job(22, 2710, 2, FIRST_FIT), Job(12, 3820, 8, FIRST_FIT), Job(10, 6890, 6, FIRST_FIT), Job(25, 760, 10, FIRST_FIT), Job(7, 8940, 8, FIRST_FIT), Job(17, 3210, 3, FIRST_FIT),  
        Job(11, 6580, 5, FIRST_FIT), Job(16, 7540, 7, FIRST_FIT), Job(20, 3610, 3, FIRST_FIT), Job(24, 5950, 5, FIRST_FIT), Job(23, 8390, 8, FIRST_FIT), Job(13, 9140, 9, FIRST_FIT),
        Job(19, 9850, 9, FIRST_FIT), 
          
		};

    memoryNode node;

    // initialize memory manager with memory blocks
    for (size_t i = 0; i < (sizeof(mblocks) / sizeof(memoryPartition)); i++)
        node.add(mblocks + i);

    // run memory manager with process batch
    node.run(process, sizeof(process) / sizeof(Job));
    
    getch();

    return 0;
}
