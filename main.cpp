#include <iostream>
#include <vector>



class MyClass {
 public:
  MyClass() {};
  ~MyClass() {};

  void add(const std::vector<int> numbers) {
    numbers_.resize(numbers.size());
    for (int index = 0; index < numbers.size(); ++index) {
      numbers_[index] = numbers[index];
    }
  }

  void print() {
    for (int i: numbers_) {
      std::cout << i << std::endl;
    }
  }

 private:
  std::vector<int> numbers_;
};



int main() {
  MyClass mycl;
  std::vector<int> numb({2, 1, 9, 3, 4, 3});
  mycl.add(numb);
  mycl.print();

  return 0;
}