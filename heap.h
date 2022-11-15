#include <bits/stdc++.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
using namespace std;


static constexpr int kNullIndex = -1;

template <class T, class Compare = less<T>>
class MaxHeap {
 public:

  int Push(T* value) {
    elements_.push_back(value);
    NotifyIndexChange(value, size() - 1);
    SiftUp(size() - 1);
    return size();
  }

  void Erase(int index) {
    if (index == -1) {
      cout << "Trying to erase something not in the heap\n";
      return;
    }
    assert(index >= 0);
    assert(index < size());
    SwapElements(index, size() - 1);
    NotifyIndexChange(elements_[size() - 1], kNullIndex);
    PopBack();
    SiftDown(index);
    SiftUp(index);
  }

  void Modify(int index) {
    if (index == -1) {
      cout << "Trying to modify something not in the heap\n";
      return;
    }
    assert(index >= 0);
    assert(index < size());
    SiftDown(index);
    SiftUp(index);
  }

  T* PopTop() {
    if (size()) {
      T* to_return = top();
      Erase(0);
      return to_return;
    } else {
      return nullptr;
    }
  }

  T* top() const {
    if (size()) {
      return elements_[0];
    } else {
      return nullptr;
    }
  }

  void PopBack() {
    elements_.pop_back();
  }

  int size() const {
    return elements_.size();
  }

  bool empty() const {
    return (size() == 0);
  }

  int SiftUp(int index) {
    while (index && CompareElements(parent(index), index)) {
      SwapElements(parent(index), index);
      index = parent(index);
    }
    return index;
  }

  void SiftDown(int index) {
    while (right_son(index) <= size()) {
      int index_to_swap = left_son(index);
      if (right_son(index) < size() && CompareElements(left_son(index), right_son(index))) {
        index_to_swap = right_son(index);
      }
      if (CompareElements(index_to_swap, index)) {
        break;
      }
      SwapElements(index, index_to_swap);
      index = index_to_swap;
    }
  }

  Compare compare_;
  vector<T*> elements_;

  MaxHeap(Compare compare = Compare()) : compare_(compare) {}

  int parent(int index) const {
    return (index - 1) / 2;
  }

  int left_son(int index) const {
    return 2 * index + 1;
  }
  int right_son(int index) const {
    return 2 * index + 2; 
  }

  bool CompareElements(int first_index, int second_index) const {
    return compare_(elements_[first_index], elements_[second_index]);
  }

  void NotifyIndexChange(T* element, int new_element_index) {
    element->heap_position = new_element_index;
  }

  void SwapElements(int first_index, int second_index) {
    T* buff_element = elements_[first_index];
    elements_[first_index] = elements_[second_index];
    elements_[second_index] = buff_element;
    NotifyIndexChange(elements_[first_index], first_index);
    NotifyIndexChange(elements_[second_index], second_index);
  }
};

// struct MyStruct {
//   int data;
//   int second_data;

//   int heap_position;

//   MyStruct(int data1 = 0, int data2 = 5) : data{data1}, second_data{data2} {
//     heap_position = -1;
//   }
// };

// struct CompareMyStructs {
//   bool operator()(const MyStruct* first, const MyStruct* second) const {
//     return first->data < second->data;
//   }
// };

// 
// It is MaxHeap is CompareMyStructs returns true when P(first) < P(second)
// To obtain MinHeap, CompareMyStruct must return true when P(second) < P(first)
// 

// using Heap = MaxHeap<MyStruct, CompareMyStructs>;




// 
// 
// Code to check the heap follows.
// 
// 

// #include "printheap.h"
// void Print(const Heap& heap) {
//   vector<int> temp;
//   for (int i = 0; i < heap.size(); i++) {
//     temp.push_back(heap.elements_[i]->data);
//   }
//   printheap(temp);
// }


// int main() {

//   string input;

//   bool flag = true;
//   int temp;

//   int N = 20;

//   vector<MyStruct> struct_vect;

//   for (int i = 0; i < 20; i++) {
//     MyStruct temp(i%10);
//     struct_vect.push_back(temp);
//   }

//   Heap heap;

//   heap.top();

//   while (flag) {
//     cin >> input;
//     if (input == "q") {
//       flag = 0;
//     }
//     else if (input == "push") {
//       cin >> temp;
//       assert(0 <= temp);
//       assert(temp < N);
//       heap.Push(&struct_vect[temp]);
//       Print(heap);
//     }
//     else if (input == "pop") {
//       MyStruct* tempp = heap.PopTop();
//       Print(heap);
//       if (tempp == nullptr) {
//         cout << "Nothing to pop " << -1 << endl;
//       } else {
//         cout << "Max was " << tempp->data << endl;
//       }
//       heap.top();
//     }
//     else if (input == "del") {
//       cin >> temp;
//       assert(0 <= temp);
//       assert(temp < N);
//       heap.Erase(struct_vect[temp].heap_position);
//       Print(heap);
//     } else if (input == "mod") {
//       int new_val;
//       cin >> temp >> new_val;
//       struct_vect[temp].data = new_val;
//       heap.Modify(struct_vect[temp].heap_position);
//       Print(heap);
//     }
//   }
//   return 0;
// }




