#pragma once
#include "request_handler.h"

long constexpr arrSize = 1000000;
size_t constexpr threadCount = 10;
RequestHandler rh;

void QuickSortPool(std::shared_ptr<int[]> array, long left, long right)
{
  if (left >= right)
    return;
  long leftBound = left;
  long rightBound = right;
  long middle = array[(static_cast<ptrdiff_t>(leftBound) + rightBound) / 2];
  do
  {
    while (array[leftBound] < middle)
      ++leftBound;
    while (array[rightBound] > middle)
      --rightBound;
    if (leftBound <= rightBound)
    {
      std::swap(array[leftBound], array[rightBound]);
      ++leftBound;
      --rightBound;
    }
  } while (leftBound <= rightBound);

  if (rightBound - left > 100000)
  {
    std::packaged_task<void()>task([=] ()
      { rh.PushRequest(QuickSortPool, array, left, rightBound); });
    QuickSortPool(array, leftBound, right);
    std::future<void> fut = task.get_future();
    task();
  }
  else
  {
    QuickSortPool(array, left, rightBound);
    QuickSortPool(array, leftBound, right);
  }
}
