#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

//Maximum key value
const int MAX_KEY = 65536;

//Maximum thread count
const int MAX_THREADS = 1024;

struct node {
   int data;
   struct node* next;
};

/* Shared variables */
struct node* head = NULL;  

int n = 500;	//Initial population element count
int m = 10000;	//Total random operations count

//These values must be changed according to each case
float mMember = 0.65;	//Fraction of Member 
float mInsert = 0.35;	//Fraction of Insert
// float mDelete = 0.25;	//Fraction of Delete

int thread_count = 1;	//Thread Count

double start_time, finish_time, time_elapsed;

int member_count=0;	//Member function call count
int insert_count=0;	//Insert function call count
int delete_count=0;	//Delete function call count

pthread_cond_t read_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t write_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t rw_mutex;

int read_count = 0;
int read_waiting = 0;
int write_queue = 0;
int write_flag = 0;

// pthread_rwlock_t rwlock;
pthread_mutex_t count_mutex;

int Member(int value);
int Insert(int value);
int Delete(int value);
void Clear_Memory(void);
int Is_Empty(void);

void* Thread_Function(void* rank);

int main(int argc, char* argv[]) {
	
	if (argc != 2){ 
		fprintf(stderr, "please provide a command line argument for thread count less than %d\n", MAX_THREADS);
   		exit(0);
	}

	thread_count = strtol(argv[1], NULL, 10);  
   	if (thread_count <= 0 || thread_count > MAX_THREADS){ 
		fprintf(stderr, "please provide a command line argument for thread count less than %d\n", MAX_THREADS);
   		exit(0);
	}

	int i=0;
	//Inserting elements to linked-list
    for(;i<n;i++){   
        int r = rand()%65536;
        if(!Insert(r)){
            i--;
        }
    }
	
	pthread_t* thread_handles;
 
	thread_handles = malloc(thread_count*sizeof(pthread_t));
	pthread_mutex_init(&count_mutex, NULL);
	// pthread_rwlock_init(&rwlock, NULL);
    pthread_mutex_init(&rw_mutex, NULL);

	start_time = clock();
	for (i = 0; i < thread_count; i++)
		pthread_create(&thread_handles[i], NULL, Thread_Function, (void*) i);

	for (i = 0; i < thread_count; i++)
		pthread_join(thread_handles[i], NULL);
		
	finish_time = clock();
	time_elapsed = (finish_time - start_time)/CLOCKS_PER_SEC;
    //printf("Time Elapsed  = %.10f seconds\n", time_elapsed);
	printf("%.10f\n", time_elapsed);
   
	Clear_Memory();
	// pthread_rwlock_destroy(&rwlock);
	pthread_mutex_destroy(&count_mutex);
    pthread_mutex_destroy(&rw_mutex);
	free(thread_handles);
	return 0;
} 

//Member Function
int Member(int value) {
	struct node* temp;

	temp = head;
	while (temp != NULL && temp->data < value)
		temp = temp->next;

	if (temp == NULL || temp->data > value) {
		//printf("%d is a member in linked-list\n", value);		
		return 0;
	} else {
		//printf("%d is a member in linked-list\n", val		
		return 1;
	}
} 

// Insert function
int Insert(int value) {
	struct node* current = head;
	struct node* pred = NULL;
	struct node* temp;
	int return_value = 1;
   
	while (current != NULL && current->data < value) {
		pred = current;
		current = current->next;
	}

	if (current == NULL || current->data > value) {
		temp = malloc(sizeof(struct node));
		temp->data = value;
		temp->next = current;
		if (pred == NULL)
			head = temp;
		else
			pred->next = temp;
	} else {
		return_value = 0;
	}
	return return_value;
}

//Delete Function
int Delete(int value) {
   struct node* current = head;
   struct node* pred = NULL;
   int return_value = 1;

   while (current != NULL && current->data < value) {
      pred = current;
      current = current->next;
   }
   
   if (current != NULL && current->data == value) {
      if (pred == NULL) { 
         head = current->next;
         free(current);
      } else { 
         pred->next = current->next;
         free(current);
      }
   } else { 
      	return_value = 0;
		//printf("%d is not in the linked-list\n", value);
   }

   return return_value;
} 

//Function to free memory used for linked-list
void Clear_Memory(void) {
   struct node* currentent;
   struct node* next;

   if (Is_Empty()) return;
   currentent = head; 
   next = currentent->next;
   while (next != NULL) {
      free(currentent);
      currentent = next;
      next = currentent->next;
   }
   free(currentent);
} 

//Function to check if linked-list is empty
int Is_Empty(void) {
   if (head == NULL)
      return 1;
   else
      return 0;
}

void start_read() {
    pthread_mutex_lock(&rw_mutex);
	read_waiting++;
    while (write_flag) {
        pthread_cond_wait(&read_cond, &rw_mutex);
    }
	read_waiting--;
    read_count++;
    pthread_mutex_unlock(&rw_mutex);
}

void end_read() {
    pthread_mutex_lock(&rw_mutex);
    read_count--;
    if (read_count == 0) {
        pthread_cond_signal(&write_cond);
    }
    pthread_mutex_unlock(&rw_mutex);
}

void start_write() {
    pthread_mutex_lock(&rw_mutex);
    write_queue++;
    while (write_flag || read_count > 0) {
        pthread_cond_wait(&write_cond, &rw_mutex);
    }
    write_queue--;
    write_flag = 1;
    pthread_mutex_unlock(&rw_mutex);
}

void end_write() {
    pthread_mutex_lock(&rw_mutex);
    write_flag = 0;

    // Prioridad a los lectores
    if (read_waiting > 0)
	{
        pthread_cond_broadcast(&read_cond);
    }
	else if (write_queue > 0)
	{
        pthread_cond_signal(&write_cond);
    }

    pthread_mutex_unlock(&rw_mutex);
}

void* Thread_Function(void* rank) {
	unsigned int seed = time(NULL) + (long)rank;
	int my_member=0;
	int my_insert=0;
	// int my_delete=0;
	
	int ops_per_thread = m/thread_count;

	for (int i = 0; i < ops_per_thread; i++)
    {
		// float operation_choice = (rand()%10000/10000.0);
		// val = rand()%MAX_KEY;
        int operation_choice = rand_r(&seed) % 10000;
        int val = rand_r(&seed) % MAX_KEY;
	  
		if (operation_choice < mMember * 10000)
        {
			// pthread_rwlock_rdlock(&rwlock);
			// Member(val);
			// pthread_rwlock_unlock(&rwlock);
			// my_member++;
            start_read();
            Member(val);
            end_read();
            my_member++;
		}
        // else if (operation_choice < mMember + mInsert)
        else
        {
			// pthread_rwlock_wrlock(&rwlock);
			// Insert(val);
			// pthread_rwlock_unlock(&rwlock);
			// my_insert++;
            start_write();
            Insert(val);
            end_write();
            my_insert++;
		}
        // else
        // {
		// 	pthread_rwlock_wrlock(&rwlock);
		// 	Delete(val);
		// 	pthread_rwlock_unlock(&rwlock);
		// 	my_delete++;
		// }
	}

	pthread_mutex_lock(&count_mutex);
	member_count += my_member;
	insert_count += my_insert;
	// delete_count += my_delete;
	pthread_mutex_unlock(&count_mutex);
	return NULL;
}