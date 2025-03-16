//Adham Makar
#include "Heap.h"


int main() {
    IndPQ pq;
    pq.insert("Task1", 5);
    pq.insert("Task2", 3);
    pq.insert("Task3", 4);
    pq.insert("Task4", 1);
    pq.insert("Task5", 9);
    pq.insert("Task6", 2);
    pq.display();
    pq.ddisplay();

    cout << "size: " << pq.size() << endl;
    cout << "Min Task: " << pq.deleteMin() << endl; // Should print "Task4"
    pq.updatePriority("Task1", 8);
    cout << "Min Task: " << pq.deleteMin() << endl; // Should print "Task2"
    cout << "Empty?: " << pq.isEmpty() << endl;
    cout << "size: " << pq.size() << endl << endl;
    pq.display();
    pq.ddisplay();

    pq.insert("Task4", 10);
    pq.insert("Task2", 3);
    pq.display();
    pq.ddisplay();

    cout << "size: " << pq.size() << endl;
    pq.remove("Task1");   
    cout << "size: " << pq.size() << endl;
    pq.clear();
    cout << "size: " << pq.size() << endl;
    cout << "Empty?: " << pq.isEmpty() << endl;
    pq.display();
    pq.ddisplay();


    return 0;
}