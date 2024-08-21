#include "buffer.h"

using namespace std;
int MAX_BUFFER = 1000;

class BoundedBuffer {
    public:
        int Size;
        string * Buffer;
        int Writing_index = 0, Reading_index = 0;  
        pthread_mutex_t BufLock;
        sem_t full, empty;
        BoundedBuffer() {}

        BoundedBuffer(int size) {
            Size = size;
            Buffer = static_cast<string*>(calloc(size, sizeof(string)));
            if (Buffer == nullptr) {
                cerr << "calloc failed" << endl;
                exit(1);
            }
            if (pthread_mutex_init(&BufLock, NULL) != 0){
                cerr << "pthread_mutex_init failed" << endl;
                free(Buffer);
                exit(1);
            }

            if (sem_init(&full, 0, 0) == -1) {
                cerr << "sem_init failed" << endl;
                free(Buffer);
                pthread_mutex_destroy(&BufLock);
                exit(1);
            }
            if (sem_init(&empty, 0, Size) == -1) {
                cerr << "sem_init failed" << endl;
                free(Buffer);
                pthread_mutex_destroy(&BufLock);
                sem_destroy(&full);
                exit(1);
            }
        }

        void WriteToBuffer(string str) {
            Buffer[Writing_index] = str;
            Writing_index = (Writing_index + 1) % Size;
            
        }

        string ReadFromBuffer() {
            string str = Buffer[Reading_index];
            Reading_index = (Reading_index + 1) % Size;
            return str;
        }

        void CloseBuffer() {
            free(Buffer);
            pthread_mutex_destroy(&BufLock);
            sem_destroy(&full);
            sem_destroy(&empty);
            return;
        }

};

// regular = no bounded
class RegularBuffer {
    public:
        int Size = MAX_BUFFER;
        string * Buffer;
        int Writing_index = 0, Reading_index = 0;  
        pthread_mutex_t BufLock;
        sem_t full, empty;
        RegularBuffer() {
            Buffer = static_cast<string*>(calloc(Size, sizeof(string)));
            if (Buffer == nullptr) {
                cerr << "calloc failed" << endl;
                exit(1);
            }
            if (pthread_mutex_init(&BufLock, NULL) != 0){
                cerr << "pthread_mutex_init failed" << endl;
                free(Buffer);
                exit(1);
            }

            if (sem_init(&full, 0, 0) == -1) {
                cerr << "sem_init failed" << endl;
                free(Buffer);
                pthread_mutex_destroy(&BufLock);
                exit(1);
            }
            if (sem_init(&empty, 0, Size) == -1) {
                cerr << "sem_init failed" << endl;
                free(Buffer);
                pthread_mutex_destroy(&BufLock);
                sem_destroy(&full);
                exit(1);
            }
        }

        void WriteToBuffer(string str) {
            Buffer[Writing_index] = str;
            Writing_index = (Writing_index + 1);
            
        }

        string ReadFromBuffer() {
            string str = Buffer[Reading_index];
            Reading_index = (Reading_index + 1);
            return str;
        }

        void CloseBuffer() {
            free(Buffer);
            pthread_mutex_destroy(&BufLock);
            sem_destroy(&full);
            sem_destroy(&empty);
            return;
        }

};
