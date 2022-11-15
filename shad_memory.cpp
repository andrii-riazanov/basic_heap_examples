#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

/*
 * Мы реализуем стандартный класс для хранения кучи с возможностью удаления
 * элемента по индексу. Для оповещения элементов об их текущих значениях
 * индексов мы используем функцию index_change_observer.
 */

template <class T, class Compare = std::less<T>>
class MaxHeap {
 public:
  using IndexChangeObserver =
  std::function<void(const T& element, size_t new_element_index)>;

  static constexpr size_t kNullIndex = static_cast<size_t>(-1);

  explicit MaxHeap(
      Compare compare = Compare(),
      IndexChangeObserver index_change_observer = IndexChangeObserver());

  size_t Push(const T& value);
  void Erase(size_t index);
  const T& top() const;
  void Pop();
  size_t size() const;
  bool empty() const;



 private:

  size_t SiftUp(size_t index);
  void SiftDown(size_t index);
  IndexChangeObserver index_change_observer_;
  Compare compare_;
  std::vector<T> elements_;

  size_t parent(size_t index) const;
  size_t left_son(size_t index) const;
  size_t right_son(size_t index) const;

  bool CompareElements(size_t first_index, size_t second_index) const;
  void NotifyIndexChange(const T& element, size_t new_element_index);
  void SwapElements(size_t first_index, size_t second_index);
};

struct MemorySegment {
  size_t left;
  size_t right;
  size_t heap_index;

  MemorySegment(size_t left, size_t right);
  size_t size() const;
  MemorySegment Unite(const MemorySegment& other) const;
};


using MemorySegmentIterator = std::list<MemorySegment>::iterator;
using MemorySegmentConstIterator = std::list<MemorySegment>::const_iterator;

struct MemorySegmentSizeCompare {
  bool operator()(MemorySegmentIterator first,
                  MemorySegmentIterator second) const;
};

using MemorySegmentHeap =
MaxHeap<MemorySegmentIterator, MemorySegmentSizeCompare>;

struct MemorySegmentsHeapObserver {
  void operator()(const MemorySegmentIterator& segment, size_t new_index) const;
};


/*
 * Мы храним сегменты в виде двусвязного списка (std::list).
 * Быстрый доступ к самому левому из наидлиннейших свободных отрезков
 * осуществляется с помощью кучи, в которой (во избежание дублирования
 * отрезков в памяти) хранятся итераторы на список — std::list::iterator.
 * Чтобы быстро определять местоположение сегмента в куче для его изменения,
 * мы внутри сегмента в списке храним heap_index, актуальность которого
 * поддерживаем с помощью index_change_observer. Мы не храним отдельной метки
 * для маркировки занятых сегментов: вместо этого мы кладём в heap_index
 * специальный kNullIndex. Более того, мы скрываем истинный тип
 * MemorySegmentIterator за названием SegmentHandle. Таким образом,
 * пользовательский
 * код абсолютно не зависит того, что мы храним сегменты в списке и в куче,
 * что позволяет нам легко поменять реализацию класса.
 */

class MemoryManager {
 public:
  using SegmentHandle = MemorySegmentIterator;

  explicit MemoryManager(size_t memory_size);
  SegmentHandle Allocate(size_t size);
  void Free(SegmentHandle segment_handle);
  SegmentHandle undefined_handle();

 private:
  MemorySegmentHeap free_memory_segments_;
  std::list<MemorySegment> memory_segments_;

  void AppendIfFree(SegmentHandle remaining, SegmentHandle appending);
};


void MemoryManager::AppendIfFree(SegmentHandle remaining, SegmentHandle appending) {
  if (remaining->heap_index != free_memory_segments_.kNullIndex) {
   if (remaining->right == appending->left) {
     free_memory_segments_.Erase(appending->heap_index);
     memory_segments_.erase(appending);
     remaining->right = appending->right;
     free_memory_segments_.SiftUp(remaining->heap_index);
   } else if (remaining->left == appending->right) {
     free_memory_segments_.Erase(appending->heap_index);
     memory_segments_.erase(appending);
     remaining->left = appending->left;
     free_memory_segments_.SiftUp(remaining->heap_index);
   }
  }
}

void MemoryManager::Free(SegmentHandle segment_handle) {
  free_memory_segments_.Push(segment_handle);
  if (memory_segments_.size() != 1) {
    if (std::next(segment_handle) != memory_segments_.end()) {
      AppendIfFree(std::next(segment_handle), segment_handle);
    }

    if (segment_handle != memory_segments_.begin()) {
      AppendIfFree(std::prev(segment_handle), segment_handle);
    }
  }

//  if (memory_segments_.size() == 1 ||
//      (segment_handle == memory_segments_.begin() &&
//       std::next(segment_handle)->heap_index == free_memory_segments_.kNullIndex) ||
//      (segment_handle == std::prev(memory_segments_.end()) &&
//       std::prev(segment_handle)->heap_index == free_memory_segments_.kNullIndex)  ) {
//    free_memory_segments_.Push(segment_handle);
//  } else if (std::next(segment_handle)->heap_index != free_memory_segments_.kNullIndex &&
//             std::prev(segment_handle)->heap_index == free_memory_segments_.kNullIndex) {
//    std::prev(segment_handle)->right = segment_handle->right;
//    memory_segments_.erase(segment_handle);
//
//  }
}



MemoryManager::SegmentHandle MemoryManager::Allocate(size_t size) {
  SegmentHandle segment_to_truncate = free_memory_segments_.top();
  if (segment_to_truncate->size() < size) {
    return memory_segments_.end();
  } else if (segment_to_truncate->size() == size) {
    free_memory_segments_.Erase(segment_to_truncate->heap_index);
    return segment_to_truncate;
  } else {
    SegmentHandle new_segment = memory_segments_.emplace(segment_to_truncate);
    new_segment->heap_index = free_memory_segments_.kNullIndex;
    new_segment->left = segment_to_truncate->left;
    new_segment->right = new_segment->left + size;
    segment_to_truncate->left += size;
    free_memory_segments_.SiftDown(segment_to_truncate->heap_index);
    return new_segment;
  }
}







size_t ReadMemorySize(std::istream& stream = std::cin);

struct AllocationQuery {
  size_t allocation_size;
};

struct FreeQuery {
  size_t allocation_query_index;
};

/*
 * Для хранения запросов используется специальный класс-обёртка
 * MemoryManagerQuery. Фишка данной реализации в том, что мы можем удобно
 * положить в него любой запрос, при этом у нас есть методы, которые позволят
 * гарантированно правильно проинтерпретировать его содержимое. При реализации
 * нужно воспользоваться тем фактом, что dynamic_cast возвращает nullptr
 * при неудачном приведении указателей.
 */

class MemoryManagerQuery {
 public:
  explicit MemoryManagerQuery(AllocationQuery allocation_query);
  explicit MemoryManagerQuery(FreeQuery free_query);

  const AllocationQuery* AsAllocationQuery() const;
  const FreeQuery* AsFreeQuery() const;

 private:
  class AbstractQuery {
   public:
    virtual ~AbstractQuery() {}

   protected:
    AbstractQuery() {}
  };

  template <typename T>
  struct ConcreteQuery : public AbstractQuery {
    T body;

    explicit ConcreteQuery(T _body) : body(std::move(_body)) {}
  };

  std::unique_ptr<AbstractQuery> query_;
};


std::vector<MemoryManagerQuery> ReadMemoryManagerQueries(
    std::istream& stream = std::cin);



struct MemoryManagerAllocationResponse {
  bool success;
  size_t position;
};

MemoryManagerAllocationResponse MakeSuccessfulAllocation(size_t position);


MemoryManagerAllocationResponse MakeFailedAllocation();


std::vector<MemoryManagerAllocationResponse> RunMemoryManager(
    size_t memory_size, const std::vector<MemoryManagerQuery>& queries);

void OutputMemoryManagerResponses(
    const std::vector<MemoryManagerAllocationResponse>& responses,
    std::ostream& ostream = std::cout);



int main() {
  std::istream& input_stream = std::cin;
  std::ostream& output_stream = std::cout;

  const size_t memory_size = ReadMemorySize(input_stream);
  const std::vector<MemoryManagerQuery> queries =
      ReadMemoryManagerQueries(input_stream);

  const std::vector<MemoryManagerAllocationResponse> responses =
      RunMemoryManager(memory_size, queries);

  OutputMemoryManagerResponses(responses, output_stream);

  return 0;
}






template <class T, class Compare = std::less<T>>
MaxHeap::MaxHeap(Compare compare, IndexChangeObserver index_change_observer)
    : compare_{compare},
      index_change_observer_{index_change_observer} {};

void MaxHeap::SiftDown(size_t index) {
  while (right_son(index) <= size()) {
    size_t index_to_swap = left_son(index);
    if (right_son(index) < size() && CompareElements(left_son(index), right_son(index))) {
      index_to_swap = right_son(index);
    }
    if (!CompareElements(index, index_to_swap)) {
      break;
    }
    SwapElements(index, index_to_swap);
    index = index_to_swap;
  }
}

size_t MaxHeap::SiftUp(size_t index) {
  while (index && CompareElements(parent(index), index)) {
    SwapElements(parent(index), index);
    index = parent(index);
  }
  return index;
}

template <class T, class Compare = std::less<T>>
bool MaxHeap::CompareElements(size_t first_index, size_t second_index) const {
  return compare_(elements_[first_index], elements_[second_index]);
}

template <class T, class Compare = std::less<T>>
void MaxHeap::SwapElements(size_t first_index, size_t second_index) {
  T buff_element = elements_[first_index];
  elements_[first_index] = elements_[second_index];
  elements_[second_index] = buff_element;
  NotifyIndexChange(elements_[first_index], first_index);
  NotifyIndexChange(elements_[second_index], second_index);
}

template <class T, class Compare = std::less<T>>
void MaxHeap::NotifyIndexChange(const T &element, size_t new_element_index) {
  index_change_observer_(element, new_element_index);
}

size_t MaxHeap::parent(size_t index) const {
  return (index - 1) / 2;
}

void MaxHeap::Pop() {
  elements_.pop_back();
}

template <class T>
const T & MaxHeap::top() const {
  return elements_[0];
}

template <class T>
size_t MaxHeap::Push(const T &value) {
  elements_.push_back(value);
  NotifyIndexChange(value, size() - 1);
  SiftUp(size() - 1);
}

void MaxHeap::Erase(size_t index) {
  SwapElements(index, size() - 1);
  NotifyIndexChange(size() - 1, kNullIndex);
  Pop();
  SiftDown(index);
  SiftUp(index);
}

size_t MaxHeap::left_son(size_t index) const {
  return 2 * index + 1;
}

size_t MaxHeap::right_son(size_t index) const {
  return 2 * index + 2;
}

size_t MaxHeap::size() const {
  return elements_.size();
}

bool MaxHeap::empty() const {
  return (size() == 0);
}

template <class T>
size_t MaxHeap::Push(const T &value) {
  elements_.push_back(value);
  NotifyIndexChange(value, size() - 1);
  SiftUp(size() - 1);
}







MemorySegment::MemorySegment(size_t left, size_t right) : left{left}, right{right} {

};

size_t MemorySegment::size() const {
  return right - left;
}

bool MemorySegmentSizeCompare::operator()(MemorySegmentIterator first,
                                          MemorySegmentIterator second) const {
  return (*first).size() < (*second).size();
}

void MemorySegmentsHeapObserver::operator()(const MemorySegmentIterator &segment,
                                            size_t new_index) const {
  segment->heap_index = new_index;
}



MemoryManager::MemoryManager(size_t memory_size) {
  MemorySegmentSizeCompare* size_compare = new MemorySegmentSizeCompare;
  MemorySegmentsHeapObserver* index_observer = new MemorySegmentsHeapObserver;
  free_memory_segments_ = MemorySegmentHeap(*size_compare, index_observer);
  MemorySegment* whole_memory = new MemorySegment(0, memory_size - 1);
  memory_segments_.push_back(*whole_memory);
  free_memory_segments_.Push(--memory_segments_.end());
}






const AllocationQuery* MemoryManagerQuery::AsAllocationQuery() const {
  return &(dynamic_cast<ConcreteQuery<AllocationQuery>*>(query_)->body);
}

const FreeQuery* MemoryManagerQuery::AsFreeQuery() const {
  return &(dynamic_cast<ConcreteQuery<FreeQuery>*>(query_)->body);
}

MemoryManagerQuery::MemoryManagerQuery(AllocationQuery allocation_query) {
  query_ = new ConcreteQuery<AllocationQuery>(allocation_query);
}

MemoryManagerQuery::MemoryManagerQuery(FreeQuery allocation_query) {
  query_ = new ConcreteQuery<FreeQuery>(allocation_query);
}





std::vector<MemoryManagerQuery> ReadMemoryManagerQueries(
    std::istream& stream) {
  size_t number_of_queries = 0;
  stream >> number_of_queries;

  std::vector<MemoryManagerQuery> vector_of_queries(number_of_queries);

  for (int number_of_query = 0; number_of_query < number_of_queries; ++number_of_query) {
    int current_query;
    stream >> current_query;
    if (current_query > 0) {
      AllocationQuery allocation_query;
      allocation_query.allocation_size = static_cast<uint64_t>(current_query);
      vector_of_queries[number_of_query].MemoryManagerQuery(allocation_query);
    } else {
      FreeQuery free_query;
      free_query.allocation_query_index = static_cast<uint64_t>(- current_query);
      vector_of_queries[number_of_query].MemoryManagerQuery(free_query);
    }
  }

  return vector_of_queries;
}




size_t ReadMemorySize(std::istream& stream = std::cin) {
  size_t size;
  stream >> size;
  return size;
};





MemoryManagerAllocationResponse MakeSuccessfulAllocation(size_t position) {
  MemoryManagerAllocationResponse success_response;
  success_response.success = true;
  success_response.position = position;
  return success_response;
}

MemoryManagerAllocationResponse MakeFailedAllocation() {
  MemoryManagerAllocationResponse failed_response;
  failed_response.success = false;
  return failed_response;
}


void OutputMemoryManagerResponses(
    const std::vector<MemoryManagerAllocationResponse>& responses,
    std::ostream& ostream) {
  for (const MemoryManagerAllocationResponse& response: responses) {
    if (response.success) {
      ostream << response.position << "\n";
    } else {
      ostream << "-1\n";
    }
  }
}
