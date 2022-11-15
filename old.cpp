#include <iostream>
#include <vector>


class Segment {
 public:
  Segment() {};
  void Set(int leftt, int rightt, bool freee, int heap, Segment* prevseg, Segment* nextseg) {
    left = leftt;
    right = rightt;
    heap_position = heap;
    next = nextseg;
    prev = prevseg;
    free = freee;
    lengthh = right - left;
  }

  int left;
  int right;
  int lengthh;
  int heap_position;
  bool free;
  Segment* next;
  Segment* prev;

  void reset() {
    lengthh = right - left;
  }

  int length() const {
    return right - left;
  }

  bool operator<(const Segment other) const {
    if (length() == other.length()) {
      return left > other.left;
    }
    return (length() < other.length());
  }
};

class Heap {
 public:
  Heap() {};

  void SiftUp(int position) {
    while (position && *heap_[(position - 1) / 2] < *heap_[position]) {
      Swap((position - 1) / 2, position);
      position = (position - 1) / 2;
    }
  };

  void SiftDown(int position) {
    while (2 * position + 2 <= heap_.size()) {
      int position_to_swap = 2 * position + 1;
      if (2 * position + 2 < heap_.size() && *heap_[2 * position + 1] < *heap_[2 * position + 2]) {
        position_to_swap = 2 * position + 2;
      }
      if (!(*heap_[position] < *heap_[position_to_swap])) {
        break;
      }
      Swap(position, position_to_swap);
      position = position_to_swap;
    }
  };

  void Swap(int first, int second) {
    heap_[first]->heap_position = second;
    heap_[second]->heap_position = first;
    Segment* temp = heap_[first];
    heap_[first] = heap_[second];
    heap_[second] = temp;
  }

  void Insert(Segment* element) {
    heap_.push_back(element);
    element->heap_position = static_cast<int>(heap_.size() - 1);
    SiftUp(static_cast<int>(heap_.size() - 1));
  };

  void Remove(int position) {
    Swap(position, static_cast<int>(heap_.size() - 1));
    heap_[static_cast<int>(heap_.size() - 1)]->heap_position = -1;
    heap_.pop_back();
    SiftDown(position);
    SiftUp(position);
  };

//  void Remove(int position) {
//    int position_to_swap = position;
//    while (2 * position_to_swap + 1 < heap_.size()) {
//      position_to_swap = 2 * position_to_swap + 1;
//    }
//    Swap(position, position_to_swap);
//    heap_[position_to_swap]->heap_position = -1;
//    heap_.pop_back();
//    SiftDown(position);
//  };

  Segment* Max() {
    if (heap_.size()) {
      return heap_[0];
    } else {
      return nullptr;
    }
  };

 private:
  std::vector<Segment*> heap_;
};


int main() {

  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int Nmemory = 0, Mqueries = 0;
  std::cin >> Nmemory >> Mqueries;

  Segment* wholemem = new(Segment);
  Segment* fictive_first = new(Segment);
  Segment* fictive_last = new(Segment);
  wholemem->Set(0, Nmemory, true, -1, fictive_first, fictive_last);
  fictive_first->Set(0, 0, false, -1, fictive_first, wholemem);
  fictive_last->Set(Nmemory, Nmemory, false, -1, wholemem, fictive_last);
  Heap heap;
  heap.Insert(wholemem);

  std::vector<Segment*> vect_queries;
  std::vector<int> queries;

  for (int numb_of_query = 0; numb_of_query < Mqueries; ++numb_of_query) {
    int numb;
    std::cin >> numb;
    queries.push_back(numb);
  }

  for (int numb_of_query = 0; numb_of_query < Mqueries; ++numb_of_query) {
    int query = queries[numb_of_query];
    if (query > 0) {
      Segment* segment_to_short = heap.Max();
      if (!segment_to_short || segment_to_short->length() < query) {
        std::cout << "-1\n";
        vect_queries.push_back(nullptr);
      } else if (segment_to_short->length() == query) {
        segment_to_short->free = false;
        heap.Remove(segment_to_short->heap_position);
        vect_queries.push_back(segment_to_short);
        std::cout << segment_to_short->left + 1 << '\n';
      } else {
        Segment* new_segment = new(Segment);
        vect_queries.push_back(new_segment);
        new_segment->Set(segment_to_short->left, segment_to_short->left + query, false,
                         -1, segment_to_short->prev, segment_to_short);
        segment_to_short->left = segment_to_short->left + query;
        segment_to_short->prev->next = new_segment;
        segment_to_short->prev = new_segment;
        segment_to_short->reset();
        std::cout << new_segment->left + 1 << '\n';
        heap.SiftDown(segment_to_short->heap_position);
      }
    } else {
      vect_queries.push_back(nullptr);
      if (vect_queries[- query - 1] != nullptr) {
        Segment *segment_to_free = vect_queries[-query - 1];
        if (!segment_to_free->next->free && !segment_to_free->prev->free) {
          segment_to_free->free = true;
          heap.Insert(segment_to_free);
        } else if (!segment_to_free->next->free && segment_to_free->prev->free) {
          segment_to_free->prev->next = segment_to_free->next;
          segment_to_free->next->prev = segment_to_free->prev;
          segment_to_free->prev->right = segment_to_free->right;
          segment_to_free->prev->reset();
          heap.SiftUp(segment_to_free->prev->heap_position);
          delete (segment_to_free);
        } else if (segment_to_free->next->free && !segment_to_free->prev->free) {
          segment_to_free->next->prev = segment_to_free->prev;
          segment_to_free->prev->next = segment_to_free->next;
          segment_to_free->next->left = segment_to_free->left;
          segment_to_free->next->reset();
          heap.SiftUp(segment_to_free->next->heap_position);
          delete (segment_to_free);
        } else {
          segment_to_free->prev->next = segment_to_free->next->next;
          segment_to_free->next->next->prev = segment_to_free->prev;
          heap.Remove(segment_to_free->next->heap_position);
          segment_to_free->prev->right = segment_to_free->next->right;
          heap.SiftUp(segment_to_free->prev->heap_position);
          segment_to_free->prev->reset();
          delete (segment_to_free->next);
          delete (segment_to_free);
        }
      }
    }
  }

  Segment* current = fictive_first->next;
  while (current != fictive_last) {
    Segment* next = current->next;
    delete(current);
    current = next;
  }
  delete(fictive_first);
  delete(fictive_last);

  return 0;
}
