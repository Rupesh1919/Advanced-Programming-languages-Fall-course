#include <iostream>
using namespace std;

int main() {
    int size = 5;
    int* data = new int[size];        // manual allocation
    int total = 0;
    for (int i = 0; i < size; i++) { data[i] = i + 1; total += data[i]; }
    cout << "Sum: " << total << endl;
    delete[] data;                    // must free manually
    data = nullptr;                   // avoid dangling pointer
}