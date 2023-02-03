#include <stdio.h>
 
 // Write a recursion function to find largest among root, left child, and right child
void heapify(int arr[], int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
 
    if (left < n && arr[left] > arr[largest])
      largest = left;
 
    if (right < n && arr[right] > arr[largest])
      largest = right;
 
    if (largest != i) {// Swap and continue heapifying if root is not largest
        int temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;
        heapify(arr, n, largest);
    }
  }
 
  void heapSort(int arr[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--)// Build max heap
        heapify(arr, n, i);
    for (int i = n - 1; i >= 0; i--) {// Heap sort
        int temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;
        heapify(arr, i, 0);// Heapify root element to get highest element at root again
    }
  }
 
void showArray(int arr[], int size) 
{ 
    int i; 
    for (i=0; i < size; i++) 
        printf("%d ", arr[i]); 
    printf("\n"); 
}
 
int main()
{ 
    int libraryNum[] = {124,235,456,123,756,476,285,998,379,108}; 
    int n = sizeof(libraryNum)/sizeof(libraryNum[0]); 
 
    printf("Initial array: \n"); 
    showArray(libraryNum, n); 
    heapSort(libraryNum, n); 
    printf("Sorted array: \n"); 
    showArray(libraryNum, n); 
    return 0; 
}