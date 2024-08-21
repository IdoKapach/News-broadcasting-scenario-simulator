#include "classes.h"

/*
 * producer creates several strings (reports) in the following format:
    “producer <i> <type> <j>”
    <i>: Producer's ID
    <type>: A random type chosen from SPORTS, NEWS, WEATHER
    <j>: The number of strings of type <type> this producer has already produced
    Each producer sends its information to the Dispatcher (introduced below) via its own private queue.
    After inserting all the products, the Producer sends a 'DONE' string through its queue.
 */
class Producer {
    public:
        BoundedBuffer *Buf;
        int NumOfProducts, Id;

        Producer() {}

        Producer(int id, int numOfProducts, BoundedBuffer *buf) {
            Id = id;
            NumOfProducts = numOfProducts;
            Buf = buf;
        }

        void Publish() {
            int sub[] = {0,0,0};
            int val, j;
            string type, message;
            for (int i = 0; i<NumOfProducts; i++) {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> distrib(0, 2);
                val = distrib(gen);

                if (val == 0) type = "NEWS";
                else if (val == 1) type = "WEATHER";
                else type = "SPORTS";
                j = sub[val]++;
                
                message = "producer " +  to_string(Id) + " " + type + " " + to_string(j);

                sem_wait(&(Buf->empty));
                pthread_mutex_lock(&(Buf->BufLock));

                Buf->WriteToBuffer(message);

                pthread_mutex_unlock(&(Buf->BufLock));
                sem_post(&(Buf->full));

            }
            sem_wait(&(Buf->empty));
            pthread_mutex_lock(&(Buf->BufLock));

            Buf->WriteToBuffer("DONE");

            pthread_mutex_unlock(&(Buf->BufLock));
            sem_post(&(Buf->full));

        }

};

/*
 * The Dispatcher continuously accepts messages from the producers' queues.
 * Each message is "sorted" by the Dispatcher and inserted into one of the Dispatcher queues,
 * which include strings of a single type - sport, news or weather.
 * When the Dispatcher receives a "DONE" message from all producers,
 * it sends a "DONE" message through each of its queues.
 */
class Dispatcher {
    public:
      BoundedBuffer ** BufArr;
      RegularBuffer *sportBuf, *weatherBuf, *newsBuf;
      int BufL;


      Dispatcher(BoundedBuffer ** arr, int arrL,  RegularBuffer *sport, RegularBuffer *weather, RegularBuffer *news) {
            BufArr = arr;
            BufL = arrL;
            sportBuf = sport;
            weatherBuf = weather;
            newsBuf = news;
      }

      void Publish(string message) {
            if (message.compare("DONE") == 0) {
                sem_wait(&(sportBuf->empty));
                pthread_mutex_lock(&(sportBuf->BufLock));
                sportBuf->WriteToBuffer(message);
                pthread_mutex_unlock(&(sportBuf->BufLock));
                sem_post(&(sportBuf->full));

                sem_wait(&(newsBuf->empty));
                pthread_mutex_lock(&(newsBuf->BufLock));
                newsBuf->WriteToBuffer(message);
                pthread_mutex_unlock(&(newsBuf->BufLock));
                sem_post(&(newsBuf->full));

                sem_wait(&(weatherBuf->empty));
                pthread_mutex_lock(&(weatherBuf->BufLock));
                weatherBuf->WriteToBuffer(message);
                pthread_mutex_unlock(&(weatherBuf->BufLock));
                sem_post(&(weatherBuf->full));
                return;
            }
            if (message.find("SPORTS") != string::npos) {
                sem_wait(&(sportBuf->empty));
                pthread_mutex_lock(&(sportBuf->BufLock));
                sportBuf->WriteToBuffer(message);
                pthread_mutex_unlock(&(sportBuf->BufLock));
                sem_post(&(sportBuf->full));
            }
            else if (message.find("NEWS") != string::npos) {
                sem_wait(&(newsBuf->empty));
                pthread_mutex_lock(&(newsBuf->BufLock));
                newsBuf->WriteToBuffer(message);
                pthread_mutex_unlock(&(newsBuf->BufLock));
                sem_post(&(newsBuf->full));
            }
            else if (message.find("WEATHER") != string::npos) {
                sem_wait(&(weatherBuf->empty));
                pthread_mutex_lock(&(weatherBuf->BufLock));
                weatherBuf->WriteToBuffer(message);
                pthread_mutex_unlock(&(weatherBuf->BufLock));
                sem_post(&(weatherBuf->full));
            }          
      }

      void Dispatch() {
        int con = 1;
        while (con) {
            con = 0;
            for (int i=0; i<BufL; i++) {
                if (BufArr[i] == nullptr) {
                    continue;
                }
                con = 1;
                sem_wait(&(BufArr[i] -> full));
                pthread_mutex_lock(&(BufArr[i]->BufLock));
                string message = BufArr[i]->ReadFromBuffer();

                pthread_mutex_unlock(&(BufArr[i]->BufLock));
                sem_post(&(BufArr[i]->empty));

                if (message.compare("DONE") == 0) {
                    BufArr[i]->CloseBuffer();
                    BufArr[i] = nullptr;
                }
                else Publish(message);

            }
        }
        Publish("DONE");
      }

};

/*
 * For each type of possible message, there is a Co-Editor that receives the message through
 * the Dispatcher’s queue, "edits" it, and passes it to the
 * screen manager via a single shared queue. The editing process is simulated by the Co-Editors
 * by blocking for one-tenth (0.1) of a second.
 * When a Co-Editor receives a "DONE" message, it passes it without waiting through the shared queue.
 */
class CoEditor {
    public:
        RegularBuffer *readBuf, *writeBuf;

        CoEditor(RegularBuffer * bufR, RegularBuffer * bufW) {
            readBuf = bufR;
            writeBuf=bufW;
        }

        void Publish() {
            bool con = true;
            string message;

            while(con){
                sem_wait(&(readBuf -> full));
                pthread_mutex_lock(&(readBuf->BufLock));
                message = readBuf->ReadFromBuffer();
                pthread_mutex_unlock(&(readBuf->BufLock));
                sem_post(&(readBuf -> empty));

                if (message.compare("DONE") != 0) {
                    this_thread::sleep_for(chrono::milliseconds(100));
                }
                else con = false;

                sem_wait(&(writeBuf -> empty));
                pthread_mutex_lock(&(writeBuf->BufLock));
                writeBuf->WriteToBuffer(message);
                pthread_mutex_unlock(&(writeBuf->BufLock));
                sem_post(&(writeBuf -> full));
            }
        }
};

/*
 * The Screen Manager displays the strings it receives via the Co-Editors' queue
 * to the screen (standard output). After printing all messages to the screen
 * and receiving three "DONE" messages, the Screen Manager displays a 'DONE' statement.
 */
class Screen {
    public:
        RegularBuffer *Buf;

        Screen(RegularBuffer *buf) {
            Buf = buf;
        }

        void Publish() {
            short int numDone = 0;
            string message;
            while(numDone < 3) {
                sem_wait(&(Buf -> full));
                pthread_mutex_lock(&(Buf->BufLock));
                message = Buf->ReadFromBuffer();
                pthread_mutex_unlock(&(Buf->BufLock));
                sem_post(&(Buf -> empty));

                if (message.compare("DONE") == 0) {
                    numDone++;
                    continue;
                }

                cout << message << endl;
            }
            cout << "DONE" << endl;

        }
};