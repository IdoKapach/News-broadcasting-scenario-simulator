#include "classes.cpp"
#include <fstream>
#include <algorithm>
#include <iterator>

void * produce(void * pro) {
    ((Producer *) pro)->Publish();
    return NULL;
}
void * dispatch(void* dis) {
    ((Dispatcher *) dis) -> Dispatch();
    return NULL;
}

void * co(void * coE) {
    ((CoEditor *) coE) -> Publish();
    return NULL;
}

void * print(void * scr) {
    ((Screen *) scr) -> Publish();
    return NULL;
}

int main(int argc, char* argv[]) {
    RegularBuffer sport, news, weather, screenBuf;
    CoEditor coS(&sport, &screenBuf), coN(&news, &screenBuf), coW(&weather, &screenBuf);
    Screen scr(&screenBuf);

// counting the num of producer in the attached file
    ifstream countF(argv[1]);
    auto count = count_if(istreambuf_iterator<char>{countF}, {}, [](char c) { return c == '\n'; });
    int numOfProducers = (count - 1) / 4;
    countF.close();
    
    BoundedBuffer *bufArr[numOfProducers];
    BoundedBuffer tmpArr[numOfProducers];

    pthread_t tArr[numOfProducers];
    Producer proArr[numOfProducers];

    ifstream readF(argv[1]);
    string line;

// getting the relevant information about each producer -
// num of reports he publishes and the size of it's buffer
    for (int i = 0; i<numOfProducers; i++) {
        getline (readF, line);
        getline (readF, line);
        int numS = stoi(line);
        getline (readF, line);
        line = line.substr(13, line.length() - 1);
        int bufS = stoi(line);
        getline (readF, line);
        
        BoundedBuffer buf(bufS);
        tmpArr[i] = buf;
        Producer pro(i + 1, numS, &tmpArr[i]);
        proArr[i] = pro;
        pthread_t tP;
        tArr[i] = tP;
    }
    readF.close();

//  creating the producers and publishing their reports
    for (int j = 0; j < numOfProducers; j++) {
        bufArr[j] = &tmpArr[j];   
        if (pthread_create(&tArr[j], NULL, produce, &proArr[j]) != 0) {
            cerr << "producer failed. id: " << j+1 << endl;
        }   
    }

//   creating the dispatcher and the rest of the news broadcasting components
    Dispatcher dis(((BoundedBuffer **) &bufArr), numOfProducers, &sport, &weather, &news);

    pthread_t t1, t2, t3, t4, t5;

    if (pthread_create(&t1, NULL, dispatch, &dis) != 0) {
            cout << "create failed: t1" << endl;
    }
    if (pthread_create(&t2, NULL, co, &coS) != 0) {
            cout << "create failed: t2" << endl;
    }
    if (pthread_create(&t3, NULL, co, &coN) != 0) {
            cout << "create failed: t3" << endl;
    }
    if (pthread_create(&t4, NULL, co, &coW) != 0) {
            cout << "create failed: t4" << endl;
    }
    if (pthread_create(&t5, NULL, print, &scr) != 0) {
            cout << "create failed: t5" << endl;
    }

    if (pthread_join(t5, NULL) != 0) {
        cout << "error joining t5" << endl;
        return 1;
    }

    return 0;
}